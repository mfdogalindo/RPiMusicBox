#include "Arduino.h"
#include <FastLED.h>

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

    startHue += 1;
    FastLED.show();
    delay(50);
  }

  void diagonal() {
    FastLED.clear();
    colors[0] = CHSV(startHue + (256 / 7), 255, 80);
    for (int i = 7; i > 0; i--) {
      colors[i] = colors[i - 1];
    }

    for (int i = 0; i < numLeds; i++) {
      leds[i] = colors[diagonalMap[i]];
    }

    startHue += 1;
    FastLED.show();
    delay(50);
  }

  // Efecto de serpiente que recorre la matriz
  void snakeEffect(uint8_t wait = 100) {
    CRGB snakeColor = CRGB(0, 255, 0);  // Verde

    // Generar la ruta de la serpiente dinámicamente
    for (int i = 0; i < numLeds; i++) {
      FastLED.clear();

      // Dibujar la cabeza y cola de la serpiente
      for (int j = 0; j < 3; j++) {
        if (i - j >= 0) {
          int x = (i - j) % matrixWidth;
          int y = ((i - j) / matrixWidth) % matrixHeight;
          if (y % 2 == 1) {
            x = matrixWidth - 1 - x;  // Zigzag
          }
          leds[xy2pos(x, y)] = snakeColor;
          leds[xy2pos(x, y)].fadeToBlackBy(j * 85);
        }
      }
      FastLED.show();
      delay(wait);
    }
  }

  void rotatingEffect(uint8_t wait = 200) {
    CRGB rotateColor = CRGB(0, 0, 255);  // Azul

    for (int rotation = 0; rotation < 4; rotation++) {
      FastLED.clear();

      for (int i = 0; i < matrixWidth; i++) {
        switch (rotation) {
          case 0:  // Horizontal
            leds[xy2pos(i, matrixHeight / 2)] = rotateColor;
            break;
          case 1:  // Diagonal
            if (i < matrixHeight) {
              leds[xy2pos(i, i)] = rotateColor;
            }
            break;
          case 2:  // Vertical
            leds[xy2pos(matrixWidth / 2, i)] = rotateColor;
            break;
          case 3:  // Diagonal inversa
            if (i < matrixHeight) {
              leds[xy2pos(i, matrixHeight - 1 - i)] = rotateColor;
            }
            break;
        }
      }
      FastLED.show();
      delay(wait);
    }
  }

  void waveEffect(uint8_t wait = 150) {
    CRGB waveColor = CRGB(255, 0, 0);  // Rojo

    // Ola horizontal
    for (int x = 0; x < matrixWidth * 2; x++) {
      FastLED.clear();
      for (int y = 0; y < matrixHeight; y++) {
        int pos = x - y;
        if (pos >= 0 && pos < matrixWidth) {
          leds[xy2pos(pos, y)] = waveColor;
        }
      }
      FastLED.show();
      delay(wait);
    }

    // Ola vertical
    for (int y = 0; y < matrixHeight * 2; y++) {
      FastLED.clear();
      for (int x = 0; x < matrixWidth; x++) {
        int pos = y - x;
        if (pos >= 0 && pos < matrixHeight) {
          leds[xy2pos(x, pos)] = waveColor;
        }
      }
      FastLED.show();
      delay(wait);
    }
  }
};