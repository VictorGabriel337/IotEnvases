#include <HX711.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Credenciais WiFi
const char* ssid = "iotenvases";
const char* password = "iotenvases42";

// Credenciais MQTT HiveMQ Cloud
const char* mqtt_server   = "534dc0a4d7544a60a30022826acda692.s1.eu.hivemq.cloud";
const char* mqtt_username = "Iotenvases";
const char* mqtt_password = "Iotenvases42";
const int   mqtt_port     = 8883;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600); // horário Brasília

// Certificado raiz
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
... (mantém seu certificado completo aqui) ...
-----END CERTIFICATE-----
)EOF";

// Objetos de rede
WiFiClientSecure espClient;
PubSubClient client(espClient);

// HX711
#define pinDT  D6
#define pinSCK D7
HX711 scale;

float medida = 0;
float medidaAnterior = 0;
int contador = 0;

const float pesoPeca       = 171.0;
const float limiteInferior = 160.0;
const float limiteSuperior = 180.0;

void setup_wifi() {
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
  // se quiser processar mensagens recebidas MQTT
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
  Serial.begin(115200);

  setup_wifi();

#ifdef ESP8266
  espClient.setInsecure();   // ignora verificação do certificado (mais simples no ESP8266)
#else
  espClient.setCACert(root_ca);
#endif

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  timeClient.begin();

  // Configuração da balança
  scale.begin(pinDT, pinSCK);
  scale.set_scale(-1755.33);   // ajuste conforme calibragem
  delay(2000);
  scale.tare();
  Serial.println("Balança Zerada");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timeClient.update();

  medida = scale.get_units(5);

  // Filtro de ruído
  if (abs(medida) <= 4.0) medida = 0;
  if (medida < 0) medida = 0;

  Serial.print("Peso atual: ");
  Serial.print(medida);
  Serial.println(" g");

  float diferenca = medida - medidaAnterior;

  // Contagem de peças
  if (diferenca >= limiteInferior) {
    int pecasAdicionadas = (int)(diferenca / pesoPeca);
    if (pecasAdicionadas > 0) {
      contador += pecasAdicionadas;
      Serial.print("Peças adicionadas: ");
      Serial.println(pecasAdicionadas);
      medidaAnterior = medida;
    }
  } else if (diferenca <= -limiteInferior) {
    int pecasRemovidas = (int)(-diferenca / pesoPeca);
    if (pecasRemovidas > 0) {
      contador -= pecasRemovidas;
      if (contador < 0) contador = 0;
      Serial.print("Peças removidas: ");
      Serial.println(pecasRemovidas);
      medidaAnterior = medida;
    }
  }

  Serial.print("Total de peças: ");
  Serial.println(contador);

  // Publica no MQTT a contagem
  if (client.connected()) {
    String payload = "{\"pieceCount\": " + String(contador) + "}";
    client.publish("machine/piece", payload.c_str());
  }

  scale.power_down();
  delay(1000);
  scale.power_up();
}
