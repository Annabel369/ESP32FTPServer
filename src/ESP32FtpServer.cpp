#include "ESP32FtpServer.h"
#include <WiFi.h>

WiFiServer ftpServer(FTP_CTRL_PORT);
WiFiServer dataServer(FTP_DATA_PORT_PASV);

FtpServer::FtpServer() {}

void FtpServer::begin(String uname, String pword) {
  _FTP_USER = uname; 
  _FTP_PASS = pword;
  if (!SD.begin()) Serial.println("Erro SD!");
  ftpServer.begin();
  dataServer.begin();
  millisTimeOut = (uint32_t)FTP_TIME_OUT * 60 * 1000;
  iniVariables();
}

void FtpServer::iniVariables() {
  dataPort = FTP_DATA_PORT_PASV;
  memset(cwdName, 0, sizeof(cwdName));
  strcpy(cwdName, "/");
  transferStatus = 0; rnfrCmd = false; iCL = 0;
}

int FtpServer::handleFTP() {
  if (ftpServer.hasClient()) {
    client.stop();
    client = ftpServer.available();
    clientConnected();
  }
  if (cmdStatus > 1 && !client.connected()) {
    cmdStatus = 1; return 0;
  }
  if (readChar() > 0) {
    if (cmdStatus == 3) { if (userIdentity()) cmdStatus = 4; } 
    else if (cmdStatus == 4) { if (userPassword()) cmdStatus = 5; } 
    else if (cmdStatus == 5) { processCommand(); }
    millisEndConnection = millis() + millisTimeOut;
  }
  if (transferStatus == 1) { if (!doRetrieve()) transferStatus = 0; }
  else if (transferStatus == 2) { if (!doStore()) transferStatus = 0; }
  if (cmdStatus > 2 && millis() > millisEndConnection) {
    client.println("530 Timeout."); client.stop(); cmdStatus = 1;
  }
  return (cmdStatus > 1);
}

void FtpServer::clientConnected() {
  client.println("220 ESP32 FTP 1.0.7 Stable");
  strcpy(cwdName, "/"); cmdStatus = 3; 
  millisEndConnection = millis() + 15000;
}

boolean FtpServer::userIdentity() {
  if (strcmp(command, "USER") == 0) {
    if (_FTP_USER.equals(parameters)) { client.println("331 Password required"); return true; }
    client.println("530 User not found");
  }
  return false;
}

boolean FtpServer::userPassword() {
  if (strcmp(command, "PASS") == 0) {
    if (_FTP_PASS.equals(parameters)) { client.println("230 Logged in."); return true; }
    client.println("530 Login incorrect");
  }
  return false;
}

