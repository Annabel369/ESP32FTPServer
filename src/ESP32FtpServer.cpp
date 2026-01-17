#include "ESP32FtpServer.h"
#include "ESP32FtpServerCert.h"

WiFiServer ftpServer(FTP_CTRL_PORT);
WiFiServer dataServer(FTP_DATA_PORT_PASV);

FtpServer::FtpServer() {
  clientPtr = nullptr;
}

void FtpServer::begin(String uname, String pword) {
  _FTP_USER = uname; 
  _FTP_PASS = pword;
  if (!SD.begin()) {
    Serial.println("Erro SD!");
  } else {
    checkCertFolder();
  }
  ftpServer.begin();
  dataServer.begin();
  millisTimeOut = (uint32_t)FTP_TIME_OUT * 60 * 1000;
  iniVariables();
}

void FtpServer::checkCertFolder() {
  if (!SD.exists("/cert")) SD.mkdir("/cert");
  if (!SD.exists("/cert/cert.crt")) {
    File f = SD.open("/cert/cert.crt", FILE_WRITE);
    if(f) { f.print(server_crt); f.close(); }
  }
  if (!SD.exists("/cert/key.key")) {
    File f = SD.open("/cert/key.key", FILE_WRITE);
    if(f) { f.print(server_key); f.close(); }
  }
}

void FtpServer::iniVariables() {
  dataPort = FTP_DATA_PORT_PASV;
  memset(cwdName, 0, sizeof(cwdName));
  strcpy(cwdName, "/");
  transferStatus = 0; 
  rnfrCmd = false; 
  iCL = 0;
  isSecure = false;
  cmdStatus = 1; 
}

int FtpServer::handleFTP() {
  if (ftpServer.hasClient()) {
    // Se houver cliente antigo, mata sem dó nem aviso 421
    if (clientPtr) { 
      clientPtr->stop(); 
      delete clientPtr; 
      clientPtr = nullptr;
    }
    clientPtr = new WiFiClient(ftpServer.available());
    clientPtr->setNoDelay(true);
    
    // RESETA O ESTADO para o novo cliente conseguir logar do zero
    cmdStatus = 3; 
    transferStatus = 0;
    iCL = 0;
    clientConnected();
  }

  // Se o cliente desconectar sozinho
  if (cmdStatus > 1 && (clientPtr == nullptr || !clientPtr->connected())) {
    iniVariables();
    cmdStatus = 1; 
    return 0;
  }

  if (clientPtr && clientPtr->available() > 0) {
    if (readChar() > 0) {
      if (cmdStatus == 3) { if (userIdentity()) cmdStatus = 4; } 
      else if (cmdStatus == 4) { if (userPassword()) cmdStatus = 5; } 
      else if (cmdStatus == 5) { processCommand(); }
      millisEndConnection = millis() + millisTimeOut;
    }
  }

  if (transferStatus == 1) { if (!doRetrieve()) transferStatus = 0; }
  else if (transferStatus == 2) { if (!doStore()) transferStatus = 0; }
  
  if (cmdStatus > 2 && millis() > millisEndConnection) {
    if(clientPtr) { clientPtr->println("530 Timeout."); clientPtr->stop(); }
    cmdStatus = 1;
  }
  return (cmdStatus > 1);
}

void FtpServer::clientConnected() {
  if(clientPtr) {
    clientPtr->println("220 ESP32 FTP 1.0.8 Secure-Ready");
    strcpy(cwdName, "/"); 
    cmdStatus = 3; 
    millisEndConnection = millis() + 15000;
  }
}

boolean FtpServer::userIdentity() {
  if (strcmp(command, "USER") == 0) {
    if (_FTP_USER.equals(parameters)) { 
      clientPtr->println("331 Password required"); 
      return true; 
    }
    clientPtr->println("530 User not found");
  }
  return false;
}

boolean FtpServer::userPassword() {
  if (strcmp(command, "PASS") == 0) {
    if (_FTP_PASS.equals(parameters)) { 
      clientPtr->println("230 Logged in."); 
      return true; 
    }
    clientPtr->println("530 Login incorrect");
  }
  return false;
}

