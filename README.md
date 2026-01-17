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

📝 Release Notes v1.0.7 - Stable Edition (2026)

Melhorias e Correções:

    Proteção Anti-Crash: Implementada verificação de caracteres ilegais (:, *, ?). O servidor agora recusa arquivos inválidos em vez de reiniciar o ESP32.

    Buffer de Caminho (Path): Aumentado de 128 para 300 bytes, permitindo lidar com nomes de arquivos extremamente longos (ex: vídeos do YouTube ou Screenshots) sem corromper a memória.

    Estabilidade de Rede: Adicionado yield() no loop de listagem de arquivos e um pequeno delay no fechamento do soquete de dados para evitar desconexões prematuras no FileZilla.

    Rename Robusto: Correção na lógica RNFR/RNTO para garantir que a flag de renomeação seja resetada corretamente após cada tentativa.

    Velocidade: Mantido o buffer de 1024 bytes para alta performance em arquivos MP4 e ZIP.

Como testar a estabilidade agora:

    Tente enviar um arquivo com nome curto (ex: teste.txt) -> Deve funcionar 100%.

    Tente enviar o arquivo da Ferrari (nome longo) -> Deve funcionar 100% (desde que não tenha caracteres proibidos).

    Tente enviar um arquivo com : -> O servidor vai dar o erro 553 mas não vai cair, permitindo que você continue trabalhando.

This library is licensed under the LGPL-3.0 License.
<img width="838" height="420" alt="image" src="https://github.com/user-attachments/assets/a4fdc501-041e-48ae-b1a4-5d2304619fc6" />
Simple FTP Server for Espressif ESP32
Based on the work from https://github.com/MollySophia/ESP32_FTPServer_SD (which again is based on https://github.com/robo8080/ESP32_FTPServer_SD) 

Modified to better support subdirectories
