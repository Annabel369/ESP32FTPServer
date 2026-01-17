ESP32FTPServer (Secure Edition 2026)

Version 1.0.8 - Professional FTP Server for Espressif ESP32 with Explicit TLS/SSL support and SD Card storage.

This version is the result of months of optimization, specifically designed to handle the ESP32 Core 3.3.5+ architecture, providing high security and rock-solid stability for personal use and IoT projects.
📝 What's New in Version 1.0.8?

Compared to version 1.0.7, this release introduces professional-grade security and networking fixes:

    FTP over TLS (Explicit SSL): Full support for the AUTH TLS command. Secure your transfers using professional certificates.

    Core 3.x Compatibility: Fully rewritten to support the new NetworkClient architecture of ESP32 Core 3.3.5+.

    Automatic SD Certificate Management: * The server automatically creates a /cert folder on the SD card.

        It deploys a default 10-year public certificate (signed via YubiKey) if no keys are found.

        Hot-Swapping: Update your certificates by simply replacing the files on the SD card—no recompilation needed.

    Anti-Timeout Logic (Error 128 Fix): Implementation of setNoDelay(true) and optimized socket timeouts to sync perfectly with FileZilla’s GnuTLS engine.

    Dynamic Memory Management: Removed heavy static buffers. Uses "Dynamic Record Sizing" for TLS, leaving over 270KB of RAM free for your application.

    Subfolder Navigation Fix: Improved LIST command with specific yield() and flush() logic to allow "Going Back" through directories without dropping the SSL session.

🛠 Installation

    Open the Arduino IDE.

    Go to Sketch -> Include Library -> Manage Libraries...

    Search for ESP32FtpServer and install version 1.0.8.

    Note: Ensure you are using ESP32 Board Manager version 3.0.0 or higher.

💻 Quick Start (Secure Mode)
C++

#include <WiFi.h>
#include <SD.h>
#include <ESP32FtpServer.h>
#include "ESP32FtpServerCert.h" // Your YubiKey Signed Certificates

#define SD_CS 5
FtpServer ftp;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  // Optimization for Network Performance
  WiFi.setSleep(false);

  if (SD.begin(SD_CS)) {
    // The server will automatically create /cert/cert.crt and /cert/key.key on SD
    ftp.begin("admin", "secure123"); 
    Serial.println("FTP Server Ready with TLS Support");
  }
}

void loop() {
  ftp.handleFTP(); // Must be called frequently
}

📂 SD Card Folder Structure

Upon the first run, the library generates the following structure for security:

    /cert/cert.crt — Public Certificate (PEM format).

    /cert/key.key — Private Key (PEM format).

    Pro Tip: To use your own domain certificate, simply overwrite these files on the SD card.

⚙️ Recommended FileZilla Settings

To ensure 100% stability with the ESP32 hardware:

    Encryption: Use "Require explicit FTP over TLS".

    Timeout: Set to 60 seconds or 0 (Infinite).

    Transfer Settings: Limit the "Maximum number of simultaneous connections" to 1.

⚖️ License & Credits

Licensed under the LGPL-3.0 License.

Maintained by: Amauri Bueno dos Santos (2026). Based on original works by MollySophia and robo8080.
For the library to be recognized by the Arduino Registry, it follows this structure:
https://github.com/MollySophia/ESP32_FTPServer_SD (which again is based on https://github.com/robo8080/ESP32_FTPServer_SD) 

Modified to better support subdirectories
## Status: Versão 1.0.8 Estável (2026) - Assinada com YubiKey
