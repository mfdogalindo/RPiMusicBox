#include "Arduino.h"
#include <FastLED.h>
#include "Config.h"


class MatrixLed {
private:
  const int numLeds;
  const int matrixWidth;
  const int matrixHeight;
  uint8_t startHue = 0;
  CRGB* leds;
  CRGB colors[7];

  const int diagonalMap[16] = {
    0, 1, 2, 3,
    4, 3, 2, 1,
    2, 3, 4, 5,
    6, 5, 4, 3
  };

  const int verticalMap[16] = {
    0, 0, 0, 0,  // Primera fila: colores 0,1,2,3
    1, 1, 1, 1,  // Segunda fila: colores 1,2,3,4
    2, 2, 2, 2,  // Tercera fila: colores 2,3,4,5
    3, 3, 3, 3   // Cuarta fila: colores 3,4,5,6
  };

  const int radialMap[16] = {
    0, 0, 0, 0,  // Primera fila: colores 0,1,2,3
    0, 1, 1, 0,  // Segunda fila: colores 1,2,3,4
    0, 1, 1, 0,  // Tercera fila: colores 2,3,4,5
    0, 0, 0, 0   // Cuarta fila: colores 3,4,5,6
  };

  // Convertir coordenadas x,y a posición en el array
  int xy2pos(int x, int y) const {
    return y * matrixWidth + x;
  }

public:
  // Constructor
  MatrixLed(int width, int height)
    : matrixWidth(width),
      matrixHeight(height),
      numLeds(width * height) {
    leds = new CRGB[numLeds];
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, numLeds).setCorrection(TypicalLEDStrip);
    FastLED.clear();

    colors[0] = CRGB(0, 0, 0);
    colors[1] = CRGB(0, 0, 0);
    colors[2] = CRGB(0, 0, 0);
    colors[3] = CRGB(0, 0, 0);
    colors[4] = CRGB(0, 0, 0);
    colors[5] = CRGB(0, 0, 0);
    colors[6] = CRGB(0, 0, 0);
  }

  // Destructor
  ~MatrixLed() {
    delete[] leds;
  }

  void vertical() {
    FastLED.clear();
    colors[0] = CHSV(startHue + (256 / 7), 255, 80);
    for (int i = 4; i > 0; i--) {
      colors[i] = colors[i - 1];
    }

    for (int i = 0; i < numLeds; i++) {
      leds[i] = colors[verticalMap[i]];
    }

    startHue += 5;
    FastLED.show();
  }

  void diagonal() {
    FastLED.clear();
    colors[0] = CHSV(startHue % 255, 255, 80);
    for (int i = 7; i > 0; i--) {
      colors[i] = colors[i - 1];
    }

    for (int i = 0; i < numLeds; i++) {
      leds[i] = colors[diagonalMap[i]];
    }

    startHue += 1;
    FastLED.show();
  }

};