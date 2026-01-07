# 🤖 AI Reference for ESP32FTPServer Library

Este arquivo contém as regras e a estrutura da biblioteca ESP32FTPServer para que IAs possam gerar códigos corretos.

# 📌 Visão Geral

Esta biblioteca transforma o ESP32 em um servidor FTP estável, otimizado para arquivos pesados (MP4, MKV, ZIP) e suporte a subpastas no cartão SD.

# 🛠️ Regras de Implementação para IA

1 - Ao gerar código para esta biblioteca, siga estas diretrizes:

2  - Buffer de Transferência: Sempre use um buffer estático de pelo menos 1024 bytes nas funções doRetrieve e doStore para evitar picos de memória e lentidão.Watchdog Safety: É obrigatório incluir yield() dentro dos loops de transferência de arquivos para evitar que o ESP32 reinicie por causa do Task 

3  - Watchdog.Gerenciamento de Pastas: - Use cwdName para rastrear o diretório atual.

O comando CWD deve validar se o diretório existe antes de mudar.Os nomes de arquivos na listagem (LIST) devem ser limpos para remover o caminho completo, deixando apenas o nome base.Conexões: O servidor deve ser operado em Modo Passivo (PASV) para maior compatibilidade com firewalls modernos.

#  📂 Estrutura de Comandos Suportados

Comando               Descrição

USER / PASS           Autenticação de usuário.

PWD / XPWD            Retorna o diretório atual.

CWD / CDUP            Navegação em pastas e subpastas.

LIST / NLST           Listagem de arquivos compatível com formato 

UNIX.RETR             Download de arquivos (Otimizado para Vídeos).

STOR                  Upload de arquivos (Otimizado para Vídeos).

MKD                   Criação de novos diretórios no SD.

# 🚀 Exemplo de Uso (Sketch Principal)C++


    #include <WiFi.h>
    #include <SD.h>
    #include "ESP32FtpServer.h"

    FtpServer ftp;
    
    void setup() {
      Serial.begin(115200);
      WiFi.begin("SSID", "SENHA");
      
      if(SD.begin()) {
        // Começa o servidor com User e Pass
        ftp.begin("admin", "1234"); 
      }
    }

    void loop() {
      // Deve ser chamado constantemente, sem delays no loop
      ftp.handleFTP(); 
    }
# ⚠️ Restrições TécnicasConexões Simultâneas: Limitar o cliente FTP para 1 conexão por vez.Hardware: Projetado especificamente para o barramento SPI do cartão SD no ESP32.# ESP32FTPServer
Simple FTP Server for Espressif ESP32


Based on the work from https://github.com/MollySophia/ESP32_FTPServer_SD (which again is based on https://github.com/robo8080/ESP32_FTPServer_SD) 



Modified to better support subdirectories
