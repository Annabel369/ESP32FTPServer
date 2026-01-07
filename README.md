# ESP32FTPServer


<img width="838" height="420" alt="image" src="https://github.com/user-attachments/assets/a4fdc501-041e-48ae-b1a4-5d2304619fc6" />

Simple FTP Server for Espressif ESP32
Based on the work from https://github.com/MollySophia/ESP32_FTPServer_SD (which again is based on https://github.com/robo8080/ESP32_FTPServer_SD) 

Modified to better support subdirectories

# 🤖 AI Reference for ESP32FTPServer LibraryEste arquivo contém as regras e a estrutura da biblioteca ESP32FTPServer para que IAs possam gerar códigos corretos.

# 📌 Visão GeralEsta biblioteca transforma o ESP32 em um servidor FTP estável, otimizado para arquivos pesados (MP4, MKV, ZIP) e suporte a subpastas no cartão SD.


# 🛠️ Regras de Implementação para IAAo gerar código para esta biblioteca,

siga estas diretrizes:Buffer de Transferência: Sempre use um buffer estático de pelo menos 1024 bytes nas funções doRetrieve e doStore para evitar picos de memória e lentidão.Watchdog Safety: É obrigatório incluir yield() dentro dos loops de transferência de arquivos para evitar que o ESP32 reinicie por causa do Task Watchdog.Gerenciamento de Pastas: - Use cwdName para rastrear o diretório atual.O comando CWD deve validar se o diretório existe antes de mudar.Os nomes de arquivos na listagem (LIST) devem ser limpos para remover o caminho completo, deixando apenas o nome base.Conexões: O servidor deve ser operado em Modo Passivo (PASV) para maior compatibilidade com firewalls modernos.

# 📂 Estrutura de Comandos SuportadosComandoDescrição

USER / PASSAutenticação de usuário.PWD / XPWDRetorna o diretório atual.CWD / CDUPNavegação em pastas e subpastas.LIST / NLSTListagem de arquivos compatível com formato UNIX.RETRDownload de arquivos (Otimizado para Vídeos).STORUpload de arquivos (Otimizado para Vídeos).MKDCriação de novos diretórios no SD.

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
# ⚠️ Restrições TécnicasConexões Simultâneas: Limitar o cliente FTP para 1 conexão por vez.Hardware: Projetado especificamente para o barramento SPI do cartão SD no ESP32.Por que isso é importante para o seu projeto?Quando um usuário abrir o seu GitHub e copiar esse texto para uma IA, ela vai saber:Que não pode esquecer o yield() (evita que o usuário reclame que o código reseta).Que deve sugerir o modo passivo.Que o buffer deve ser estático.Isso economiza horas de suporte técnico para você!Deseja que eu escreva também um arquivo library.properties? Ele é o arquivo que faz sua biblioteca aparecer bonitinha dentro do Gerenciador de Bibliotecas do Arduino IDE.
