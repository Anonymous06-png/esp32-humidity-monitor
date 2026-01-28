#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_SSD1306.h>

/* ================= WIFI CONFIG ================= */
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

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
bool systemActive = false;

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
}

/* ================= LOOP ================= */
void loop() {
  sensors_event_t humidity, temperature;
  aht.getEvent(&humidity, &temperature);

  float humi = humidity.relative_humidity;
  float temp = temperature.temperature;

  /* ===== HYSTERESIS ===== */
  if (!systemActive && humi >= HUMI_ON) {
    systemActive = true;
  } else if (systemActive && humi <= HUMI_OFF) {
    systemActive = false;
  }

  /* ===== OUTPUT ===== */
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

  /* ===== HTTP SEND ===== */
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://httpbin.org/post");
    http.addHeader("Content-Type", "application/json");

    String json = "{";
    json += "\"temperature\":" + String(temp, 1) + ",";
    json += "\"humidity\":" + String(humi, 0) + ",";
    json += "\"alert\":" + String(systemActive ? 1 : 0);
    json += "}";

    int httpCode = http.POST(json);
    Serial.print("HTTP code: ");
    Serial.println(httpCode);

    http.end();
  }

  /* ===== OLED ===== */
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
