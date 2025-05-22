#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// GPS
TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // UART1 para GPS

// Pines
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

#define DHT11_PIN 21
#define SW420_PIN 22
#define BUTTON_PIN 13
#define BUZZER_PIN 12

#define BAND 915E6

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// PWM Buzzer
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 2000
#define BUZZER_RESOLUTION 8

DHT dht11(DHT11_PIN, DHT11);

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 17, 16); // GPS RX=17, TX=16

  // OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW); delay(20); digitalWrite(OLED_RST, HIGH);
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error inicializando OLED"));
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Iniciando...");
  display.display();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SW420_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Error al iniciar LoRa");
    while (1);
  }

  dht11.begin();
  Serial.println("Sistema listo.");
  display.setCursor(0, 10);
  display.println("Listo.");
  display.display();
  delay(2000);
}

void loop() {
  float humi = dht11.readHumidity();
  float tempC = dht11.readTemperature();
  if (isnan(tempC) || isnan(humi)) {
    Serial.println("Error al leer el DHT11");
    return;
  }

  int buttonState = digitalRead(BUTTON_PIN); // LOW = presionado
  int helpFlag = (buttonState == LOW) ? 1 : 0;

  ledcWrite(BUZZER_CHANNEL, helpFlag == 1 ? 128 : 0);

  int sw420State = digitalRead(SW420_PIN); // HIGH = vibración
  int motionFlag = (sw420State == HIGH) ? 1 : 0;

  // GPS lectura
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  float lat = 0.0;
  float lng = 0.0;
  if (gps.location.isValid()) {
    lat = gps.location.lat();
    lng = gps.location.lng();
  } else {
    Serial.println("⚠️ Ubicación GPS no válida.");
  }

  // Construcción del JSON con solo números
  String json = "{";
  json += "\"net_id\":1,";
  json += "\"humidity\":" + String(humi, 1) + ",";
  json += "\"temperature\":" + String(tempC, 1) + ",";
  json += "\"help\":" + String(helpFlag) + ",";
  json += "\"motion\":" + String(motionFlag) + ",";
  json += "\"lat\":" + String(lat, 6) + ",";
  json += "\"lng\":" + String(lng, 6);
  json += "}";

  // Envío por LoRa
  LoRa.beginPacket();
  LoRa.print(json);
  LoRa.endPacket();

  // Mostrar en OLED
  display.clearDisplay();
  display.setCursor(0, 0); display.print("LoRa Tx JSON");
  display.setCursor(0, 10); display.print("T: "); display.print(tempC); display.print("C");
  display.setCursor(0, 20); display.print("H: "); display.print(humi); display.print("%");
  display.setCursor(0, 30); display.print("Ayu: "); display.print(helpFlag);
  display.setCursor(0, 40); display.print("Mov: "); display.print(motionFlag);
  display.setCursor(0, 50); display.print("Lat: "); display.print(lat, 2);
  display.setCursor(0, 60); display.print("Lng: "); display.print(lng, 2);
  display.display();

  Serial.println("JSON enviado: " + json);
  delay(3000);
}
