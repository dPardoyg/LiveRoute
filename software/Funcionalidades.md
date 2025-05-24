## Funcionalidades del Proyecto IoT de Monitoreo Vehicular en Tiempo Real

Este sistema IoT está diseñado para monitorear en tiempo real el estado de una motocicleta utilizada en servicios de entrega a domicilio. El sistema recopila datos ambientales y de seguridad, los transmite de forma inalámbrica, y los visualiza en una plataforma centralizada para facilitar la supervisión y análisis histórico.

### Características Principales

#### 1. **Adquisición de Datos desde Sensores (Placa Heltec LoRa 32 V2 #1)**

* **Sensor DHT11:** Mide la temperatura y humedad ambiental.
* **Sensor SW-420:** Detecta vibraciones o movimientos bruscos en la motocicleta.
* **Módulo GPS NEO-6M:** Obtiene coordenadas geográficas (latitud y longitud).
* **Botón de ayuda:** Envía una alerta de emergencia cuando se presiona.
* **Buzzer:** Se activa para emitir una señal sonora en situaciones críticas.

#### 2. **Transmisión Inalámbrica vía ESP-NOW (Placa Heltec LoRa 32 V2 #1 → Placa Heltec LoRa 32 V2 #2)**

* Se empaquetan los datos en formato JSON.
* Se transmite el paquete usando protocolo ESP-NOW de forma eficiente y sin dependencia de redes Wi-Fi.

#### 3. **Recepción y Reenvío de Datos (Placa Heltec LoRa 32 V2 #2)**

* Recibe el paquete JSON enviado vía ESP-NOW.
* Lo retransmite a la placa LILYGO T-SIM7600G-H vía UART serial.

#### 4. **Conectividad GPRS y Publicación MQTT (Placa LILYGO T-SIM7600G-H)**

* Establece conexión GPRS forzando red 3G mediante comandos AT.
* Extrae y agrega coordenadas GPS propias si están disponibles.
* Publica los datos en un **broker MQTT (EMQX)** alojado en la Raspberry Pi.
* En caso de emergencia (botón de ayuda presionado o pérdida de red), envía **SMS automatizados** con coordenadas.

#### 5. **Servidor de Monitoreo y Almacenamiento (Raspberry Pi)**

* **EMQX Broker:** Recibe datos MQTT desde la LILYGO.
* **Node-RED:** Procesa y transforma los datos entrantes, filtra eventos críticos y gestiona lógica condicional.
* **InfluxDB:** Almacena datos históricos de sensores para análisis.
* **Grafana:** Visualiza gráficamente temperatura, humedad, movimiento y ubicación en tiempo real.

#### 6. **Automatización y Alertas en Telegram**

* Node-RED analiza el campo `message` del JSON.
* Si `message = 1`, se envía una alerta con coordenadas al bot de Telegram configurado.

### Interacción del Software con el Hardware

* **Sensores ↔ Heltec LoRa 1:** A través de pines digitales y comunicación UART.
* **Heltec LoRa 1 ↔ Heltec LoRa 2:** Mediante ESP-NOW.
* **Heltec LoRa 2 ↔ LILYGO SIM7600:** Comunicación serial.
* **LILYGO SIM7600 ↔ Raspberry Pi:** Comunicación MQTT vía red GPRS.
* **Raspberry Pi ↔ Usuario final:** A través de Node-RED, Grafana y Telegram.

Este conjunto de funcionalidades permite la supervisión integral del vehículo, asegurando que los operadores puedan responder rápidamente a situaciones críticas y analizar patrones históricos para mejorar la eficiencia del servicio de entrega.
