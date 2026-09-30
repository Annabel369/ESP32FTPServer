#ifndef USER_SETUP_CUSTOM_H
#define USER_SETUP_CUSTOM_H

// Avisa a TFT_eSPI para IGNORAR o User_Setup.h interno dela
#define USER_SETUP_LOADED 1
#define USER_SETUP_INFO "ESP32-2432S028R_Amarelinho"

// -----------------------------------------------------------------------------
// 1. Driver do Display
// -----------------------------------------------------------------------------
#define ILI9341_2_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// -----------------------------------------------------------------------------
// 2. Mapeamento de Pinos do ESP32-2432S028R (Amarelinho)
// -----------------------------------------------------------------------------
#define TFT_BL           21
#define TFT_BACKLIGHT_ON HIGH

#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1  // Conectado ao Reset do ESP32

#define TOUCH_CS 33  // Chip Select do Touch XPT2046

// -----------------------------------------------------------------------------
// 3. Fontes Ativas
// -----------------------------------------------------------------------------
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// -----------------------------------------------------------------------------
// 4. Frequências SPI
// -----------------------------------------------------------------------------
#define SPI_FREQUENCY       27000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000

#endif // USER_SETUP_CUSTOM_H