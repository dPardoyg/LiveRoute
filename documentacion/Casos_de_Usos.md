# Casos de Uso

En este apartado se describe los principales casos de uso del sistema de monitoreo IoT para motocicleta, detallando el comportamiento esperado y las acciones realizadas por el sistema para cada tipo de usuario.

---

## Caso de Uso 1: Monitoreo en Tiempo Real (Usuario Final)

**Actor Principal:** Repartidor (usuario en la motocicleta)
**Propósito:** Transmitir automáticamente información de sensores durante el trayecto de la entrega.
**Precondiciones:** El sistema está encendido y conectado correctamente.
**Escenario Principal:**

* El sensor DHT11 mide temperatura y humedad ambiental.
* El sensor SW-420 detecta movimientos/vibraciones inusuales.
* El módulo GPS obtiene las coordenadas de ubicación en tiempo real.
* Si se presiona el botón de ayuda, se activa una alerta.
* La placa Heltec LoRa V2 (transmisora) envía los datos por LoRa.
* La segunda Heltec LoRa V2 (receptora) retransmite los datos por ESP-NOW.
* La placa LILYGO SIM7600 los publica a un broker MQTT vía GPRS.
* Node-RED recibe, filtra y almacena la información en InfluxDB.
* Grafana presenta los datos en un tablero de visualización en tiempo real.

**Resultado Esperado:** Los datos son capturados, transmitidos, almacenados y visualizados correctamente.

---

## Caso de Uso 2: Alertas por Botón de Emergencia (Administrador / Usuario Final)

**Actor Principal:** Repartidor / Administrador del sistema
**Propósito:** Notificar un incidente en tiempo real mediante Telegram.
**Precondiciones:** El sistema tiene acceso a red móvil (GPRS/EDGEM).
**Escenario Principal:**

* El botón de emergencia es presionado por el usuario.
* La señal se detecta y se marca como `"message": 1` en el JSON.
* Se activa el flujo en Node-RED que desencadena el envío de un mensaje con coordenadas por Telegram.

**Resultado Esperado:** El administrador recibe inmediatamente una alerta por Telegram con la ubicación del incidente.

---

## Caso de Uso 3: Análisis Histórico (Administrador / Proveedor del Servicio)

**Actor Principal:** Administrador o Analista de logística
**Propósito:** Consultar datos históricos de recorrido, temperatura, vibración, ubicación, etc.
**Precondiciones:** Existencia de datos almacenados en la base de datos InfluxDB.
**Escenario Principal:**

* El administrador accede a Grafana.
* Selecciona el intervalo de tiempo deseado.
* Consulta las métricas de viaje: temperatura, humedad, movimiento y ubicación.

**Resultado Esperado:** Se presentan visualizaciones gráficas y estadísticas sobre el desempeño del vehículo y las condiciones del trayecto.

---

## Caso de Uso 4: Administración del Sistema (Administrador)

**Actor Principal:** Administrador técnico del sistema
**Propósito:** Configurar, mantener y monitorear el sistema IoT completo.
**Precondiciones:** Acceso físico o remoto a los dispositivos.
**Escenario Principal:**

* Revisión del estado de conexión de cada dispositivo.
* Actualización del firmware o flujos Node-RED.
* Mantenimiento preventivo (verificación de conexiones, limpieza de sensores, etc).

**Resultado Esperado:** El sistema se mantiene funcional y actualizado, garantizando su operatividad continua.
