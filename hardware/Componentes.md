# Componentes Utilizados

Este apartado se muestran los principales componentes de hardware utilizados en el desarrollo del sistema IoT para el monitoreo de vehiculos de reparto, especificando su función dentro del sistema.

---

##  Placas de Desarrollo

### 1. **Heltec WiFi LoRa 32 V2** (x2)

* **Función:** Recolección de datos desde sensores y transmisión mediante LoRa.
* **Características clave:**

  * Módulo Wi-Fi + LoRa SX1276
  * Pantalla OLED integrada
  * Alimentación por micro-USB o pines VIN (5V)

### 2. **LILYGO T-SIM7600G-H** (x1)

* **Función:** Nodo principal de comunicación; recibe datos por ESP-NOW y publica a través de red celular (GSM/GPRS/3G) mediante MQTT.
* **Características clave:**

  * Modem 4G LTE SIM7600
  * Soporte GPS integrado
  * Comunicación GSM/GPRS
  * Ranura para tarjeta SIM y microSD

---

##  Sensores y Actuadores

### 3. **Sensor de Temperatura y Humedad DHT11** (x1)

* **Función:** Captura de temperatura y humedad ambiental.
* **Conectado a:** Placa Heltec 1 (Lado Sensor)

### 4. **Sensor de Vibración SW-420** (x1)

* **Función:** Detección de movimiento o vibración sospechosa.
* **Conectado a:** Placa Heltec 1

### 5. **Botón de Pánico** (x1)

* **Función:** Activación manual de alerta por parte del usuario.
* **Conectado a:** Placa Heltec 1

### 6. **GPS Neo-6M Ublox** (x1)

* **Función:** Obtención de coordenadas geográficas.
* **Conectado a:** Placa Heltec 1 mediante UART1

### 7. **Zumbador (Buzzer)** (x1)

* **Función:** Indicación sonora de eventos como activación de alerta.
* **Conectado a:** Placa Heltec 1

---

##  Alimentación y Conversión de Voltaje

### 8. **Módulo Regulador de Voltaje DC-DC XL4015 Step-Down 5A 75W con Voltímetro** (x1)

* **Función:** Conversión del voltaje de la batería de la motocicleta (12V) a 5V estables para alimentar todos los dispositivos.
* **Conexión en paralelo:** Todas las placas y sensores están alimentados desde la misma línea de 5V generada por este módulo.

---

##  Otros Componentes

* **Cables Dupont / Jumpers** – Para conexiones internas.
* **Protoboard y conectores rápidos** – Para pruebas de conexión.
* **Caja protectora / encapsulado** – Para asegurar los componentes en la motocicleta.

---

> **Nota:** Todos los componentes han sido seleccionados por su compatibilidad con microcontroladores ESP32, bajo consumo energético y confiabilidad en entornos móviles.
