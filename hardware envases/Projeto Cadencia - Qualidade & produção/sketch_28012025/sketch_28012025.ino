#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer>

#define SENSOR_PIN A0           // Pino analógico para leitura do sensor
#define THRESHOLD_LOW 400       // Limite de tensão para LOW (ajustável)
#define THRESHOLD_HIGH 800      // Limite de tensão para HIGH (ajustável)
#define DEBOUNCE_DELAY 50       // Tempo de debounce para estabilização do sinal

const char* ssid = "Aimee1306";         // Nome da rede Wi-Fi
const char* password = "@fernandacampos1306";    // Senha da rede Wi-Fi

WiFiServer server(80);  // Cria um servidor na porta 80

unsigned long lastLowTime = 0;       // Armazena o tempo da última detecção de LOW
unsigned long currentTime = 0;      // Armazena o tempo atual
unsigned long stateStartTime = 0;   // Marca o início do estado atual (cadência ou fora de cadência)
unsigned long lastEdgeTime = 0;     // Tempo da última alternância de HIGH para LOW
unsigned long lastDebounceTime = 0; // Tempo da última leitura estável

unsigned long cadenceTotalTime = 0;    // Tempo acumulado em cadência
unsigned long nonCadenceTotalTime = 0; // Tempo acumulado fora de cadência
unsigned int lowSignalCount = 0;       // Contador de sinais LOW (não reseta)
bool inCadence = true;                 // Indica se a máquina está em cadência
int edgeCount = 0;                     // Contador de bordas descendentes (HIGH para LOW)
int lastSensorState = HIGH;            // Estado anterior do sensor
int stableSensorState = HIGH;          // Estado estável do sensor após debounce

LiquidCrystal_I2C lcd(0x27, 16, 2);    // Endereço do display LCD, com 16 colunas e 2 linhas

void setup() {
  pinMode(SENSOR_PIN, INPUT);         // Configura o pino analógico para leitura
  Serial.begin(115200);
  lcd.init();                         // Inicializa o display LCD
  lcd.backlight();                    // Liga a luz de fundo do display
  stateStartTime = millis();          // Inicializa o tempo de estado
  lcd.clear();

  // Conecta ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado ao Wi-Fi!");
    Serial.print("IP do dispositivo: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha na conexão Wi-Fi.");
  }

  // Inicia o servidor
  server.begin();
}



void loop() {
  currentTime = millis();

  // Lê o valor do pino analógico
  int sensorValue = analogRead(SENSOR_PIN);

  // Converte o valor analógico para HIGH ou LOW com base nos limites
  int sensorState = map(sensorValue, 0, 1023, 0, 1023); // Mapeia o valor analógico
  if (sensorState < THRESHOLD_LOW) {
    sensorState = LOW;
  } else if (sensorState > THRESHOLD_HIGH) {
    sensorState = HIGH;
  } else {
    sensorState = lastSensorState;  // Mantém o estado anterior se estiver entre os limites
  }

  // Detecta borda descendente (transição de HIGH para LOW no estado estável)
  if (sensorState == LOW && lastSensorState == HIGH) {
    lastLowTime = currentTime;   // Atualiza o tempo da última detecção de LOW
    lastEdgeTime = currentTime;  // Atualiza o tempo da última borda
    if (!inCadence) {            // Incrementa o contador somente fora de cadência
      edgeCount++;
      Serial.print("Borda detectada (HIGH -> LOW). Contagem: ");
      Serial.println(edgeCount);
    }

    if (!inCadence && edgeCount >= 5) {
      // Máquina voltou à cadência após 5 bordas descendentes
      nonCadenceTotalTime += (currentTime - stateStartTime); // Atualiza tempo fora de cadência
      Serial.print("Tempo acumulado fora de cadência: ");
      printTime(nonCadenceTotalTime / 1000); // Exibe o tempo fora de cadência
      stateStartTime = currentTime; // Reinicia o tempo de estado
      Serial.println("Máquina voltou à cadência.");
      inCadence = true; // Atualiza o estado
      edgeCount = 0;    // Reseta o contador de bordas
    }
  }

  // Verifica se mais de 10 segundos se passaram desde o último LOW para sair de cadência
  if ((currentTime - lastLowTime) >= 10000 && inCadence) {
    // Máquina saiu de cadência
    cadenceTotalTime += (currentTime - stateStartTime); // Atualiza tempo em cadência
    Serial.print("Tempo acumulado em cadência: ");
    printTime(cadenceTotalTime / 1000); // Exibe o tempo em cadência
    stateStartTime = currentTime; // Reinicia o tempo de estado
    Serial.println("A máquina saiu de cadência!");
    inCadence = false; // Atualiza o estado
    edgeCount = 0; // Reseta o contador de bordas
  }

  // Aguarda por um cliente para servir as informações via HTTP
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Novo cliente conectado!");

    // Espera até que o cliente envie dados
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        Serial.println(request);
        client.flush();

        // Responde com o valor do sensor
        client.print("HTTP/1.1 200 OK\r\n");
        client.print("Content-Type: application/json\r\n");
        client.print("\r\n");
        client.print("{\"sensor\": ");
        client.print(sensorValue);
        client.print(", \"cadence\": ");
        client.print(inCadence ? "true" : "false");
        client.print("}\r\n");

        break;
      }
    }

    // Fecha a conexão com o cliente
    client.stop();
    Serial.println("Cliente desconectado.");
  }

  // Exibe o tempo atual no estado e total acumulado no LCD
  lcd.clear(); // Limpa o display antes de exibir novas informações
  if (inCadence) {
    unsigned long currentCadenceTime = currentTime - stateStartTime;
    lcd.setCursor(0, 0); // Linha 0, coluna 0
    lcd.print("Cadencia ativa:");
    lcd.setCursor(5, 1); // Linha 1, coluna 0
    printTimeLCD(currentCadenceTime / 1000); // Exibe o tempo no formato MM:SS
  } else {
    unsigned long currentNonCadenceTime = currentTime - stateStartTime;
    lcd.setCursor(0, 0); // Linha 0, coluna 0
    lcd.print("Fora de cadencia:");
    lcd.setCursor(5, 1); // Linha 1, coluna 0
    printTimeLCD(currentNonCadenceTime / 1000); // Exibe o tempo no formato MM:SS
  }

  delay(1000); // Atualiza as informações a cada 1 segundo
}

// Função para exibir o tempo no formato MM:SS no serial
void printTime(unsigned long totalSeconds) {
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");
  if (seconds < 10) Serial.print("0");
  Serial.println(seconds);
}

// Função para exibir o tempo no formato MM:SS no LCD
void printTimeLCD(unsigned long totalSeconds) {
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
}
