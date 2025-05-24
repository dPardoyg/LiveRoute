## Estructura del Código

El proyecto se organiza modularmente para facilitar su mantenimiento y expansión. A continuación se describe la estructura de los archivos y funciones utilizadas en cada componente del sistema:

---

### 1. Heltec LoRa 32 (Transmisor - LoRa 1)

**Ubicación:** `/Software/Heltec_LoRa1`

#### Archivos principales:

* `main.cpp`

  * Inicializa los sensores (DHT11, SW-420, botón).
  * Captura lecturas de sensores y GPS.
  * Construye un objeto JSON con la estructura:

    ```json
    {
      "id": "Vehiculo_1",
      "temperature": 28,
      "humidity": 60,
      "motion": 1,
      "latitude": 20.4213,
      "longitude": -99.8834,
      "message": 0,
      "timestamp": 1688499323000
    }
    ```
  * Envía el objeto por LoRa a LoRa 2.

* `gps.cpp` / `gps.h`

  * Utiliza `TinyGPS++` para obtener coordenadas desde el módulo Neo-6M por UART1.
  * Convierte lat/lon a tipo `float` con precisión aceptable.

* `lora_send.cpp` / `lora_send.h`

  * Configura el canal LoRa y envía el JSON como `char[]`.
  * Controla tiempos de espera entre transmisiones.

* `config.h`

  * Define pines para sensores, UART GPS, OLED, y ajustes LoRa.

---

### 2. Heltec LoRa 32 (Receptor - LoRa 2)

**Ubicación:** `/Software/Heltec_LoRa2`

#### Archivos principales:

* `main.cpp`

  * Recibe los datos JSON desde LoRa.
  * Muestra resumen en pantalla OLED.
  * Envía por UART al módulo LILYGO SIM7600.

* `lora_receive.cpp` / `lora_receive.h`

  * Configura recepción por LoRa.
  * Maneja errores de paquetes mal formateados.

* `oled_display.cpp` / `oled_display.h`

  * Muestra temperatura, humedad y GPS brevemente.
  * Útil para diagnóstico de campo.

---

### 3. LILYGO SIM7600 (Publicador por GPRS/MQTT)

**Ubicación:** `/Software/Lilygo_SIM7600`

#### Archivos principales:

* `main.cpp`

  * Recibe datos por UART.
  * Verifica integridad del mensaje.
  * Publica por MQTT al broker local en Raspberry Pi (EMQX).

* `mqtt_publish.cpp` / `mqtt_publish.h`

  * Utiliza `TinyGSM` y `PubSubClient`.
  * Reintenta en caso de pérdida de conexión MQTT o GPRS.

* `at_commands.cpp` / `at_commands.h`

  * Configura red 3G (`AT+CNMP=2` y `AT+CMNB=2`).
  * Inicializa GPRS y APN.
  * Verifica estado del módem y red.

* `config.h`

  * APN, PIN de SIM, tópico MQTT, parámetros de conexión.

---

### 4. Raspberry Pi

**Ubicación:** `/Software/RaspberryPi`

#### Subcarpetas:

* `/NodeRED`

  * `flows.json`: Flujos con nodos que:

    * Reciben mensajes MQTT.
    * Almacenan datos en InfluxDB.
    * Envían alertas por Telegram si `message == 1`.
    * Visualizan datos en panel básico.

* `/EMQX_Broker`

  * `config.yaml`: Configuración del broker MQTT (tópicos, usuarios).
  * Script `emqx_start.sh`: Arranca el broker automáticamente al boot.

* `/InfluxDB`

  * `init_influxdb.sh`: Script para crear base de datos `vehiculo_db`.
  * `create_user.sh`: Usuario y contraseña para consultas de Grafana.

* `/TelegramBot`

  * `telegram.js`: Código en Node.js para enviar mensaje con:

    * Latitud, longitud y enlace directo a Google Maps.
  * Se conecta con Node-RED mediante `exec node`.

---

### 5. PC (Visualización y monitoreo)

#### Software usado:

* Grafana (corriendo en Raspberry Pi, accedido desde la PC)

  * Dashboards:

    * Temperatura y humedad en tiempo real.
    * Mapa con última ubicación.
    * Registro de eventos de movimiento o botón de pánico.

* Zabbix (opcional)

  * Puede integrarse para monitoreo de disponibilidad de placas y enlaces.
