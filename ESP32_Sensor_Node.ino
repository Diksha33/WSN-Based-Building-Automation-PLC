#include <WiFi.h>
#include <ModbusIP_ESP8266.h>
#include <DHT.h>   // "DHT sensor library" by Adafruit (also install "Adafruit Unified Sensor")

// =====================================================
// 1. WIFI CONFIGURATION
// =====================================================
const char* ssid     = "FTTH-9942";
const char* password = "ESP32test2026";

// Static IP must match RemoteAddress in the PLC's DB2 (192.168.0.3).
// Set USE_STATIC_IP to false to use DHCP instead.
#define USE_STATIC_IP true
IPAddress local_IP(192, 168, 0, 3);
IPAddress gateway(192, 168, 0, 1);     // your router IP
IPAddress subnet(255, 255, 255, 0);

// =====================================================
// 2. MODBUS REGISTER MAP  (PLC address = 40001 + offset)
// Block 1 (TEMP)  : ADDR 40101, LEN 1
// Block 2 (smoke) : ADDR 40102, LEN 1
// Humidity        : ADDR 40110, LEN 1
// =====================================================
ModbusIP mb;

const uint16_t HREG_TEMP        = 100;  // 40101  temperature x100 (2500 = 25.00 C)
const uint16_t HREG_GAS_AO      = 101;  // 40102  smoke/gas analog 0-4095
const uint16_t HREG_GAS_ALARM   = 102;  // 40103  1 = gas detected
const uint16_t HREG_FLAME_ALARM = 103;  // 40104  1 = flame detected
const uint16_t HREG_RAIN_AO     = 104;  // 40105  rain analog 0-4095
const uint16_t HREG_RAIN_ALARM  = 105;  // 40106  1 = rain detected
const uint16_t HREG_TEMP_STATUS = 106;  // 40107  0 = OK, 1 = DHT11 read fault
const uint16_t HREG_HEARTBEAT   = 107;  // 40108  increments every second
const uint16_t HREG_BTN_COUNT   = 108;  // 40109  push button press counter
const uint16_t HREG_HUMIDITY    = 109;  // 40110  humidity x100 (6500 = 65.00 %RH)

// =====================================================
// 3. PIN CONFIGURATION
// =====================================================
// DHT11 data pin
const int DHT_PIN = 4;
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Analog pins must be ADC1 (32-39), because ADC2 does not work with WiFi.
const int GAS_AO_PIN   = 34;   // smoke AO
const int RAIN_AO_PIN  = 33;
const int GAS_DO_PIN   = 27;   // smoke DO
const int FLAME_DO_PIN = 26;
const int RAIN_DO_PIN  = 25;

// Push button: one leg to GPIO 14, diagonal leg to GND (internal pull-up used)
const int BTN_PIN = 14;

// Most sensor modules pull DO LOW when triggered. Set false if yours is the opposite.
const bool GAS_ACTIVE_LOW   = true;
const bool FLAME_ACTIVE_LOW = true;
const bool RAIN_ACTIVE_LOW  = true;

// =====================================================
// 4. VARIABLES
// =====================================================
float    temperature = 0.0;
float    humidity    = 0.0;
uint16_t scaledTemp  = 0;      // last good value is kept on fault
uint16_t scaledHum   = 0;      // last good value is kept on fault
uint16_t tempStatus  = 0;
uint16_t heartbeat   = 0;

// Button
uint16_t btnCount = 0;
bool lastBtnState = HIGH;
unsigned long lastBtnTime = 0;

unsigned long previousMillis = 0;
const unsigned long interval = 1000;

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 10000;

// =====================================================
// 5. HELPERS
// =====================================================
uint16_t readAnalogAvg(int pin, int samples = 8) {
  uint32_t sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delayMicroseconds(200);
  }
  return (uint16_t)(sum / samples);
}

uint16_t readAlarm(int pin, bool activeLow) {
  int v = digitalRead(pin);
  return (activeLow ? (v == LOW) : (v == HIGH)) ? 1 : 0;
}

void connectWiFi() {
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);

#if USE_STATIC_IP
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Static IP config failed");
  }
#endif

  WiFi.begin(ssid, password);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) {   // about 20 s timeout
    delay(500);
    Serial.print(".");
    tries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("===== WIFI CONNECTED =====");
    Serial.print("ESP32 IP : "); Serial.println(WiFi.localIP());
    Serial.print("Gateway  : "); Serial.println(WiFi.gatewayIP());
    Serial.print("Subnet   : "); Serial.println(WiFi.subnetMask());
    Serial.print("MAC      : "); Serial.println(WiFi.macAddress());
  } else {
    Serial.println("WiFi connection failed. Will keep retrying in loop.");
  }
}

// Checks the button every loop pass so a quick press is never missed
void checkButton() {
  bool btn = digitalRead(BTN_PIN);
  if (btn != lastBtnState && millis() - lastBtnTime > 50) {   // 50 ms debounce
    lastBtnTime = millis();
    lastBtnState = btn;
    if (btn == LOW) {                       // pressed
      btnCount++;
      mb.Hreg(HREG_BTN_COUNT, btnCount);
      Serial.print("Button pressed, count = ");
      Serial.println(btnCount);
    }
  }
}

