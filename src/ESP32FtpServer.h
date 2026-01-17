#ifndef FTP_SERVERESP_H
#define FTP_SERVERESP_H

#include "SD.h"
#include <FS.h>
#include <WiFiClient.h>

#define FTP_CTRL_PORT      21  
#define FTP_DATA_PORT_PASV 50009 
#define FTP_TIME_OUT       5    
#define FTP_CMD_SIZE       350 // Buffer grande para evitar crash em comandos longos
#define FTP_CWD_SIZE       350 
#define FTP_BUF_SIZE       1024 

class FtpServer {
public:
  FtpServer();
  void begin(String uname, String pword);
  int  handleFTP();

private:
  void iniVariables();
  void clientConnected();
  void disconnectClient();
  boolean userIdentity();
  boolean userPassword();
  boolean processCommand();
  boolean dataConnect();
  boolean doRetrieve();
  boolean doStore();
  void closeTransfer();
  void abortTransfer();
  boolean makePath(char *fullName); 
  int8_t readChar();

  IPAddress dataIp;
  WiFiClient client;
  WiFiClient data;
  File file;

  uint16_t dataPort;
  char cmdLine[FTP_CMD_SIZE];
  char cwdName[FTP_CWD_SIZE];
  char command[6];
  char rnfrName[300]; // Buffer para o arquivo original do Rename
  bool rnfrCmd;       
  char *parameters;
  uint16_t iCL;
  int8_t cmdStatus, transferStatus;
  uint32_t millisTimeOut, millisEndConnection, millisBeginTrans, bytesTransfered;
  String _FTP_USER, _FTP_PASS;
};

#endif
