/*
 * FTP SERVER FOR ESP32 (Optimized Version 1.0.7)
 * Based on the work of: Jean-Michel Gallego, David Paiva, @robo8080.
 * Modified and Enhanced by: Amauri Bueno (Annabel369) - 2026
 */

#ifndef FTP_SERVERESP_H
#define FTP_SERVERESP_H

#include "SD.h"
#include <FS.h>
#include <WiFiClient.h>

#define FTP_CTRL_PORT      21  
#define FTP_DATA_PORT_PASV 50009 
#define FTP_TIME_OUT       5    
#define FTP_CMD_SIZE       263
#define FTP_CWD_SIZE       263
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
  char command[5];
  char rnfrName[128]; // Para Rename
  bool rnfrCmd;       // Flag Rename
  char *parameters;
  uint16_t iCL;
  int8_t cmdStatus, transferStatus;
  uint32_t millisTimeOut, millisEndConnection, millisBeginTrans, bytesTransfered;
  String _FTP_USER;
  String _FTP_PASS;
};

#endif
