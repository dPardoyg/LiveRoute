#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// Pines LoRa
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

// Pantalla OLED
#define ANCHOPANTALLA 128
#define ALTOPANTALLA 64
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
Adafruit_SSD1306 display(ANCHOPANTALLA, ALTOPANTALLA, &Wire, OLED_RST);

// Dirección MAC de la placa 3 (ESP-NOW)
uint8_t broadcastAddress[] = {0xc8, 0x2e, 0x18, 0x90, 0xb8, 0x40};

// Nueva estructura con solo valores numéricos
typedef struct struct_message {
  float humidity;
  float temperature;
  int help;
  int motion;
  float lat;
  float lng;
} struct_message;

struct_message outgoingData;
esp_now_peer_info_t peerInfo;

String incomingData;
unsigned long lastAlive = 0;

void setup() {
  Serial.begin(115200);

  // Inicializar pantalla OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Fallo iniciando SSD1306"));
    delay(2000);
    ESP.restart();
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("RECEPTOR LORA");
  display.display();

  // Inicializar LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(915E6)) {
    Serial.println("Error iniciando LoRa");
    delay(2000);
    ESP.restart();
  }

  display.setCursor(0, 10);
  display.print("LoRa iniciado!");
  display.display();
  delay(1500);

  // Inicializar ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    delay(2000);
    ESP.restart();
  }

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar peer");
    delay(2000);
    ESP.restart();
  }
}

void loop() {
  if (millis() - lastAlive > 3000) {
    Serial.println("Esperando datos LoRa...");
    lastAlive = millis();
  }

  int packetSize = LoRa.parsePacket();
  if (packetSize > 0 && packetSize < 256) {
    incomingData = "";
    while (LoRa.available()) {
      incomingData += (char)LoRa.read();
    }

    Serial.print("JSON recibido: ");
    Serial.println(incomingData);

    // Parsear JSON
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, incomingData);
    if (error) {
      Serial.print("Error al parsear JSON: ");
      Serial.println(error.c_str());
      return;
    }

    outgoingData.humidity = doc["humidity"] | 0.0;
    outgoingData.temperature = doc["temperature"] | 0.0;
    outgoingData.help = doc["help"] | 0;
    outgoingData.motion = doc["motion"] | 0;
    outgoingData.lat = doc["lat"] | 0.0;
    outgoingData.lng = doc["lng"] | 0.0;

    // Mostrar en pantalla OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("T: "); display.print(outgoingData.temperature); display.print("C");
    display.setCursor(0, 10);
    display.print("H: "); display.print(outgoingData.humidity); display.print("%");
    display.setCursor(0, 20);
    display.print("Ayuda: "); display.print(outgoingData.help);
    display.setCursor(0, 30);
    display.print("Mov: "); display.print(outgoingData.motion);
    display.setCursor(0, 40);
    display.print("Lat: "); display.print(outgoingData.lat, 2);
    display.setCursor(0, 50);
    display.print("Lng: "); display.print(outgoingData.lng, 2);
    display.display();

    // Enviar por ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));
    if (result == ESP_OK) {
      Serial.println("✅ Datos enviados por ESP-NOW:");
      Serial.printf("Temp: %.1f, Hum: %.1f, Ayuda: %d, Mov: %d, Lat: %.6f, Lng: %.6f\n",
                    outgoingData.temperature,
                    outgoingData.humidity,
                    outgoingData.help,
                    outgoingData.motion,
                    outgoingData.lat,
                    outgoingData.lng);
    } else {
      Serial.println("❌ Error al enviar por ESP-NOW");
    }
  }
}