boolean FtpServer::processCommand() {
  char path[350]; 

  if (!strcmp(command, "AUTH") && parameters != NULL && !strcmp(parameters, "TLS")) {
    clientPtr->println("234 Proceed with negotiation");
    clientPtr->flush();
    
    WiFiClientSecure *sClient = new WiFiClientSecure();
    sClient->setCertificate(server_crt);
    sClient->setPrivateKey(server_key);

    WiFiClient *oldClient = clientPtr;
    clientPtr = sClient;
    delete oldClient;
    
    isSecure = true;
    Serial.println(">>> TLS Ativado na 1.0.8");
    return true;
  }

  if (!strcmp(command, "FEAT")) {
    clientPtr->println("211-Extensions supported:");
    clientPtr->println(" AUTH TLS");
    clientPtr->println(" UTF8");
    clientPtr->println("211 End");
    return true;
  }
  
  if (!strcmp(command, "PWD") || !strcmp(command, "XPWD")) {
    clientPtr->printf("257 \"%s\"\r\n", cwdName);
  }
  else if (!strcmp(command, "PASV")) {
    dataIp = WiFi.localIP();
    clientPtr->printf("227 Entering Passive Mode (%u,%u,%u,%u,%u,%u)\r\n", 
                  dataIp[0], dataIp[1], dataIp[2], dataIp[3], dataPort >> 8, dataPort & 255);
  }
  else if (!strcmp(command, "CWD")) {
    if (makePath(path)) {
      File dir = SD.open(path);
      if (dir && dir.isDirectory()) {
        strncpy(cwdName, path, sizeof(cwdName) - 1);
        // Garante a barra no final
        if (cwdName[strlen(cwdName) - 1] != '/') strcat(cwdName, "/");
        clientPtr->printf("250 CWD successful. %s\r\n", cwdName);
      } else {
        // Se não for diretório, avisa o Dolphin para ele parar de tentar
        clientPtr->println("550 Not a directory");
      }
      if (dir) dir.close();
    }
  }
  else if (!strcmp(command, "LIST") || !strcmp(command, "NLST")) {
    if (dataConnect()) {
      clientPtr->println("150 Accepted data connection");
      File root = SD.open(cwdName);
      if (root && root.isDirectory()) {
        File entry;
        while (entry = root.openNextFile()) {
          String name = String(entry.name());
          int lastSlash = name.lastIndexOf('/');
          if (lastSlash != -1) name = name.substring(lastSlash + 1);

          if (entry.isDirectory()) {
            // O 'd' no início é vital para o Dolphin não tentar entrar em arquivos
            data.printf("d--x--x--x 1 owner group 0 Jan 01 1970 %s\r\n", name.c_str());
          } else {
            data.printf("-rw-rw-rw- 1 owner group %u Jan 01 1970 %s\r\n", (unsigned)entry.size(), name.c_str());
          }
          data.flush();
          entry.close();
          yield();
        }
      }
      if (root) root.close();
      data.stop();
      clientPtr->println("226 List done");
    }
  }
  else if (!strcmp(command, "RETR")) {
    if (makePath(path)) {
      file = SD.open(path, "r");
      if (file && !file.isDirectory() && dataConnect()) {
        clientPtr->printf("150 %u bytes\r\n", (unsigned)file.size());
        transferStatus = 1;
      } else { clientPtr->println("550 File not found"); }
    }
  }
  else if (!strcmp(command, "STOR")) {
    if (makePath(path)) {
      file = SD.open(path, "w");
      if (file && dataConnect()) {
        clientPtr->println("150 OK"); 
        transferStatus = 2;
      } else { clientPtr->println("451 SD Error"); if(file) file.close(); }
    }
  }
  else if (!strcmp(command, "QUIT")) {
    clientPtr->println("221 Goodbye"); 
    clientPtr->stop(); 
    cmdStatus = 1; 
    return false;
  }
  else { clientPtr->println("200 OK"); }
  return true;
}

boolean FtpServer::doRetrieve() {
  if (file && file.available()) {
    static uint8_t buf[1024]; // Buffer fixo para evitar erro 104
    int nb = file.read(buf, sizeof(buf));
    if (nb > 0) {
        data.write(buf, nb);
        data.flush(); // Crucial para estabilidade TLS
    }
    yield(); 
    return true;
  }
  closeTransfer(); 
  return false;
}

boolean FtpServer::doStore() {
  if (data.connected() && data.available()) {
    static uint8_t buf[1024];
    int nb = data.read(buf, sizeof(buf));
    if (nb > 0) file.write(buf, nb);
    yield(); 
    return true;
  } else if (!data.connected()) {
    closeTransfer(); 
    return false;
  }
  return true;
}

void FtpServer::closeTransfer() {
  if (file) file.close();
  data.stop(); 
  delay(10);
  transferStatus = 0;
  if(clientPtr) clientPtr->println("226 Done");
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
  if (!clientPtr || !clientPtr->available()) return -1;
  char c = clientPtr->read();
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
  memset(fullName, 0, 350);
  if (parameters == NULL || strlen(parameters) == 0) return false;

  // Se o Dolphin pedir "/" ou "."
  if (!strcmp(parameters, "/") || !strcmp(parameters, ".")) {
    strcpy(fullName, "/");
    return true;
  }

  if (parameters[0] == '/') {
    strncpy(fullName, parameters, 349);
  } else {
    strncpy(fullName, cwdName, 349);
    if (fullName[strlen(fullName)-1] != '/') strcat(fullName, "/");
    strncat(fullName, parameters, 349 - strlen(fullName));
  }
  
  // Limpeza de barras duplas "//" que o Dolphin costuma enviar
  char *p;
  while ((p = strstr(fullName, "//")) != NULL) {
    memmove(p, p + 1, strlen(p));
  }
  return true;
}

void FtpServer::disconnectClient() { 
  if(clientPtr) { clientPtr->stop(); delete clientPtr; clientPtr = nullptr; }
  cmdStatus = 1; 
}

