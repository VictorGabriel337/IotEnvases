#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SENSOR_PIN D6            // Pino digital para leitura do sensor
#define DEBOUNCE_DELAY 50        // Tempo de debounce para estabilização do sinal (não usado aqui)

const char* ssid = "Aimee1306";
const char* password = "@fernandacampos1306";

const char* mqtt_server = "534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud";
const char* mqtt_username = "Iotenvases";
const char* mqtt_password = "Iotenvases42";
const int mqtt_port = 8883;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600); // Horário de Brasília

static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastLowTime = 0;
unsigned long currentTime = 0;
unsigned long stateStartTime = 0;
unsigned long lastEdgeTime = 0;
unsigned long cadenceTotalTime = 0;
unsigned long nonCadenceTotalTime = 0;
unsigned int lowSignalCount = 0;
bool inCadence = true;
int edgeCount = 0;
int lastSensorState = HIGH;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup_wifi() {
  Serial.begin(115200);
  delay(10);
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado. IP:");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Mensagens recebidas via MQTT podem ser processadas aqui
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT... ");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Conectado ao MQTT.");
    } else {
      Serial.print("Falha ao conectar. Código: ");
      Serial.print(client.state());
      Serial.println(". Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  stateStartTime = millis();
  lcd.clear();

  setup_wifi();

  #ifdef ESP8266
  espClient.setInsecure();
  #else
  espClient.setCACert(root_ca);
  #endif

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  timeClient.begin();
}

void loop() {
  currentTime = millis();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timeClient.update();

  int sensorState = digitalRead(SENSOR_PIN);

  if (sensorState == LOW && lastSensorState == HIGH) {
    lastLowTime = currentTime;
    lastEdgeTime = currentTime;
    lowSignalCount++;

    if (!inCadence) {
      edgeCount++;
    }

    if (!inCadence && edgeCount >= 5) {
      nonCadenceTotalTime += (currentTime - stateStartTime);
      stateStartTime = currentTime;
      inCadence = true;
      edgeCount = 0;
    }
  }

  if ((currentTime - lastLowTime) >= 300000 && inCadence) {
    cadenceTotalTime += (currentTime - stateStartTime);
    stateStartTime = currentTime;
    inCadence = false;
    edgeCount = 0;
  }

  // Publicação MQTT
  if (client.connected()) {
    String cadenceStatus = inCadence ? "Em cadência" : "Fora de cadência";

    unsigned long currentCadenceTime = 0;
    unsigned long currentNonCadenceTime = 0;

    if (inCadence) {
      currentCadenceTime = (currentTime - stateStartTime) / 1000;
    } else {
      currentNonCadenceTime = (currentTime - stateStartTime) / 1000;
    }

    String payload = "{\"cadenceStatus\": \"" + cadenceStatus + "\", "
                     "\"lowSignalCount\": " + String(lowSignalCount) + ", "
                     "\"cadenceTotalTime\": " + String(cadenceTotalTime / 1000) + ", "
                     "\"nonCadenceTotalTime\": " + String(nonCadenceTotalTime / 1000) + ", "
                     "\"currentCadenceTime\": " + String(currentCadenceTime) + ", "
                     "\"currentNonCadenceTime\": " + String(currentNonCadenceTime) + "}";

    client.publish("machine/status", payload.c_str());
  }

  // LCD Display
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

  lastSensorState = sensorState;

  // Reset diário
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  if (currentHour == 0 && currentMinute == 0) {
    lowSignalCount = 0;
    cadenceTotalTime = 0;
    nonCadenceTotalTime = 0;
  }
}

// Função para exibir MM:SS no LCD
void printTimeLCD(unsigned long totalSeconds) {
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
}
