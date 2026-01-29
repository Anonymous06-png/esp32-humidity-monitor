#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

/* ================= WIFI CONFIG ================= */
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

/* ================= MQTT CONFIG ================= */
const char* mqtt_server = "YOUR_IP";  // IP máy chạy Node-RED
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/humidity_monitor";
const char* mqtt_client_id = "ESP32_HumidityMonitor";

/* ================= OLED CONFIG ================= */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* ================= PIN CONFIG ================= */
#define RELAY_PIN   27   // HIGH = relay ON
#define LED_PIN     18   // HIGH = LED ON
#define BUZZER_PIN  19   // LOW  = buzzer ON

/* ================= HUMIDITY THRESHOLD ================= */
#define HUMI_ON   70.0
#define HUMI_OFF  65.0

/* ================= GLOBAL OBJECT ================= */
Adafruit_AHTX0 aht;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
bool systemActive = false;

/* ================= MQTT RECONNECT ================= */
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    
    if (mqttClient.connect(mqtt_client_id)) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retry in 5s...");
      delay(5000);
    }
  }
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);
  delay(500);

  // GPIO
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, HIGH);

  // I2C
  Wire.begin(21, 22);

  // AHT30
  if (!aht.begin()) {
    Serial.println("ERROR: AHT30 not found!");
    while (1);
  }

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ERROR: OLED not found!");
    while (1);
  }

  // Splash
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Humidity");
  display.println("Monitor");
  display.display();
  delay(1500);

  /* ===== WIFI CONNECT ===== */
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  /* ===== MQTT SETUP ===== */
  mqttClient.setServer(mqtt_server, mqtt_port);
  reconnectMQTT();
}

/* ================= LOOP ================= */
void loop() {
  // Check MQTT connection
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  sensors_event_t humidity, temperature;
  aht.getEvent(&humidity, &temperature);

  float humi = humidity.relative_humidity;
  float temp = temperature.temperature;

  /* ===== HYSTERESIS (LOGIC CŨ - GIỮ NGUYÊN) ===== */
  if (!systemActive && humi >= HUMI_ON) {
    systemActive = true;
  } else if (systemActive && humi <= HUMI_OFF) {
    systemActive = false;
  }

  /* ===== OUTPUT (LOGIC CŨ - GIỮ NGUYÊN) ===== */
  digitalWrite(RELAY_PIN, systemActive ? HIGH : LOW);
  digitalWrite(LED_PIN, systemActive ? HIGH : LOW);
  digitalWrite(BUZZER_PIN, systemActive ? LOW : HIGH);

  /* ===== SERIAL ===== */
  Serial.print("Temp: ");
  Serial.print(temp, 1);
  Serial.print(" C | Humidity: ");
  Serial.print(humi, 0);
  Serial.print(" % | System: ");
  Serial.println(systemActive ? "ON" : "OFF");

  /* ===== MQTT PUBLISH ===== */
  if (WiFi.status() == WL_CONNECTED && mqttClient.connected()) {
    // Tạo JSON document
    StaticJsonDocument<200> doc;
    doc["temperature"] = round(temp * 10) / 10.0;  // 1 chữ số thập phân
    doc["humidity"] = round(humi);                  // Làm tròn
    doc["alert"] = systemActive ? 1 : 0;
    
    // Serialize to string
    char jsonBuffer[200];
    serializeJson(doc, jsonBuffer);
    
    // Publish
    if (mqttClient.publish(mqtt_topic, jsonBuffer)) {
      Serial.print("MQTT published: ");
      Serial.println(jsonBuffer);
    } else {
      Serial.println("MQTT publish failed!");
    }
  }

  /* ===== OLED (LOGIC CŨ - GIỮ NGUYÊN) ===== */
  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("H:");
  display.print(humi, 0);
  display.print("%");

  display.setCursor(66, 0);
  display.print("T:");
  display.print(temp, 0);
  display.write((char)247);
  display.print("C");

  display.setTextSize(1);
  display.setCursor(0, 32);
  display.println("----------------");

  display.setTextSize(2);
  display.setCursor(16, 44);
  display.print(systemActive ? "ALERT ON" : "NORMAL");

  display.display();

  delay(3000);
}
