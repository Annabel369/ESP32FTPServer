# ESP32FTPServer
ESP32 FTP Server (Optimized 2026)

Simple and robust FTP Server for Espressif ESP32 using SD Card storage. This version is specifically optimized for handling large files and stable navigation.
🚀 Key Features (Version 1.0.6)

    Subfolder Stability: Fixed CWD and CDUP logic for reliable navigation through nested directories in FileZilla and Windows Explorer.

    Large File Optimization: Implemented 1024-byte static buffering, making it stable for transferring MP4, ZIP, and binary files.

    File Management: Full support for RNFR/RNTO (Rename) and DELE (Delete) commands.

    High Performance: Includes recommendations to disable WiFi Power Save mode for maximum transfer speeds.

🛠 Installation

    Open the Arduino IDE.

    Go to Sketch -> Include Library -> Manage Libraries...

    Search for ESP32FtpServer and install version 1.0.6.

💻 Quick Start
C++

#include <WiFi.h>
#include <SD.h>
#include <ESP32FtpServer.h>

#define SD_CS 5
FtpServer ftp;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  // Optimization for speed
  WiFi.setSleep(false);

  if (SD.begin(SD_CS)) {
    ftp.begin("admin", "1234"); // User, Password
  }
}

void loop() {
  ftp.handleFTP(); // Must be called in the loop
}

📂 Project Structure

For the library to be recognized by the Arduino Registry, it follows this structure:

    examples/ESP32FTPServerExample/ESP32FTPServerExample.ino

    src/ESP32FtpServer.h & src/ESP32FtpServer.cpp

    library.properties

    LICENSE (LGPL-3.0)

⚖️ License

This library is licensed under the LGPL-3.0 License.
<img width="838" height="420" alt="image" src="https://github.com/user-attachments/assets/a4fdc501-041e-48ae-b1a4-5d2304619fc6" />
Simple FTP Server for Espressif ESP32
Based on the work from https://github.com/MollySophia/ESP32_FTPServer_SD (which again is based on https://github.com/robo8080/ESP32_FTPServer_SD) 

Modified to better support subdirectories
