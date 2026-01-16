/**
 * ESP32 FTP Server - 2026 Optimized Version
 * Author: Annabel369
 * * Este exemplo demonstra como configurar um servidor FTP estável no ESP32
 * com suporte a cartões SD e navegação avançada de pastas.
 */

#include <WiFi.h>
#include <SD.h>
#include <ESP32FtpServer.h> // Usando < > para bibliotecas instaladas

// Configurações do Cartão SD (Ajuste o CS conforme sua placa)
#define SD_CS 5

// Credenciais do WiFi
const char* ssid     = "SEU_WIFI_AQUI";
const char* password = "SUA_SENHA_AQUI";

// Credenciais do FTP
const char* ftp_user = "admin";
const char* ftp_pass = "1234";

FtpServer ftp;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n--- ESP32 FTP Server 2026 Optimized ---");

  // 1. Inicializa o Cartão SD
  if (!SD.begin(SD_CS)) {
    Serial.println("ERRO: Falha ao montar o cartão SD!");
    // Não prossegue se o SD falhar, pois o FTP precisa de armazenamento
    while (true) { delay(1000); }
  }
  Serial.println("SD Card montado com sucesso.");

  // 2. Conexão WiFi
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // 3. Melhoria de Performance (Essencial para vídeos MP4/ZIP)
  // Desativa o gerenciamento de energia do WiFi para evitar latência no FTP
  WiFi.setSleep(false);

  Serial.println("\nWiFi Conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Use este IP no FileZilla ou Windows Explorer.");

  // 4. Inicia o Servidor FTP
  ftp.begin(ftp_user, ftp_pass);
  Serial.println("Servidor FTP ativo!");
}

void loop() {
  // O handleFTP deve ser chamado continuamente no loop principal.
  // Ele gerencia a comunicação com o cliente e as transferências de arquivos.
  ftp.handleFTP();
}