// =====================================================
// 6. SETUP
// =====================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=================================");
  Serial.println(" ESP32 BUILDING MANAGEMENT SYSTEM");
  Serial.println("=================================");

  pinMode(GAS_DO_PIN, INPUT);
  pinMode(FLAME_DO_PIN, INPUT);
  pinMode(RAIN_DO_PIN, INPUT);
  pinMode(GAS_AO_PIN, INPUT);
  pinMode(RAIN_AO_PIN, INPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  analogReadResolution(12);

  dht.begin();

  connectWiFi();

  mb.server();   // Modbus TCP server, port 502

  mb.addHreg(HREG_TEMP, 0);
  mb.addHreg(HREG_GAS_AO, 0);
  mb.addHreg(HREG_GAS_ALARM, 0);
  mb.addHreg(HREG_FLAME_ALARM, 0);
  mb.addHreg(HREG_RAIN_AO, 0);
  mb.addHreg(HREG_RAIN_ALARM, 0);
  mb.addHreg(HREG_TEMP_STATUS, 0);
  mb.addHreg(HREG_HEARTBEAT, 0);
  mb.addHreg(HREG_BTN_COUNT, 0);
  mb.addHreg(HREG_HUMIDITY, 0);

  Serial.println("Modbus TCP Server Started (port 502)");
  delay(1000);   // DHT11 needs about 1 s after power-up
}

// =====================================================
// 7. READ SENSORS
// =====================================================
void readSensors() {
  // ---- Temperature + Humidity (DHT11) ----
  float t = dht.readTemperature();   // Celsius
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    tempStatus = 1;   // keep last good values
    Serial.println("WARNING: DHT11 read failed. Check wiring (DATA on GPIO 4, VCC, GND).");
  } else {
    tempStatus = 0;
    temperature = t;
    humidity    = h;
    // x100, clamped so a signed Int in the PLC never overflows
    scaledTemp = (uint16_t)constrain((int)roundf(temperature * 100.0f), 0, 32767);
    scaledHum  = (uint16_t)constrain((int)roundf(humidity * 100.0f), 0, 10000);
  }

  // ---- Gas / Flame / Rain ----
  uint16_t gasAO      = readAnalogAvg(GAS_AO_PIN);
  uint16_t gasAlarm   = readAlarm(GAS_DO_PIN, GAS_ACTIVE_LOW);
  uint16_t flameAlarm = readAlarm(FLAME_DO_PIN, FLAME_ACTIVE_LOW);
  uint16_t rainAO     = readAnalogAvg(RAIN_AO_PIN);
  uint16_t rainAlarm  = readAlarm(RAIN_DO_PIN, RAIN_ACTIVE_LOW);

  // ---- Write to Modbus ----
  mb.Hreg(HREG_TEMP, scaledTemp);
  mb.Hreg(HREG_HUMIDITY, scaledHum);
  mb.Hreg(HREG_GAS_AO, gasAO);
  mb.Hreg(HREG_GAS_ALARM, gasAlarm);
  mb.Hreg(HREG_FLAME_ALARM, flameAlarm);
  mb.Hreg(HREG_RAIN_AO, rainAO);
  mb.Hreg(HREG_RAIN_ALARM, rainAlarm);
  mb.Hreg(HREG_TEMP_STATUS, tempStatus);
  mb.Hreg(HREG_HEARTBEAT, ++heartbeat);
}

// =====================================================
// 8. SERIAL DISPLAY
// =====================================================
void printSensorData() {
  Serial.println();
  Serial.println("=========== BMS DATA ===========");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
  Serial.print("Temperature : ");
  if (tempStatus) Serial.println("FAULT");
  else { Serial.print(temperature, 1); Serial.println(" C"); }
  Serial.print("Humidity    : ");
  if (tempStatus) Serial.println("FAULT");
  else { Serial.print(humidity, 1); Serial.println(" %RH"); }
  Serial.print("R100 TEMP x100  = "); Serial.println(mb.Hreg(HREG_TEMP));
  Serial.print("R101 SMOKE AO   = "); Serial.println(mb.Hreg(HREG_GAS_AO));
  Serial.print("R102 GAS ALARM  = "); Serial.println(mb.Hreg(HREG_GAS_ALARM));
  Serial.print("R103 FLAME ALM  = "); Serial.println(mb.Hreg(HREG_FLAME_ALARM));
  Serial.print("R104 RAIN AO    = "); Serial.println(mb.Hreg(HREG_RAIN_AO));
  Serial.print("R105 RAIN ALARM = "); Serial.println(mb.Hreg(HREG_RAIN_ALARM));
  Serial.print("R106 TEMP STAT  = "); Serial.println(mb.Hreg(HREG_TEMP_STATUS));
  Serial.print("R107 HEARTBEAT  = "); Serial.println(mb.Hreg(HREG_HEARTBEAT));
  Serial.print("R108 BTN COUNT  = "); Serial.println(mb.Hreg(HREG_BTN_COUNT));
  Serial.print("R109 HUMIDITY x100 = "); Serial.println(mb.Hreg(HREG_HUMIDITY));
  Serial.println("================================");
}

// =====================================================
// 9. MAIN LOOP
// =====================================================
void loop() {
  mb.task();      // keep Modbus alive; never block the loop for long
  checkButton();  // runs every pass, so presses are caught immediately

  unsigned long now = millis();

  if (now - previousMillis >= interval) {
    previousMillis = now;

    if (WiFi.status() == WL_CONNECTED) {
      readSensors();
      printSensorData();
    } else if (now - lastReconnectAttempt >= reconnectInterval) {
      lastReconnectAttempt = now;
      Serial.println("WiFi disconnected, reconnecting...");
      WiFi.disconnect();
      WiFi.reconnect();
    }
  }
}
