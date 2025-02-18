#ifndef DISPLAY_RENDERER_H
#define DISPLAY_RENDERER_H

#include <Adafruit_ST7789.h>

class DisplayRenderer {
private:
    Adafruit_ST7789& display;
    const uint16_t width;
    const uint16_t height;
    uint16_t* frameBuffer;
    
public:
    DisplayRenderer(Adafruit_ST7789& tft, uint16_t w, uint16_t h) 
        : display(tft), width(w), height(h) {
        frameBuffer = new uint16_t[w * h];
        // Inicializar buffer con negro
        memset(frameBuffer, 0, w * h * sizeof(uint16_t));
    }
    
    ~DisplayRenderer() {
        delete[] frameBuffer;
    }

    void drawSprite(const uint16_t* spriteData, uint16_t spriteWidth, uint16_t spriteHeight, 
                   uint16_t x, uint16_t y, uint8_t scale = 1) {
        // Verificar límites
        if (x >= width || y >= height) return;
        
        // Calcular dimensiones finales
        uint16_t scaledWidth = spriteWidth * scale;
        uint16_t scaledHeight = spriteHeight * scale;
        
        // Ajustar si se sale de los límites
        if (x + scaledWidth > width) scaledWidth = width - x;
        if (y + scaledHeight > height) scaledHeight = height - y;
        
        // Buffer temporal para acumular cambios
        bool* pixelChanged = new bool[scaledWidth * scaledHeight]();
        uint16_t* newPixels = new uint16_t[scaledWidth * scaledHeight];
        
        // Generar imagen escalada y detectar cambios
        for (uint16_t sy = 0; sy < scaledHeight; sy++) {
            for (uint16_t sx = 0; sx < scaledWidth; sx++) {
                // Calcular posición original en el sprite
                uint16_t origX = sx / scale;
                uint16_t origY = sy / scale;
                uint16_t origColor = spriteData[origY * spriteWidth + origX];
                
                // Calcular posición en pantalla
                uint16_t screenX = x + sx;
                uint16_t screenY = y + sy;
                uint16_t bufferPos = sy * scaledWidth + sx;
                uint16_t screenPos = screenY * width + screenX;
                
                // Guardar nuevo color
                newPixels[bufferPos] = origColor;
                
                // Detectar si el pixel cambió
                if (frameBuffer[screenPos] != origColor) {
                    pixelChanged[bufferPos] = true;
                }
            }
        }
        
        // Actualizar pantalla por bloques de píxeles cambiados
        int startX = -1;
        for (uint16_t sy = 0; sy < scaledHeight; sy++) {
            for (uint16_t sx = 0; sx <= scaledWidth; sx++) {
                uint16_t bufferPos = sy * scaledWidth + sx;
                bool isChanged = (sx < scaledWidth) ? pixelChanged[bufferPos] : false;
                
                if (isChanged && startX == -1) {
                    // Inicio de un nuevo bloque
                    startX = sx;
                } else if (!isChanged && startX != -1) {
                    // Fin de un bloque, actualizar pantalla
                    uint16_t blockWidth = sx - startX;
                    display.startWrite();
                    display.setAddrWindow(x + startX, y + sy, blockWidth, 1);
                    for (int i = 0; i < blockWidth; i++) {
                        uint16_t color = newPixels[sy * scaledWidth + startX + i];
                        display.pushColor(color);
                        // Actualizar frameBuffer
                        frameBuffer[(y + sy) * width + (x + startX + i)] = color;
                    }
                    display.endWrite();
                    startX = -1;
                }
            }
        }
        
        delete[] pixelChanged;
        delete[] newPixels;
    }
    
    void clearArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color = 0) {
        // Verificar límites
        if (x >= width || y >= height) return;
        if (x + w > width) w = width - x;
        if (y + h > height) h = height - y;
        
        display.startWrite();
        display.setAddrWindow(x, y, w, h);
        
        for (uint16_t py = 0; py < h; py++) {
            for (uint16_t px = 0; px < w; px++) {
                uint16_t screenPos = (y + py) * width + (x + px);
                if (frameBuffer[screenPos] != color) {
                    display.pushColor(color);
                    frameBuffer[screenPos] = color;
                }
            }
        }
        
        display.endWrite();
    }
};

#endif // DISPLAY_RENDERER_H