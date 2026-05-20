#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>
#include "DHTesp.h"

// =========================
// WIFI
// =========================
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

WebServer server(80);

// =========================
// PINOS
// =========================
#define DHT_PIN 18
#define BUTTON_PIN 33
#define LED_AZUL 26
#define LED_VERMELHO 27

// =========================
// LIMITES
// =========================
#define TEMP_FRIO 18.0
#define TEMP_QUENTE 23.0

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHTesp dhtSensor;

// =========================
// API CLIMA SP
// =========================
const char* SP_URL =
  "http://api.open-meteo.com/v1/forecast?latitude=-23.55&longitude=-46.63&current=temperature_2m";

// =========================
// VARIAVEIS
// =========================
unsigned long lastRequestTime = 0;
const unsigned long REQUEST_INTERVAL = 10000;

unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_TIME = 300;

unsigned long lastSPRequestTime = 0;
const unsigned long SP_REQUEST_INTERVAL = 30000;

bool mostrandoSP = false;

float temperaturaSala = 0.0;
float umidadeSala = 0.0;
float temperaturaSP = 0.0;

unsigned long lastBlinkTime = 0;
const unsigned long BLINK_INTERVAL = 500;
bool ledState = false;

// =========================
// WIFI
// =========================
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);

  Serial.print("Conectando ao WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void ensureWiFiConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado. Reconectando...");
    connectWiFi();
  }
}

// =========================
// LCD
// =========================
void atualizarLCD() {
  lcd.clear();

  if (!mostrandoSP) {
    lcd.setCursor(0, 0);
    lcd.print("Sala dos Pets");

    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(temperaturaSala, 1);
    lcd.print("C U:");
    lcd.print(umidadeSala, 0);
    lcd.print("%");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Sao Paulo");

    lcd.setCursor(0, 1);
    lcd.print("Temp:");
    lcd.print(temperaturaSP, 1);
    lcd.print("C");
  }
}

// =========================
// LEDS
// =========================
void atualizaLEDs() {
  bool deveAzul = temperaturaSala < TEMP_FRIO;
  bool deveVermelho = temperaturaSala > TEMP_QUENTE;

  unsigned long now = millis();

  if (now - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = now;
    ledState = !ledState;

    digitalWrite(LED_AZUL, deveAzul ? ledState : LOW);
    digitalWrite(LED_VERMELHO, deveVermelho ? ledState : LOW);
  }
}

// =========================
// LER SENSOR DA SALA
// =========================
void buscarTemperaturaSala() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();

  temperaturaSala = data.temperature;
  umidadeSala = data.humidity;

  Serial.println("\n--- SENSOR DHT22 ---");
  Serial.print("Temperatura Sala: ");
  Serial.println(temperaturaSala);
  Serial.print("Umidade Sala: ");
  Serial.println(umidadeSala);

  if (!mostrandoSP) {
    atualizarLCD();
  }
}

// =========================
// BUSCAR TEMPERATURA DE SP
// =========================
void buscarTemperaturaSP() {
  HTTPClient http;

  Serial.println("\n--- BUSCANDO TEMPERATURA DE SP ---");

  http.begin(SP_URL);

  int httpCode = http.GET();

  Serial.print("Status HTTP SP: ");
  Serial.println(httpCode);

  if (httpCode <= 0) {
    Serial.println("Erro ao buscar temperatura de SP");
    http.end();
    return;
  }

  String payload = http.getString();
  http.end();

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("Erro JSON SP: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc["current"]["temperature_2m"].isNull()) {
    Serial.println("Campo temperature_2m nao encontrado");
    return;
  }

  temperaturaSP = doc["current"]["temperature_2m"];

  Serial.print("Temperatura SP: ");
  Serial.println(temperaturaSP);

  if (mostrandoSP) {
    atualizarLCD();
  }
}

// =========================
// SETUP
// =========================
void setup() {

  Serial.begin(115200);

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("VetTemp");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  connectWiFi();

  buscarTemperaturaSala();
  buscarTemperaturaSP();

  atualizarLCD();

  // =========================
  // API GET
  // =========================

  server.on("/api/status", HTTP_GET, []() {
    String json = "{";
    json += "\"temperaturaSala\":" + String(temperaturaSala, 1) + ",";
    json += "\"umidadeSala\":" + String(umidadeSala, 1) + ",";
    json += "\"temperaturaSP\":" + String(temperaturaSP, 1);
    json += "}";

    server.send(200, "application/json", json);
  });

  server.begin();
}

// =========================
// LOOP
// =========================
void loop() {
  server.handleClient();

  unsigned long now = millis();

  static bool lastButtonState = HIGH;

  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    unsigned long nowButton = millis();

    if (nowButton - lastButtonPress > DEBOUNCE_TIME) {
      lastButtonPress = nowButton;

      mostrandoSP = !mostrandoSP;
      atualizarLCD();
    }
  }

  lastButtonState = currentButtonState;

  if (now - lastRequestTime >= REQUEST_INTERVAL) {
    lastRequestTime = now;

    ensureWiFiConnected();
    buscarTemperaturaSala();
  }

  atualizaLEDs();
}