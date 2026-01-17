#ifndef FTP_SERVERESP_H
#define FTP_SERVERESP_H

#include <SD.h>
#include <FS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define FTP_CTRL_PORT      21  
#define FTP_DATA_PORT_PASV 50009 
#define FTP_TIME_OUT       5    
#define FTP_CMD_SIZE       350 
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
  boolean makePath(char *fullName); 
  int8_t readChar();
  void checkCertFolder(); // Nova função de gestão do SD

  IPAddress dataIp;
  WiFiClient *clientPtr = nullptr; 
  WiFiClient data;
  File file;

  uint16_t dataPort;
  char cmdLine[FTP_CMD_SIZE];
  char cwdName[FTP_CWD_SIZE];
  char command[6];
  char rnfrName[300];
  bool rnfrCmd;       
  char *parameters;
  uint16_t iCL;
  int8_t cmdStatus, transferStatus;
  uint32_t millisTimeOut, millisEndConnection, bytesTransfered;
  String _FTP_USER, _FTP_PASS;
  bool isSecure = false; 
};
#endif
