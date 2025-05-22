#define TINY_GSM_MODEM_SIM7600
//#define TINY_GSM_DEBUG Serial  // Descomenta si necesitas depuración detallada

#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define SerialMon Serial
#define SerialAT Serial1

#define TINY_GSM_RX_BUFFER 1024

// Pines módem
#define MODEM_PWRKEY 4
#define MODEM_POWER_ON 25
#define LED_PIN 12

// GPRS y MQTT
const char apn[] = "internet.itelcel.com";
const char gprsUser[] = "webgprs";
const char gprsPass[] = "webgprs2002";
const char* broker = "broker.emqx.io";
const char* topicSensorData = "liveroute/esp32";

// Número de teléfono para SMS (opcional)
const char* phoneNumber = "+527715460525";

// Estructura de datos recibida por ESP-NOW
struct SensorData {
  float humidity;
  float temperature;
  int help;
  int motion;
  float lat;
  float lng;
};

SensorData receivedData;
SensorData datosPendientes;
bool nuevaLectura = false;

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

unsigned long lastReconnectAttempt = 0;
const long reconnectInterval = 30000;

bool smsEnviadoDesconexion = false;
bool smsEnviadoBoton = false;

void enviarSMS(const String& mensaje) {
  if (modem.sendSMS(phoneNumber, mensaje)) {
    SerialMon.println("📲 SMS enviado: " + mensaje);
  } else {
    SerialMon.println("❌ Error al enviar SMS");
  }
}

bool testModem() {
  SerialMon.println("📡 Probando comunicación con módem...");
  for (int i = 0; i < 10; i++) {
    if (modem.testAT()) {
      SerialMon.println("✅ Módem respondió correctamente.");
      return true;
    }
    delay(500);
  }
  SerialMon.println("❌ No hay respuesta del módem.");
  return false;
}

bool conectarGPRS() {
  if (!testModem()) {
    SerialMon.println("🔁 Reiniciando módem...");
    modem.restart();
    delay(2000);
  }

  modem.sendAT("+CNMP=14");  // Solo WCDMA (3G)
  modem.sendAT("+CMNB=2");   // Sin LTE
  delay(2000);

  SerialMon.println("📶 Esperando red móvil...");
  if (!modem.waitForNetwork(180000)) {
    SerialMon.println("❌ Error al buscar red móvil");
    return false;
  }

  SerialMon.println("🌐 Conectando al APN...");
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println("❌ Error al conectar al APN");
    return false;
  }

  SerialMon.println("✅ Conectado a GPRS con IP: " + modem.getLocalIP());
  return true;
}

bool mqttConnect() {
  SerialMon.print("🔌 Conectando a MQTT... ");
  mqtt.setServer(broker, 1883);
  if (mqtt.connect("SIM7600_Client")) {
    SerialMon.println("¡Conectado!");
    return true;
  } else {
    SerialMon.printf("❌ Falló (estado: %d)\n", mqtt.state());
    return false;
  }
}

void sendMQTTMessage() {
  String jsonPayload = "{\"humedad\":" + String(receivedData.humidity, 2) +
                       ",\"temperatura\":" + String(receivedData.temperature, 2) +
                       ",\"ayuda\":" + String(receivedData.help) +
                       ",\"movimiento\":" + String(receivedData.motion) +
                       ",\"latitud\":" + String(receivedData.lat, 6) +
                       ",\"longitud\":" + String(receivedData.lng, 6) + "}";

  if (mqtt.connected()) {
    mqtt.publish(topicSensorData, jsonPayload.c_str());
    SerialMon.println("📤 Publicado: " + jsonPayload);
  } else {
    SerialMon.println("❗ MQTT desconectado");
  }
}

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(SensorData)) {
    memcpy(&datosPendientes, incomingData, sizeof(SensorData));
    nuevaLectura = true;
  }
}

void setup() {
  SerialMon.begin(115200);
  delay(10);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  pinMode(MODEM_PWRKEY, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  WiFi.mode(WIFI_OFF);

  SerialMon.println("⚙️ Encendiendo módem...");
  digitalWrite(MODEM_POWER_ON, HIGH);
  delay(100);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1000);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(3000);

  SerialAT.begin(115200, SERIAL_8N1, 26, 27);
  delay(3000);

  if (!conectarGPRS()) {
    SerialMon.println("❌ No se pudo conectar GPRS, reiniciando...");
    enviarSMS("⚠️ Error al conectar GPRS.");
    ESP.restart();
  }

  if (!mqttConnect()) {
    SerialMon.println("❌ No se pudo conectar MQTT, reiniciando...");
    enviarSMS("⚠️ Error al conectar MQTT.");
    ESP.restart();
  }

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    SerialMon.println("❌ Fallo al iniciar ESP-NOW");
    ESP.restart();
  }
  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  if (nuevaLectura) {
    nuevaLectura = false;
    memcpy(&receivedData, &datosPendientes, sizeof(SensorData));

    SerialMon.println("📩 Datos recibidos por ESP-NOW:");
    SerialMon.printf("🌡️ Temp: %.2f°C, 💧 Hum: %.2f%%, Ayuda: %d, Movimiento: %d, Lat: %.6f, Lng: %.6f\n",
                     receivedData.temperature, receivedData.humidity,
                     receivedData.help, receivedData.motion,
                     receivedData.lat, receivedData.lng);

    if (receivedData.help == 1 && !smsEnviadoBoton) {
      enviarSMS("🚨 Botón de ayuda activado. Lat " + String(receivedData.lat, 6) + ", Lon " + String(receivedData.lng, 6));
      smsEnviadoBoton = true;
    }

    sendMQTTMessage();
  }

  if (!modem.isNetworkConnected() || !modem.isGprsConnected()) {
    SerialMon.println("❗ Conexión GPRS perdida, intentando reconectar...");
    if (!smsEnviadoDesconexion) {
      enviarSMS("⚠️ Conexión GPRS perdida. Última ubicación: Lat " + String(receivedData.lat, 6) + ", Lon " + String(receivedData.lng, 6));
      smsEnviadoDesconexion = true;
    }
    if (!conectarGPRS()) {
      SerialMon.println("🔁 Reintento fallido, reiniciando...");
      ESP.restart();
    }
  } else {
    smsEnviadoDesconexion = false;
  }

  if (!mqtt.connected() && millis() - lastReconnectAttempt > reconnectInterval) {
    lastReconnectAttempt = millis();
    if (!mqttConnect() && !smsEnviadoDesconexion) {
      enviarSMS("⚠️ Conexión MQTT perdida. Última ubicación: Lat " + String(receivedData.lat, 6) + ", Lon " + String(receivedData.lng, 6));
      smsEnviadoDesconexion = true;
    }
  }

  mqtt.loop();
}
