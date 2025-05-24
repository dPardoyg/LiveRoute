# Tecnologías Utilizadas

A continuación se presenta una lista detallada de las tecnologías empleadas en el desarrollo del proyecto de monitoreo IoT para motocicleta.

## Lenguajes de Programación

* **C/C++**: Utilizado para la programación de las placas Heltec LoRa 32 y la LILYGO SIM7600 mediante el entorno de desarrollo Arduino.
* **JavaScript**: Usado en Node-RED para el procesamiento de datos, envío de alertas y simulaciones.
* **Python**: Para scripts de administración y procesamiento adicional en la Raspberry Pi (opcional).

## Frameworks y Entornos de Desarrollo

* **Arduino IDE / PlatformIO**: Para el desarrollo y la carga de firmware en las placas ESP32 (Heltec y LilyGO).
* **Node-RED**: Plataforma de desarrollo de lógica visual utilizada para la recepción, procesamiento y envío de mensajes MQTT, y para interactuar con Telegram.
* **Grafana**: Herramienta de visualización de datos en tiempo real a partir de la base de datos InfluxDB.
* **Zabbix**: Opcional, para la monitorización del estado del sistema.

## Bibliotecas (Arduino)

* **LoRa.h**: Para la comunicación LoRa entre las placas Heltec.
* **DHT.h**: Para la lectura de sensores DHT11 de temperatura y humedad.
* **SoftwareSerial / HardwareSerial**: Para la comunicación UART con GPS y módulo SIM7600.
* **TinyGPS++**: Para el procesamiento de datos NMEA provenientes del módulo GPS Neo-6M.
* **ArduinoJson**: Para la construcción y análisis de mensajes en formato JSON.
* **PubSubClient**: Cliente MQTT para el envío de mensajes al broker desde la LILYGO SIM7600.

## Protocolos

* **ESP-NOW**: Para la comunicación inalámbrica entre sensores y nodos intermedios.
* **LoRa**: Comunicación entre nodos intermedios y nodo central (LILYGO SIM7600).
* **MQTT**: Protocolo de mensajería ligera utilizado para enviar datos al broker EMQX alojado en la Raspberry Pi.
* **AT Commands**: Comandos utilizados para el control del módulo SIM7600, incluyendo conexión a red, envío de datos y obtención de ubicación GPS.

## Bases de Datos

* **InfluxDB**: Base de datos de series temporales usada para almacenar los datos históricos de sensores y alertas.

## Otras Herramientas

* **EMQX Broker**: Servidor MQTT que recibe los datos desde la LILYGO SIM7600.
* **Telegram Bot**: Usado para enviar alertas y notificaciones en tiempo real al usuario cuando se detecta movimiento o se presiona el botón de ayuda.
* **VNC / SSH**: Para la administración remota de la Raspberry Pi.
