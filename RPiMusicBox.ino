#include "Config.h"

float p = 3.1415926;

MatrixLed ledsMatrix(4,4);
Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);   // I added reference to SPI1, not needed to use SPI0
RPI_PICO_Timer ITimer2(2);
DFRobotDFPlayerMini dfPlayer;
BMI160 IMU(Wire1);
Character* character;

calData calib = { 0 };  //Calibration data
AccelData accelData;    //Sensor data
GyroData gyroData;
MagData magData;

bool TimerHandler2(struct repeating_timer *t)
{  
  ledsMatrix.diagonal();
  return true;
}

DisplayRenderer renderer(tft, LCD_WIDTH, LCD_HEIGHT);
AnimatedSprite sprite(gladis_data, 100, 4);

void setup() {
#if defined(NEOPIXEL_POWER)
  // If this board has a power control pin, we must set it to output and high
  // in order to enable the NeoPixels. We put this in an #if defined so it can
  // be reused for other boards without compilation errors
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
#endif

  pixels.setPixelColor(0, 0xffffff);
  pixels.show();


  ITimer2.attachInterruptInterval(TIMER2_INTERVAL_MS * 20, TimerHandler2);

  SPI.setRX(PIN_MISO);  // (RP2040 ZERO pin 0, 4)
  SPI.setSCK(PIN_SCK);  // (RP2040 ZERO pin 2, 6, 18)
  SPI.setTX(PIN_MOSI);  // (RP2040 ZERO pin 3, 7, 19)
  SPI.begin();

  tft.init(240, 320);
  tft.fillScreen(ST77XX_BLACK);

    // Inicializar Serial para debuggear
  Serial.begin(115200);
  
  // Inicializar UART0 (Serial1) para comunicación con DFPlayer
  Serial1.setTX(MP3_TX);
  Serial1.setRX(MP3_RX);
  Serial1.begin(9600);
  
  Serial.println("Inicializando DFPlayer...");
  
  // Inicializar DFPlayer usando Serial1 (UART0)
  if (!dfPlayer.begin(Serial1)) {
    Serial.println("Error al iniciar DFPlayer:");
    Serial.println("1. Revisa las conexiones");
    Serial.println("2. Inserta una tarjeta SD");
    // while(true);
  }

  // Mostrar información de la tarjeta SD
  // mostrarInfoSD();

  Wire1.setSDA(I2C_SDA);
  Wire1.setSCL(I2C_SCL);
  Wire1.begin();

  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
  }

  tft.fillScreen(ST77XX_BLACK);
  drawLabels();


  //uint8_t spriteId = renderer.createSprite(spriteData, 8, 8, palette, 4);
  sprite.play();

  character = new Character(0);
}

// Variables globales para mantener la posición del texto
const int TEXT_HEIGHT = 16;  // Altura de texto con textSize(2)
const int VALUE_X = 50;      // Posición X donde empiezan los valores
int currentY = 0;           // Para seguimiento de posición Y


void loop() {
  
  IMU.update();
  currentY = TEXT_HEIGHT;  // Reiniciar posición Y después de las etiquetas
  
  // Actualizar valores del acelerómetro
  IMU.getAccel(&accelData);
  updateValue(accelData.accelX, VALUE_X, currentY);
  currentY += TEXT_HEIGHT;
  updateValue(accelData.accelY, VALUE_X, currentY);
  currentY += TEXT_HEIGHT;
  updateValue(accelData.accelZ, VALUE_X, currentY);
  currentY += TEXT_HEIGHT * 2;  // Espacio extra entre secciones
  
  // Saltar el título del giroscopio
  currentY += TEXT_HEIGHT;
  
  // Actualizar valores del giroscopio
  IMU.getGyro(&gyroData);
  updateValue(gyroData.gyroX, VALUE_X, currentY);
  currentY += TEXT_HEIGHT;
  updateValue(gyroData.gyroY, VALUE_X, currentY);
  currentY += TEXT_HEIGHT;
  updateValue(gyroData.gyroZ, VALUE_X, currentY);
  
  //character->update(tft, accelData.accelX, accelData.accelY, accelData.accelZ);

sprite.update();
if (sprite.isPlaying()) {
    renderer.drawSprite(
        sprite.getCurrentFrame(),
        sprite.getWidth(),
        sprite.getHeight(),
        60, 180,
        sprite.getScale()
    );
}
  delay(100);  // Podemos reducir el delay ya que no hay parpadeo
}



void drawLabels() {
  // Dibujar las etiquetas que no cambiarán
  tft.setTextSize(2);
  currentY = 0;
  
  // Acelerómetro
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(0, currentY);
  tft.println("Accelerometer:");
  currentY += TEXT_HEIGHT;
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, currentY);
  tft.print("X:");
  currentY += TEXT_HEIGHT;
  tft.setCursor(0, currentY);
  tft.print("Y:");
  currentY += TEXT_HEIGHT;
  tft.setCursor(0, currentY);
  tft.print("Z:");
  currentY += TEXT_HEIGHT * 2;  // Espacio extra entre secciones
  
  // Giroscopio
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(0, currentY);
  tft.println("Gyroscope:");
  currentY += TEXT_HEIGHT;
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, currentY);
  tft.print("X:");
  currentY += TEXT_HEIGHT;
  tft.setCursor(0, currentY);
  tft.print("Y:");
  currentY += TEXT_HEIGHT;
  tft.setCursor(0, currentY);
  tft.print("Z:");
}

void updateValue(float value, int x, int y) {
  // Limpiar solo el área del valor
  tft.fillRect(x, y, 120, TEXT_HEIGHT, ST77XX_BLACK);
  tft.setCursor(x, y);
  tft.print(value);
}


void mostrarInfoSD() {
  Serial.println("\n=== Información de la tarjeta SD ===");
  
  // Obtener número total de archivos en la SD
  uint16_t totalFiles = dfPlayer.readFileCounts();
  Serial.print("Total de archivos MP3: ");
  Serial.println(totalFiles);
  
  // Obtener pista actual
  uint16_t currentTrack = dfPlayer.readCurrentFileNumber();
  Serial.print("Pista actual: ");
  Serial.println(currentTrack);
  
  Serial.println("\nListado de pistas disponibles:");
  for (uint16_t i = 1; i <= totalFiles; i++) {
    Serial.print("Pista ");
    Serial.print(i);
    // Si es la pista actual, marcarla
    if (i == currentTrack) {
      Serial.println(" [Reproduciendo]");
    } else {
      Serial.println();
    }
  }
  
  // Mostrar estado del volumen
  Serial.print("\nVolumen actual: ");
  Serial.println(dfPlayer.readVolume());
}
