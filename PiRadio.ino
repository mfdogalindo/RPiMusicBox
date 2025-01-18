#define LED_PIN 29
#include "Leds.h"

MatrixLed ledsMatrix(4,4);

void setup() {
}

void loop() {
  //ledsMatrix.snakeEffect();
  //ledsMatrix.rotatingEffect();
  //ledsMatrix.waveEffect();
  ledsMatrix.vertical();
}
