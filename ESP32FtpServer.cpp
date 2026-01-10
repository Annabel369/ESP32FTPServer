#include "ESP32FtpServer.h"
#include <WiFi.h>
#include <FS.h>
#include "SD.h"

WiFiServer ftpServer(FTP_CTRL_PORT);
WiFiServer dataServer(FTP_DATA_PORT_PASV);

FtpServer::FtpServer() {}

void FtpServer::begin(String uname, String pword) {
  _FTP_USER = uname;
  _FTP_PASS = pword;

  if (!SD.begin()) {
    Serial.println("Falha ao montar o SD!");
  }

  ftpServer.begin();
  dataServer.begin();
  
  millisTimeOut = (uint32_t)FTP_TIME_OUT * 60 * 1000;
  cmdStatus = 0;
  iniVariables();
}

void FtpServer::iniVariables() {
  dataPort = FTP_DATA_PORT_PASV;
  strcpy(cwdName, "/");
  transferStatus = 0;
  rnfrCmd = false;
  iCL = 0;
}

int FtpServer::handleFTP() {
  if (ftpServer.hasClient()) {
    client.stop();
    client = ftpServer.available();
    clientConnected();
  }

  if (cmdStatus > 1 && !client.connected()) {
    strcpy(cwdName, "/"); // Reset de segurança ao desconectar
    cmdStatus = 1;
    return 0;
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
    client.println("530 Timeout.");
    client.stop();
    cmdStatus = 1;
  }

  return (cmdStatus > 1);
}

void FtpServer::clientConnected() {
  client.println("220 Welcome to ESP32 FTP Server");
  strcpy(cwdName, "/"); 
  cmdStatus = 3; 
  millisEndConnection = millis() + 10000;
}

boolean FtpServer::userIdentity() {
  if (strcmp(command, "USER") == 0) {
    if (_FTP_USER.equals(parameters)) {
      client.println("331 Password required");
      return true;
    }
    client.println("530 User not found");
  }
  return false;
}

boolean FtpServer::userPassword() {
  if (strcmp(command, "PASS") == 0) {
    if (_FTP_PASS.equals(parameters)) {
      client.println("230 Logged in.");
      return true;
    }
    client.println("530 Login incorrect");
  }
  return false;
}

boolean FtpServer::processCommand() {
  if (!strcmp(command, "PWD") || !strcmp(command, "XPWD")) {
    client.printf("257 \"%s\"\r\n", cwdName);
  }
  else if (!strcmp(command, "PASV")) {
    dataIp = WiFi.localIP();
    client.printf("227 Entering Passive Mode (%u,%u,%u,%u,%u,%u)\r\n", 
                  dataIp[0], dataIp[1], dataIp[2], dataIp[3], dataPort >> 8, dataPort & 255);
  }
  else if (!strcmp(command, "CWD")) {
    char path[128];
    makePath(path);
    if (strlen(path) == 0) strcpy(path, "/");

    File dir = SD.open(path);
    if (dir && dir.isDirectory()) {
      strcpy(cwdName, path);
      if (cwdName[strlen(cwdName) - 1] != '/') strcat(cwdName, "/");
      client.printf("250 CWD successful. Current directory is %s\r\n", cwdName);
    } else { client.println("550 Directory not found"); }
    if (dir) dir.close();
  }
  else if (!strcmp(command, "CDUP")) {
    if (strcmp(cwdName, "/") != 0) {
      int len = strlen(cwdName);
      if (cwdName[len-1] == '/') cwdName[len-1] = 0;
      char * lastSlash = strrchr(cwdName, '/');
      if (lastSlash) *(lastSlash + 1) = 0;
      else strcpy(cwdName, "/");
    }
    client.printf("250 CDUP successful. Current directory is %s\r\n", cwdName);
  }
  else if (!strcmp(command, "LIST") || !strcmp(command, "NLST")) {
    if (dataConnect()) {
      client.println("150 Accepted data connection");
      File root = SD.open(cwdName);
      File entry;
      int count = 0;
      while (entry = root.openNextFile()) {
        String name = String(entry.name());
        int lastSlash = name.lastIndexOf('/');
        if (lastSlash != -1) name = name.substring(lastSlash + 1);
        
        if (entry.isDirectory()) data.printf("drwxr-xr-x 1 owner group %8u Jan 01 2026 %s\r\n", 0, name.c_str());
        else data.printf("-rw-r--r-- 1 owner group %8u Jan 01 2026 %s\r\n", (unsigned)entry.size(), name.c_str());
        entry.close();
        count++;
      }
      root.close();
      data.stop();
      client.printf("226 %d matches total\r\n", count);
    }
  }
  else if (!strcmp(command, "RETR")) {
    char path[128];
    makePath(path);
    file = SD.open(path, "r");
    if (file && !file.isDirectory() && dataConnect()) {
      client.printf("150 %u bytes to download\r\n", (unsigned)file.size());
      transferStatus = 1;
      millisBeginTrans = millis();
      bytesTransfered = 0;
    } else { client.println("550 File not found"); }
  }
  else if (!strcmp(command, "STOR")) {
    char path[128];
    makePath(path);
    file = SD.open(path, "w"); // "w" sobrescreve e resolve duplicatas ao salvar
    if (file && dataConnect()) {
      client.println("150 Ok to send");
      transferStatus = 2;
      millisBeginTrans = millis();
      bytesTransfered = 0;
    } else { client.println("451 Error"); }
  }
  else if (!strcmp(command, "DELE")) { // NOVO: Deletar arquivos
    char path[128];
    makePath(path);
    if (SD.remove(path)) client.println("250 File deleted");
    else client.println("550 Delete failed");
  }
  else if (!strcmp(command, "MKD")) {
    char path[128];
    makePath(path);
    if (SD.mkdir(path)) client.printf("257 \"%s\" created\r\n", parameters);
    else client.println("550 Create directory failed");
  }
  else if (!strcmp(command, "QUIT")) {
    client.println("221 Goodbye");
    client.stop();
    strcpy(cwdName, "/"); 
    cmdStatus = 1;
    return false;
  }
  else { client.println("200 OK"); }
  return true;
}

