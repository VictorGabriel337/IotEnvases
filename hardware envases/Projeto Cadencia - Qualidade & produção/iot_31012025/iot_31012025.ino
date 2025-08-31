#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>  // Biblioteca para HTTP

#define SENSOR_PIN A0           // Pino analógico para leitura do sensor
#define THRESHOLD_LOW 400       // Limite de tensão para LOW
#define THRESHOLD_HIGH 800      // Limite de tensão para HIGH

const char* ssid = "Aimee1306"; // Nome da rede Wi-Fi
const char* password = "@fernandacampos1306"; // Senha da rede


WiFiClient client;
HTTPClient http;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço do display LCD

unsigned long lastLowTime = 0;       
unsigned long currentTime = 0;      
unsigned long stateStartTime = 0;   
unsigned long lastEdgeTime = 0;     
unsigned long lastDebounceTime = 0;

unsigned long cadenceTotalTime = 0;
unsigned long nonCadenceTotalTime = 0;
unsigned int lowSignalCount = 0;
bool inCadence = true;
int edgeCount = 0;
int lastSensorState = HIGH;
int stableSensorState = HIGH;

void setup() {
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  stateStartTime = millis();
  lcd.clear();

  // Conecta ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print("...");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado ao Wi-Fi!");
    Serial.print("IP do dispositivo: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha na conexão Wi-Fi.");
  }
}

void loop() {
  currentTime = millis();
  int sensorValue = analogRead(SENSOR_PIN);

  int sensorState = map(sensorValue, 0, 1023, 0, 1023);
  if (sensorState < THRESHOLD_LOW) {
    sensorState = LOW;
  } else if (sensorState > THRESHOLD_HIGH) {
    sensorState = HIGH;
  } else {
    sensorState = lastSensorState;
  }

  if (sensorState == LOW && lastSensorState == HIGH) {
    lastLowTime = currentTime;
    lastEdgeTime = currentTime;
    lowSignalCount++;

    Serial.print("Detecção LOW. Contagem: "); 
    Serial.println(lowSignalCount);

    if (!inCadence) { 
      edgeCount++;
      Serial.print("Borda detectada (HIGH -> LOW). Contagem: ");
      Serial.println(edgeCount);
    }

    if (!inCadence && edgeCount >= 5) {
      nonCadenceTotalTime += (currentTime - stateStartTime);
      Serial.print("Tempo acumulado fora de cadência: ");
      printTime(nonCadenceTotalTime / 1000);
      stateStartTime = currentTime;
      Serial.println("Máquina voltou à cadência.");
      inCadence = true;
      edgeCount = 0;
    }
  }

  if ((currentTime - lastLowTime) >= 10000 && inCadence) {
    cadenceTotalTime += (currentTime - stateStartTime);
    Serial.print("Tempo acumulado em cadência: ");
    printTime(cadenceTotalTime / 1000);
    stateStartTime = currentTime;
    Serial.println("A máquina saiu de cadência!");
    inCadence = false;
    edgeCount = 0;
  }

  // Enviar dados para o proxy no Railway
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Enviando dados para o Railway...");
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{";
    jsonPayload += "\"lowSignalCount\":" + String(lowSignalCount) + ",";
    jsonPayload += "\"cadenceTotalTime\":" + String(cadenceTotalTime / 1000) + ",";
    jsonPayload += "\"nonCadenceTotalTime\":" + String(nonCadenceTotalTime / 1000);
    jsonPayload += "}";

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.print("Resposta do servidor: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erro na requisição: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  lcd.clear();
  if (inCadence) {
    unsigned long currentCadenceTime = currentTime - stateStartTime;
    lcd.setCursor(0, 0);
    lcd.print("Cadencia ativa:");
    lcd.setCursor(5, 1);
    printTimeLCD(currentCadenceTime / 1000);
  } else {
    unsigned long currentNonCadenceTime = currentTime - stateStartTime;
    lcd.setCursor(0, 0);
    lcd.print("Fora de cadencia:");
    lcd.setCursor(5, 1);
    printTimeLCD(currentNonCadenceTime / 1000);
  }

  delay(1000);
}

// Função para exibir tempo no formato MM:SS
void printTime(unsigned long totalSeconds) {
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");
  if (seconds < 10) Serial.print("0");
  Serial.println(seconds);
}

void printTimeLCD(unsigned long totalSeconds) {
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
}
