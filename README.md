ESP32FTPServer (Secure Edition 2026)

<img width="1533" height="670" alt="image" src="https://github.com/user-attachments/assets/75b34b53-9936-4109-aabe-1fac28db4d34" />


Version 1.1.4 - Professional FTP Server for Espressif ESP32 with Explicit TLS/SSL support and SD Card storage.

This version is the result of months of optimization, specifically designed to handle the ESP32 Core 3.3.5+ architecture, providing high security and rock-solid stability for personal use and IoT projects.
📝 What's New in Version 1.1.4?

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

    Search for ESP32FtpServer and install version 1.1.4.

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

<img width="1013" height="399" alt="image" src="https://github.com/user-attachments/assets/2efdb55c-3907-404e-893c-d16786665200" />

🎨 Custom TFT_eSPI Setup

    //#include <User_Setup.h>           // Default setup is root library folder
    #include "../ESP32FtpServer/src/User_Setup_Custom.h"

## 🛠️ Open Source Hardware & Custom PCB Design

This library is fully tailormade for the **ESP32-2432S028R** ("Amarelinho" / CYD) hardware architecture, utilizing its integrated MicroSD card slot (SPI CS GPIO 5), TFT display, and touchscreen out of the box.

### 📐 PCB Fabrication & Customization

For makers and developers looking to build, customize, or produce their own board variations:

- **Ready-to-Manufacture Gerber Files:** You can easily order custom PCBs through services like [PCBWay](https://www.pcbway.com/).
- **Aesthetic Customization:** Choose your preferred Solder Mask color (purple, black, white, red, etc.) during fabrication.
- **Hardware Upgrades:** 
  - Upgrade to ESP32 modules featuring extended **PSRAM / SPI Flash** for larger network buffers and enhanced TLS throughput.
  - Custom pinout adaptations for **E-Paper / E-Ink** displays, perfect for ultra-low-power status dashboards and FTP server monitoring.


## 📜 Open Source Governance, Standardization, and Reflections

One of the ongoing discussions in the embedded development and Arduino ecosystem revolves around library naming conventions, namespace management, and open-source governance.

### ⚠️️ The Challenge of Naming Collisions (`SD.h`, `WiFi.h`, etc.)

When the Arduino team originally introduced the Library Manager, generic names like `SD`, `WiFi`, and `Ethernet` were established for legacy AVR architectures (such as the Arduino Uno). As hardware evolved and Espressif released the **ESP32**, core libraries maintained these identical header names (`SD.h`, `WiFi.h`) to preserve backward compatibility with existing codebases and examples.

However, the absence of explicit namespaces or architecture-specific prefixes (such as `ESP32_SD` or `Arduino_SD`) creates two fundamental issues within the open-source community:

1. **Compatibility Ambiguity:** Compiler conflicts occur when multiple libraries share identical file names, leading to resolution ambiguities when building across different platforms.
2. **Authorship and Precedence:** Overlapping generic names can obscure original authorship and the historical precedence of independent developers who first authored and published solutions under those names.

### 💡 Scopes and Package Management Standards
Modern package managers (such as Node.js npm, Rust Cargo, or Python PyPI) address this problem using scoped packages/namespaces (e.g., @annabel369/sd vs. @espressif/sd).

By explicitly declaring dependencies within library.properties and library.json, this library ensures transparent dependency resolution while fully respecting the underlying ESP32 core architecture.

### 🔮 Legacy and Project Continuity
This project was built with dedication, extensive testing, AI assistance, and research to deliver a stable FTP Server solution for ESP32 devices (such as the ESP32-2432S028R).

In the true spirit of Open Source and the Linux community:

1. **Forking & Evolution: Anyone in the community is welcome to fork this repository, improve the codebase, fix bugs, or add new features.

2. **Attribution: If you create a derivative work (e.g., ESP32FtpServer2 or an extended version), please preserve the original credits and license (LGPL-3.0).

3. **Maintenance: If you are interested in becoming a co-maintainer or contributing via Pull Requests, feel free to open an issue or reach out.

### ⚖️ License & Credits
Licensed under the LGPL-3.0 License.

Maintained by: Amauri Bueno dos Santos (2026).

Based on original works by MollySophia and robo8080.

Status: Versão 1.1.4 Estável (2026) - Assinada com YubiKey