boolean FtpServer::doRetrieve() {
  if (file && file.available()) {
    static uint8_t videoBuf[1024]; 
    int nb = file.read(videoBuf, sizeof(videoBuf));
    if (nb > 0) {
      data.write(videoBuf, nb);
      bytesTransfered += nb;
    }
    yield(); 
    return true;
  }
  closeTransfer();
  return false;
}

boolean FtpServer::doStore() {
  if (data.connected()) {
    static uint8_t videoBuf[1024];
    if (data.available()) {
      int nb = data.read(videoBuf, sizeof(videoBuf));
      if (nb > 0) {
        file.write(videoBuf, nb);
        bytesTransfered += nb;
      }
    }
    yield(); 
    return true;
  }
  closeTransfer();
  return false;
}

void FtpServer::closeTransfer() {
  uint32_t deltaT = millis() - millisBeginTrans;
  if (deltaT > 0 && bytesTransfered > 0) {
    client.printf("226-File successfully transferred\r\n");
    client.printf("226 %u ms, %.2f kbytes/s\r\n", deltaT, (float)bytesTransfered / deltaT);
  } else { client.println("226 File successfully transferred"); }
  if (file) file.close();
  data.stop();
  transferStatus = 0;
}

boolean FtpServer::dataConnect() {
  unsigned long start = millis();
  while (!dataServer.hasClient() && millis() - start < 5000) yield();
  if (dataServer.hasClient()) {
    data = dataServer.available();
    return true;
  }
  return false;
}

int8_t FtpServer::readChar() {
  if (!client.available()) return -1;
  char c = client.read();
  if (c == '\r') return 0;
  if (c != '\n') {
    if (iCL < FTP_CMD_SIZE) cmdLine[iCL++] = c;
    return 0;
  }
  cmdLine[iCL] = 0;
  iCL = 0;
  parameters = strchr(cmdLine, ' ');
  if (parameters) { *parameters = 0; parameters++; }
  strcpy(command, cmdLine);
  for (int i = 0; command[i]; i++) command[i] = toupper(command[i]);
  return 1;
}

boolean FtpServer::makePath(char *fullName) {
  if (parameters == NULL || strlen(parameters) == 0 || strcmp(parameters, "/") == 0) {
    strcpy(fullName, "/");
    return true;
  }
  if (parameters[0] == '/') strcpy(fullName, parameters);
  else {
    strcpy(fullName, cwdName);
    if (fullName[strlen(fullName) - 1] != '/') strcat(fullName, "/");
    strcat(fullName, parameters);
  }
  char *doubleSlash;
  while ((doubleSlash = strstr(fullName, "//")) != NULL) {
    memmove(doubleSlash, doubleSlash + 1, strlen(doubleSlash));
  }
  return true;
}

void FtpServer::disconnectClient() { client.stop(); cmdStatus = 1; }
void FtpServer::abortTransfer() { if (transferStatus > 0) { file.close(); data.stop(); } transferStatus = 0; }