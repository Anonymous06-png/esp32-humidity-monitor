# esp32-humidity-monitor
ESP32 Temperature & Humidity Monitoring using **MQTT** (IoT Project)

---

## 📌 Description
This project implements an **ESP32-based temperature and humidity monitoring system**
using the **MQTT protocol** for data communication and **Node-RED Dashboard** for
real-time visualization.

The ESP32 reads temperature and humidity data from an **AHT30 sensor**, displays
information locally on an **OLED screen**, and publishes the data to an **MQTT broker**.
Node-RED subscribes to the MQTT topic and visualizes the data on a web dashboard.

The system also supports **alerting and actuator control** (relay, LED, buzzer) when
humidity exceeds a predefined threshold.

---

## 🧠 System Architecture

AHT30 Sensor ->      ESP32     ->       MQTT Broker     -> Node-RED -> Web Dashboard.

---

## 🔧 Hardware Components
- ESP32 DevKit V1
- **AHT30 Temperature & Humidity Sensor (I2C)**
- OLED SSD1306 Display (I2C)
- Relay Module (fan / device control)
- LED + resistor
- Buzzer
- Breadboard & jumper wires

---

## 🧩 Software & Technologies
- Arduino IDE
- **MQTT protocol (Publish / Subscribe)**
- Node-RED
- PubSubClient (MQTT client for ESP32)
- ArduinoJson
- Adafruit AHTX0 Library
- Adafruit SSD1306 Library

---

## 📡 MQTT Communication
### MQTT Broker
- Local MQTT Broker (running together with Node-RED)

### MQTT Topic
esp32/humidity_monitor:

### MQTT Payload (JSON)
```json
{
  "temperature": 28.6,
  "humidity": 71.8,
  "alert": true
}
⚙️ System Operation
ESP32 connects to the configured Wi-Fi network.

ESP32 establishes a connection with the MQTT broker.

Temperature and humidity are read from the AHT30 sensor.

Humidity is compared against a predefined threshold.

Sensor data is published to the MQTT topic in JSON format.

Node-RED subscribes to the topic and updates the dashboard in real time.

When humidity exceeds the threshold:

Relay is activated

LED is turned on

Buzzer is triggered as an alert

📊 Node-RED Dashboard
The Node-RED dashboard displays:

🌡️ Temperature (°C)

💧 Humidity (%)

🚨 Alert status

📈 Real-time charts

▶️ How to Run the Project
1️⃣ ESP32 Setup
Open the .ino file in Arduino IDE.

Configure:

Wi-Fi SSID and password

MQTT broker IP address

Select board: DOIT ESP32 DEVKIT V1

Upload the code to ESP32.

2️⃣ Node-RED Setup:

node-red
Import the Node-RED MQTT flow.

Deploy the flow.

Open the dashboard:

http://<NodeRED_IP>:1880/ui
📂 Repository Structure
Copy code
.
├── esp32_humidity_mqtt_oled.ino
├── README.md

```
##🧪 Results

Stable MQTT communication between ESP32 and Node-RED.

Real-time temperature and humidity visualization.

Correct alert triggering when humidity exceeds the threshold.

System runs independently once ESP32 is powered.

##🚀 Future Improvements

Store sensor data in a database (InfluxDB).

Allow threshold configuration from the dashboard.

Use a cloud MQTT broker (HiveMQ / EMQX).

Support multiple ESP32 sensor nodes.

##👥 Team Members

Đinh Việt Kỳ – Hardware & ESP32 Firmware

Nguyễn Lâm Bảo Thạch – IoT Flow & Dashboard

Hoàng Ngọc Minh – Documentation & Presentation


