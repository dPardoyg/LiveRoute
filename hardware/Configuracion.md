 ## Configuración

La configuración física de los componentes del sistema IoT está diseñada para una **instalación eficiente en un vehículo de reparto (motocicleta)**, donde todas las placas y sensores se alimentan desde una fuente común regulada a 5V. A continuación, se detallan las instrucciones de conexión y alimentación:

###  Fuente de alimentación

* Se utiliza un módulo **DC-DC XL4015 Step-Down 5A 75W con voltímetro**.
* El regulador se conecta directamente a la batería de la motocicleta (12V DC).
* La salida del regulador está configurada a **5V constantes**, permitiendo una corriente estable hasta 5A.
* Desde esta salida de 5V, se realiza una **conexión en paralelo** a los siguientes componentes:

  * Placa Heltec LoRa 32 V2 (transmisora)
  * Placa Heltec LoRa 32 V2 (receptora)
  * Placa LILYGO SIM7600G-H (controlador principal con GPRS)
  * Sensores: DHT11, SW-420, Botón de pánico, Módulo GPS NEO-6M

> Se recomienda usar una distribución tipo "barril o bus" para conectar en paralelo, asegurando un cableado robusto, con soldadura o terminales de buena calidad para evitar caídas de voltaje o falsos contactos.

---

###  Conexiones físicas entre componentes

#### Heltec LoRa 32 (Placa 1 - Transmisora)

* **Sensor DHT11**

  * VCC → 5V
  * GND → GND
  * DATA → GPIO 14
* **Sensor de vibración SW-420**

  * VCC → 5V
  * GND → GND
  * DO → GPIO 13
* **Botón de emergencia**

  * Un pin → GPIO 27
  * Otro pin → GND (modo PULL-UP)
* **Módulo GPS NEO-6M**

  * VCC → 5V
  * GND → GND
  * TX → GPIO 17 (RX1)
  * RX → GPIO 16 (TX1)
* **Pantalla OLED (I2C)**

  * SDA → GPIO 4
  * SCL → GPIO 15

#### Heltec LoRa 32 (Placa 2 - Receptora intermedia)

* Alimentación: VCC → 5V, GND → GND
* Comunicación LoRa: configurada para recibir del transmisor y reenviar por ESP-NOW
* Pantalla OLED (I2C): SDA → GPIO 4, SCL → GPIO 15

#### LILYGO SIM7600G-H (Placa 3 - Receptora final con GPRS)

* Alimentación directa: **VCC → 5V**, GND → GND

  * Importante: Este modelo soporta alimentación entre 3.7V y 5V regulada.
* Comunicación ESP-NOW: Receptor final
* Comunicación GSM/GPRS: mediante comandos AT desde el microcontrolador interno
* Pantalla OLED (opcional): SDA → GPIO 21, SCL → GPIO 22

<img src="/documentacion/funcionalidades/Liveroute.jpg" alt="Prototipo" width="300" />
*Descripción:* Usuario configurando los sensores de temperatura, turbidez y pH en la placa LoRa 32 v2.
---

###  Consideraciones adicionales

* Se recomienda usar un **fusible de protección** entre la batería de la motocicleta y el regulador DC-DC.
* Se verificó con multímetro que cada punto recibe los 5V esperados antes de energizar las placas.