boolean FtpServer::processCommand() {
  char path[300]; 
  if (!strcmp(command, "PWD") || !strcmp(command, "XPWD")) {
    client.printf("257 \"%s\"\r\n", cwdName);
  }
  else if (!strcmp(command, "PASV")) {
    dataIp = WiFi.localIP();
    client.printf("227 Entering Passive Mode (%u,%u,%u,%u,%u,%u)\r\n", 
                  dataIp[0], dataIp[1], dataIp[2], dataIp[3], dataPort >> 8, dataPort & 255);
  }
  else if (!strcmp(command, "CWD")) {
    if (makePath(path)) {
      File dir = SD.open(path);
      if (dir && dir.isDirectory()) {
        strncpy(cwdName, path, sizeof(cwdName)-1);
        if (cwdName[strlen(cwdName)-1] != '/') strncat(cwdName, "/", sizeof(cwdName)-strlen(cwdName)-1);
        client.printf("250 CWD successful. %s\r\n", cwdName);
      } else { client.println("550 Directory not found"); }
      if (dir) dir.close();
    } else { client.println("550 Invalid path"); }
  }
  else if (!strcmp(command, "LIST") || !strcmp(command, "NLST")) {
    if (dataConnect()) {
      client.println("150 Accepted data connection");
      File root = SD.open(cwdName);
      File entry;
      while (entry = root.openNextFile()) {
        String name = String(entry.name());
        int lastSlash = name.lastIndexOf('/');
        if (lastSlash != -1) name = name.substring(lastSlash + 1);
        if (entry.isDirectory()) data.printf("drwxr-xr-x 1 owner group 0 Jan 16 2026 %s\r\n", name.c_str());
        else data.printf("-rw-r--r-- 1 owner group %u Jan 16 2026 %s\r\n", (unsigned)entry.size(), name.c_str());
        entry.close(); yield();
      }
      root.close(); data.stop(); client.println("226 List done");
    }
  }
  else if (!strcmp(command, "RETR")) {
    if (makePath(path)) {
      file = SD.open(path, "r");
      if (file && !file.isDirectory() && dataConnect()) {
        client.printf("150 %u bytes\r\n", (unsigned)file.size());
        transferStatus = 1; millisBeginTrans = millis(); bytesTransfered = 0;
      } else { client.println("550 File not found"); }
    } else { client.println("550 Invalid name"); }
  }
  else if (!strcmp(command, "RNFR")) {
    rnfrCmd = false;
    if (makePath(rnfrName)) {
      if (SD.exists(rnfrName)) { client.println("350 OK"); rnfrCmd = true; }
      else { client.println("550 Not found"); }
    } else { client.println("550 Invalid name"); }
  }
  else if (!strcmp(command, "RNTO")) {
    if (rnfrCmd && makePath(path)) {
      if (SD.rename(rnfrName, path)) client.println("250 Renamed");
      else client.println("550 Failed");
    } else { client.println("503 Bad sequence"); }
    rnfrCmd = false;
  }
  else if (!strcmp(command, "STOR")) {
    if (makePath(path)) {
      file = SD.open(path, "w");
      if (file && dataConnect()) {
        client.println("150 OK"); transferStatus = 2;
        millisBeginTrans = millis(); bytesTransfered = 0;
      } else { client.println("451 SD Error"); if(file) file.close(); }
    } else { client.println("553 Invalid filename (No ':' allowed)"); }
  }
  else if (!strcmp(command, "DELE")) {
    if (makePath(path)) {
      if (SD.remove(path)) client.println("250 Deleted");
      else client.println("550 Failed");
    } else { client.println("550 Invalid name"); }
  }
  else if (!strcmp(command, "QUIT")) {
    client.println("221 Goodbye"); client.stop(); cmdStatus = 1; return false;
  }
  else { client.println("200 OK"); }
  return true;
}

boolean FtpServer::doRetrieve() {
  if (file && file.available()) {
    static uint8_t buf[FTP_BUF_SIZE];
    int nb = file.read(buf, sizeof(buf));
    if (nb > 0) data.write(buf, nb);
    yield(); return true;
  }
  closeTransfer(); return false;
}

boolean FtpServer::doStore() {
  if (data.connected() && data.available()) {
    static uint8_t buf[FTP_BUF_SIZE];
    int nb = data.read(buf, sizeof(buf));
    if (nb > 0) file.write(buf, nb);
    yield(); return true;
  } else if (!data.connected()) {
    closeTransfer(); return false;
  }
  return true;
}

void FtpServer::closeTransfer() {
  if (file) file.close();
  data.stop(); delay(5);
  transferStatus = 0;
  client.println("226 Done");
}

boolean FtpServer::dataConnect() {
  unsigned long start = millis();
  while (!dataServer.hasClient() && millis() - start < 5000) yield();
  if (dataServer.hasClient()) { data = dataServer.available(); return true; }
  return false;
}

int8_t FtpServer::readChar() {
  if (!client.available()) return -1;
  char c = client.read();
  if (c == '\r') return 0;
  if (c != '\n') {
    if (iCL < FTP_CMD_SIZE - 1) cmdLine[iCL++] = c;
    return 0;
  }
  cmdLine[iCL] = 0; iCL = 0;
  parameters = strchr(cmdLine, ' ');
  if (parameters) { *parameters = 0; parameters++; }
  strncpy(command, cmdLine, 5); command[5] = 0;
  for (int i = 0; command[i]; i++) command[i] = toupper(command[i]);
  return 1;
}

boolean FtpServer::makePath(char *fullName) {
  memset(fullName, 0, 300);
  if (parameters == NULL || strlen(parameters) == 0) return false;
  // BLOQUEIO CRÍTICO: Impede que o SD tente abrir nomes com ':' e trave o ESP32
  if (strchr(parameters, ':')) return false; 

  if (parameters[0] == '/') strncpy(fullName, parameters, 299);
  else {
    strncpy(fullName, cwdName, 299);
    if (fullName[strlen(fullName)-1] != '/') strncat(fullName, "/", 299-strlen(fullName));
    strncat(fullName, parameters, 299-strlen(fullName));
  }
  char *doubleSlash;
  while ((doubleSlash = strstr(fullName, "//")) != NULL) {
    ::memmove(doubleSlash, doubleSlash + 1, strlen(doubleSlash));
  }
  return true;
}

void FtpServer::disconnectClient() { client.stop(); cmdStatus = 1; }
