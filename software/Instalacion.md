\## 🛠 Instrucciones de Instalación

A continuación se describen los pasos necesarios para instalar y configurar el sistema completo de monitoreo IoT en todos sus entornos: **Heltec LoRa 32**, **Raspberry Pi (IoT Stack)** y la **placa LilyGO SIM7600G-H** con conectividad GSM/GPRS.

---

### 1. Configuración del Nodo Transmisor (Heltec LoRa 1)

* **Placa:** Heltec LoRa 32 V2.
* **Sensores conectados:** DHT11 (temperatura/humedad), SW-420 (vibración), botón de ayuda y módulo GPS (NEO-6M).
* **Acciones:**

  * Configurar los pines GPIO en el sketch de Arduino.
  * Leer valores de cada sensor y GPS.
  * Construir un objeto JSON con los datos obtenidos.
  * Enviar este paquete JSON mediante comunicación LoRa a la segunda placa receptora.

  <img src="/documentacion/funcionalidades/Lora_Transmisora_Dht11.jpg" alt="Configuración de Sensores" width="300" />
*Descripción:* Configurando del sensor de temperatura y humedad en la placa LoRa 32 v2.

---

### 2. Configuración del Nodo Receptor (Heltec LoRa 2)

* **Placa:** Heltec LoRa 32 V2.
* **Acciones:**

  * Escuchar mensajes entrantes vía LoRa.
  * Recibir el JSON enviado desde la primera placa.
  * Deserializar el contenido JSON.
  * Reenviar los datos recibidos mediante ESP-NOW a la placa principal encargada de conectividad celular (LilyGO).

---

### 3. Renvío de Datos por ESP-NOW

* **Tecnología:** ESP-NOW (protocolo inalámbrico rápido punto a punto).
* **Acciones:**

  * Configurar MAC address del destinatario (LilyGO).
  * Serializar nuevamente los datos en formato JSON.
  * Enviar al nodo central encargado de la comunicación GSM.

---

### 4. Recepción de Datos ESP-NOW (LilyGO SIM7600G-H)

* **Placa:** LilyGO T-SIM7600G-H.
* **Acciones:**

  * Configurar recepción de datos ESP-NOW.
  * Parsear el JSON recibido.
  * Iniciar conexión a red móvil (3G/GPRS/EDGE) utilizando comandos AT y puerto Serial del SIM7600.

  <img src="/documentacion/funcionalidades/Estado_de_LILYGO_SIM7600.jpg" alt="Estado de la Placa" width="300" />
*Descripción:* Estado de leds al tener conexion del APN Y MQTT.

---

### 5. Conexión a Red GSM/GPRS/EDGE

* **Acciones:**

  * Ejecutar comandos AT para establecer el modo de red (preferentemente forzar 3G).
  * Inicializar conexión GPRS.
  * Verificar conexión al APN del proveedor móvil.

  <img src="/documentacion/funcionalidades/Conexion APN_MQTT.png" alt="CoNEXION apn_MQTT" width="300" />
*Descripción:* Monitor Serial de la PLaca LILYGO SIM-7600G-H .

---

### 5.1 Conexión al Broker MQTT (EMQX)

* **Broker:** EMQX instalado en la Raspberry Pi.
* **Acciones:**

  * Configurar cliente MQTT (topic, QoS, keepAlive).
  * Publicar mensajes JSON recibidos desde ESP-NOW al broker MQTT.
  * Incluir autenticación si está habilitada en el broker.

  <img src="/documentacion/funcionalidades/Subcriptor_broker.jpg" alt="Conexion al Broker" width="300" />
*Descripción:* Subcritor del topico esp32/sensores (pruebas).

---

### 6. Configuración del IoT Stack (Raspberry Pi)

* **Plataforma base:** Raspberry Pi (Raspbian OS).
* **Servicios utilizados:**

  * **Node-RED:** procesamiento de datos y reglas lógicas.
  * **InfluxDB:** almacenamiento de series temporales.
  * **Grafana:** visualización de datos históricos.

---

### 6.1 Configuración del Flujo en Node-RED

* **Acciones:**

  * Suscribirse al topic MQTT en Node-RED.
  * Deserializar JSON.
  * Filtrar eventos críticos (como botón de ayuda o movimiento).
  * Reenviar datos a InfluxDB y activar alerta por Telegram si es necesario.

---

### 6.2 Almacenamiento en InfluxDB

* **Acciones:**

  * Crear una base de datos (por ejemplo: `iot_moto`).
  * Utilizar el nodo "influxdb out" en Node-RED.
  * Mapear campos de medición (temperature, humidity, motion, gps, etc.).

---

### 6.3 Conexión de InfluxDB a Grafana

* **Acciones:**

  * Configurar InfluxDB como fuente de datos en Grafana.
  * Crear dashboards personalizados con gráficas de temperatura, humedad, ubicación, movimiento, etc.
  * Ajustar filtros y rangos de tiempo.

---

### 7. Creación y Configuración del Bot de Telegram

* **Acciones:**

  * Crear un bot en Telegram mediante \[@BotFather].
  * Obtener el token de autenticación.
  * Integrar el bot en Node-RED usando el nodo `telegram sender`.
  * Configurar el flujo para que, cuando `msg.payload.message == 1`, se envíe una alerta con ubicación y estado actual al usuario.

---

Este conjunto de pasos asegura que todo el sistema funcione de manera integrada: desde la adquisición de datos en la motocicleta hasta la visualización y notificación en la nube.
