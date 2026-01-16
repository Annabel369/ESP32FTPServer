#include <WiFi.h>
#include <SD.h>
#include "ESP32FtpServer.h"

// Defina seus pinos do SD se não forem os padrão
#define SD_CS 5

FtpServer ftp;

void setup() {
  Serial.begin(115200);

  // 1. Configuração do WiFi
  WiFi.begin("SEU_WIFI", "SUA_SENHA");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Conectado!");
  Serial.print("IP para o Windows: ");
  Serial.println(WiFi.localIP());

  // 2. Desativa economia de energia do WiFi (Melhora velocidade de vídeos)
  WiFi.setSleep(false);

  // 3. Inicializa o Cartão SD
  if (!SD.begin(SD_CS)) {
    Serial.println("Erro ao montar SD!");
    while(1);
  }

  // 4. Inicia o FTP (User, Password)
  ftp.begin("admin", "1234");
}

void loop() {
  // handleFTP retorna 1 se houver cliente ativo
  ftp.handleFTP();
}
