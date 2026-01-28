# esp32-humidity-monitor
ESP32 Humidity Monitoring using HTTP (IoT Project)
# ESP32 Humidity Monitor (HTTP)

# ESP32 Humidity Monitor (HTTP)

## Description
This project uses ESP32 to monitor temperature and humidity, display data
locally on an OLED screen, and send sensor data to a server using HTTP
for IoT dashboard visualization.

## Features
- Read temperature & humidity using AHT30 sensor
- Display real-time data on OLED (SSD1306)
- Automatic humidity threshold control with hysteresis
- Cooling fan control via relay (external power supply)
- Visual and sound alerts using LED and buzzer
- Send sensor data to server via HTTP (JSON format)
- Data visualization on Node-RED dashboard

## Technology
- ESP32
- HTTP
- OLED (SSD1306)
- Relay module (fan control)
- Node-RED dashboard

## System Overview
The ESP32 continuously reads temperature and humidity values from the
AHT30 sensor. Measured data is shown on an OLED display and periodically
sent to a server using HTTP in JSON format.

When humidity exceeds a predefined threshold, the system automatically
activates a cooling fan through a relay module, along with LED and
buzzer alerts. The cooling fan is powered by an external supply and
controlled indirectly via relay to ensure electrical isolation and
system safety.

Collected data is visualized on a Node-RED dashboard for monitoring
and analysis.

## Note
This project was developed as part of the Embedded & IoT coursework.
The system architecture is designed to be simple, stable, and suitable
for small-scale IoT applications. The current implementation focuses on
HTTP-based communication for reliability and ease of deployment.

