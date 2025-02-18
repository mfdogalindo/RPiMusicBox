// WaveShare RP2040 Nano

#define MP3_TX      0
#define MP3_RX      1
#define PIN_SCK     2
#define PIN_MOSI    3
#define PIN_MISO    4
#define TFT_CS      5 
#define TFT_DC      6  
#define TFT_RST     7  
#define LCD_WIDTH   240
#define LCD_HEIGHT  320

#define I2C_SDA     26
#define I2C_SCL     27

#define TIMER2_INTERVAL_MS    1000

#define IMU_ADDRESS 0x68    //Change to the address of the IMU


#include "Leds.h"
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h> 
#include <Adafruit_ST7789.h>
#include "hardware/clocks.h" // Librería para controlar los relojes del RP2040
#include "RPi_Pico_TimerInterrupt.h"
#include <DFRobotDFPlayerMini.h>
#include "FastIMU.h"
#include <Wire.h>
#include "Character.hpp"

#include "SpriteData.h"

#include "DisplayRenderer.h"

#include "gladis_sprite.h"

//#define LCD_WIDTH 240
//#define LCD_HEIGHT 320
