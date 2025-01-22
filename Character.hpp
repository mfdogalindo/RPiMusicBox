class Character {
  private:
    const int SPRITE_WIDTH = 16;
    const int SPRITE_HEIGHT = 24;
    const float ACCELERATION_THRESHOLD = 0.6;
    const int FLOOR_Y = 280;
    
    // Estados de la animación
    enum State {
      WALKING,
      FALLING,
      FALLEN,
      GETTING_UP
    };
    
    int x, y;
    int frame = 0;
    State currentState = WALKING;
    unsigned long lastFrameChange = 0;
    unsigned long stateStartTime = 0;
    
    // Tiempos para cada estado (en ms)
    const unsigned long FALLING_DURATION = 500;    // Duración de la caída
    const unsigned long FALLEN_DURATION = 1000;    // Tiempo que permanece caído
    const unsigned long GETTING_UP_DURATION = 500; // Duración de levantarse
    const int FRAME_DELAY = 200;                   // Tiempo entre frames de caminata
    
    const uint16_t walkingColor = ST77XX_WHITE;
    const uint16_t fallingColor = ST77XX_RED;
    
  public:
    Character(int startX) : x(startX), y(FLOOR_Y - SPRITE_HEIGHT) {}
    
    void update(Adafruit_ST7789& tft, float accelX, float accelY, float accelZ) {
      // Limpiar posición anterior
      tft.fillRect(x - 2, y, SPRITE_WIDTH + 4, SPRITE_HEIGHT + 4, ST77XX_BLACK);
      
      unsigned long currentTime = millis();
      
      // Detectar caída solo si está caminando
      if (currentState == WALKING && 
          (abs(accelX) > ACCELERATION_THRESHOLD || 
           abs(accelY) > ACCELERATION_THRESHOLD || 
           abs(accelZ) > 2.0)) {
        currentState = FALLING;
        stateStartTime = currentTime;
      }
      
      // Manejar estados y transiciones
      switch(currentState) {
        case WALKING:
          handleWalking(tft, currentTime);
          break;
          
        case FALLING:
          if (currentTime - stateStartTime >= FALLING_DURATION) {
            currentState = FALLEN;
            stateStartTime = currentTime;
          }
          drawFalling(tft, (currentTime - stateStartTime) / (float)FALLING_DURATION);
          break;
          
        case FALLEN:
          if (currentTime - stateStartTime >= FALLEN_DURATION) {
            currentState = GETTING_UP;
            stateStartTime = currentTime;
          }
          drawFallen(tft);
          break;
          
        case GETTING_UP:
          if (currentTime - stateStartTime >= GETTING_UP_DURATION) {
            currentState = WALKING;
          }
          drawGettingUp(tft, (currentTime - stateStartTime) / (float)GETTING_UP_DURATION);
          break;
      }
    }
    
  private:
    void handleWalking(Adafruit_ST7789& tft, unsigned long currentTime) {
      if (currentTime - lastFrameChange > FRAME_DELAY) {
        frame = (frame + 1) % 4;
        lastFrameChange = currentTime;
        
        x += 2;
        if (x > tft.width()) {
          x = 0;
        }
      }
      drawWalking(tft);
    }
    
    void drawWalking(Adafruit_ST7789& tft) {
      drawStickFigure(tft, walkingColor, frame);
    }
    
    void drawFalling(Adafruit_ST7789& tft, float progress) {
      // Animación de caída progresiva
      int fallY = y + (progress * 4); // Se mueve un poco hacia abajo
      float rotation = progress * 45; // Rotación hasta 45 grados
      
      // Dibujar figura rotada/inclinada
      int centerX = x + 8;
      int centerY = fallY + 12;
      
      tft.drawLine(centerX, centerY + 8, centerX, centerY - 2, fallingColor); // Cuerpo
      tft.drawCircle(centerX, centerY - 6, 4, fallingColor); // Cabeza
      
      // Brazos cayendo progresivamente
      int armDrop = progress * 6;
      tft.drawLine(centerX, centerY, centerX - 4, centerY + armDrop, fallingColor);
      tft.drawLine(centerX, centerY, centerX + 4, centerY + armDrop, fallingColor);
      
      // Piernas cayendo
      int legSpread = 6 - (progress * 3);
      tft.drawLine(centerX, centerY + 8, centerX - legSpread, centerY + 12, fallingColor);
      tft.drawLine(centerX, centerY + 8, centerX + legSpread, centerY + 12, fallingColor);
    }
    
    void drawFallen(Adafruit_ST7789& tft) {
      int centerX = x + 8;
      int centerY = y + 20;
      
      // Figura completamente caída
      tft.drawLine(centerX - 6, centerY, centerX + 6, centerY, fallingColor); // Cuerpo horizontal
      tft.drawCircle(centerX - 8, centerY - 2, 4, fallingColor); // Cabeza
      tft.drawLine(centerX - 2, centerY, centerX - 2, centerY - 4, fallingColor); // Brazo
      tft.drawLine(centerX + 2, centerY, centerX + 2, centerY - 4, fallingColor); // Brazo
      tft.drawLine(centerX - 4, centerY + 2, centerX - 4, centerY + 4, fallingColor); // Pierna
      tft.drawLine(centerX + 4, centerY + 2, centerX + 4, centerY + 4, fallingColor); // Pierna
      
      // Estrellas o espirales para efecto cómico
      if ((millis() / 200) % 2) {  // Parpadeo cada 200ms
        tft.drawPixel(centerX - 12, centerY - 8, ST77XX_YELLOW);
        tft.drawPixel(centerX + 12, centerY - 6, ST77XX_YELLOW);
        tft.drawPixel(centerX, centerY - 10, ST77XX_YELLOW);
      }
    }
    
    void drawGettingUp(Adafruit_ST7789& tft, float progress) {
      int centerX = x + 8;
      int centerY = y + 20 - (progress * 8);  // Se va levantando
      
      // Interpolación entre caído y de pie
      tft.drawLine(centerX, centerY + 8, centerX, centerY - 2, ST77XX_ORANGE); // Cuerpo
      tft.drawCircle(centerX, centerY - 6, 4, ST77XX_ORANGE); // Cabeza
      
      // Brazos y piernas moviéndose a posición
      float angle = (1.0 - progress) * PI / 3;  // De horizontal a vertical
      int armY = sin(angle) * 6;
      int armX = cos(angle) * 6;
      tft.drawLine(centerX, centerY, centerX - armX, centerY + armY, ST77XX_ORANGE);
      tft.drawLine(centerX, centerY, centerX + armX, centerY + armY, ST77XX_ORANGE);
      
      tft.drawLine(centerX, centerY + 8, centerX - 4, centerY + 12, ST77XX_ORANGE);
      tft.drawLine(centerX, centerY + 8, centerX + 4, centerY + 12, ST77XX_ORANGE);
    }
    
    void drawStickFigure(Adafruit_ST7789& tft, uint16_t color, int pose) {
      // [El código existente de drawStickFigure se mantiene igual]
      int centerX = x + 8;
      int centerY = y + 12;
      
      // Cuerpo base
      tft.drawLine(centerX, centerY + 8, centerX, centerY - 4, color); // Cuerpo
      tft.drawCircle(centerX, centerY - 8, 4, color); // Cabeza
      
      // Piernas y brazos según la pose de caminata
      switch(pose) {
        case 0: // Neutral
          tft.drawLine(centerX, centerY + 8, centerX - 4, centerY + 12, color);
          tft.drawLine(centerX, centerY + 8, centerX + 4, centerY + 12, color);
          tft.drawLine(centerX, centerY, centerX - 4, centerY + 4, color);
          tft.drawLine(centerX, centerY, centerX + 4, centerY + 4, color);
          break;
        case 1: // Paso derecho
          tft.drawLine(centerX, centerY + 8, centerX - 4, centerY + 10, color);
          tft.drawLine(centerX, centerY + 8, centerX + 4, centerY + 6, color);
          tft.drawLine(centerX, centerY, centerX + 4, centerY + 4, color);
          tft.drawLine(centerX, centerY, centerX - 4, centerY + 4, color);
          break;
        case 2: // Paso completo
          tft.drawLine(centerX, centerY + 8, centerX - 4, centerY + 12, color);
          tft.drawLine(centerX, centerY + 8, centerX + 4, centerY + 12, color);
          tft.drawLine(centerX, centerY, centerX + 4, centerY - 4, color);
          tft.drawLine(centerX, centerY, centerX - 4, centerY - 4, color);
          break;
        case 3: // Paso izquierdo
          tft.drawLine(centerX, centerY + 8, centerX - 4, centerY + 6, color);
          tft.drawLine(centerX, centerY + 8, centerX + 4, centerY + 10, color);
          tft.drawLine(centerX, centerY, centerX + 4, centerY + 4, color);
          tft.drawLine(centerX, centerY, centerX - 4, centerY + 4, color);
          break;
      }
    }
};