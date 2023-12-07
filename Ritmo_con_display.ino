#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAX_DATA_POINTS 128  // Número máximo de puntos de datos en el gráfico
int dataPoints[MAX_DATA_POINTS] = {0};  // Inicializa el arreglo con ceros
int dataPointer = 0;  // Puntero para seguir la posición de los datos

SoftwareSerial bluetooth(5, 6);

int contadorPicos = 0;
unsigned long tiempoAnterior = 0;
const int umbralPico = 400;

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  pinMode(10, INPUT);
  pinMode(11, INPUT);

  // Inicializa la pantalla OLED con la dirección I2C 0x3C
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(41,16);             // Start at top-left corner
  display.println("HealthPatch");
  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.display();
  delay(3000);
}

void loop() {
  if ((digitalRead(11) == 1) || (digitalRead(10) == 1)) {
    // En lugar de imprimir "%" en el puerto serie, muestra un texto en la pantalla OLED
    //Serial.println("%");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 16);
    display.println("Pulso no detectado");
    display.setTextSize(2);
    display.display();
  } else {
    // Lee la señal del ECG y muéstrala en la pantalla OLED
    //Serial.println(analogRead(A0));
    int ecgValue = analogRead(A0);

    // Almacena el valor en el arreglo de datos
    dataPoints[dataPointer] = ecgValue;
    dataPointer = (dataPointer + 1) % MAX_DATA_POINTS;

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(60, 28);
    display.println(ecgValue);

    for (int i = 1; i < SCREEN_WIDTH; i++) {
    int dataIdx1 = (dataPointer + i - 1) % MAX_DATA_POINTS;
      int dataIdx2 = (dataPointer + i) % MAX_DATA_POINTS;
      int yPos1 = map(dataPoints[dataIdx1], 0, 450, 0, SCREEN_HEIGHT);
      int yPos2 = map(dataPoints[dataIdx2], 0, 450, 0, SCREEN_HEIGHT);
      display.drawLine(i - 1, SCREEN_HEIGHT - yPos1, i, SCREEN_HEIGHT - yPos2, SSD1306_WHITE);
    }

    /*display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(64, 16);
    display.println(ecgValue);*/
    display.display();

    int ecgBPM = analogRead(A0);

    if (ecgBPM > umbralPico){
      unsigned long tiempoActual = millis();
      if (tiempoActual - tiempoAnterior > 60000){
        int bpm = contadorPicos;
        Serial.println("BPM: " + String(bpm));
        contadorPicos = 0;
        tiempoAnterior = tiempoActual;
      }
      contadorPicos++;
    }

    //String mensaje = String(ecgValue);
    //bluetooth.println("BPM: " + mensaje);
    //bluetooth.write('\n');
    //Serial.println("BPM: " + mensaje);
  }

  delay(25);
}