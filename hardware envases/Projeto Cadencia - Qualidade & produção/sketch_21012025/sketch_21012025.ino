#define SENSOR_PIN D4

unsigned long lastLowTime = 0;       // Armazena o tempo da última detecção de LOW
unsigned long currentTime = 0;      // Armazena o tempo atual
unsigned long stateStartTime = 0;   // Marca o início do estado atual (cadência ou fora de cadência)
unsigned long lastEdgeTime = 0;     // Tempo da última alternância de HIGH para LOW
unsigned long lastPrintTime = 0;    // Tempo da última impressão no Serial

unsigned long cadenceTotalTime = 0;    // Tempo acumulado em cadência
unsigned long nonCadenceTotalTime = 0; // Tempo acumulado fora de cadência
bool inCadence = true;                 // Indica se a máquina está em cadência
int edgeCount = 0;                     // Contador de bordas descendentes (HIGH para LOW)
int lastSensorState = HIGH;            // Estado anterior do sensor

void setup() {
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(115200);
  stateStartTime = millis(); // Inicializa o tempo de estado
}

void loop() {
  currentTime = millis();
  int sensorValue = digitalRead(SENSOR_PIN);

  // Detecta borda descendente (transição de HIGH para LOW)
  if (sensorValue == LOW && lastSensorState == HIGH) {
    lastLowTime = currentTime;   // Atualiza o tempo da última detecção de LOW
    lastEdgeTime = currentTime; // Atualiza o tempo da última borda
    if (!inCadence) {           // Incrementa o contador somente fora de cadência
      edgeCount++;
      Serial.print("Borda detectada (HIGH -> LOW). Contagem: ");
      Serial.println(edgeCount);
    }

    if (!inCadence && edgeCount >= 5) {
      // Máquina voltou à cadência após 5 bordas descendentes
      nonCadenceTotalTime += (currentTime - stateStartTime); // Atualiza tempo fora de cadência
      stateStartTime = currentTime; // Reinicia o tempo de estado
      Serial.println("Máquina voltou à cadência.");
      inCadence = true; // Atualiza o estado
      edgeCount = 0;    // Reseta o contador de bordas
    }
  }

  // Atualiza o estado anterior do sensor
  lastSensorState = sensorValue;

  // Verifica se mais de 10 segundos se passaram desde a última alternância
  if (!inCadence && (currentTime - lastEdgeTime) >= 10000 && edgeCount > 0) {
    // Reseta o contador se não houver alternâncias por 10 segundos
    Serial.println("Sem alternâncias por 10 segundos. Contador resetado.");
    edgeCount = 0;
  }

  // Verifica se mais de 10 segundos se passaram desde o último LOW para sair de cadência
  if ((currentTime - lastLowTime) >= 10000 && inCadence) {
    // Máquina saiu de cadência
    cadenceTotalTime += (currentTime - stateStartTime); // Atualiza tempo em cadência
    stateStartTime = currentTime; // Reinicia o tempo de estado
    Serial.println("A máquina saiu de cadência!");
    inCadence = false; // Atualiza o estado
    edgeCount = 0; // Reseta o contador de bordas
  }

  // Exibe o tempo acumulado e atual no estado (a cada 1 segundo)
  if (currentTime - lastPrintTime >= 1000) {
    lastPrintTime = currentTime;

    if (inCadence) {
      unsigned long currentCadenceTime = currentTime - stateStartTime;
      Serial.print("Tempo atual em cadência: ");
      printTime(currentCadenceTime / 1000); // Exibe o tempo no formato MM:SS
    } else {
      unsigned long currentNonCadenceTime = currentTime - stateStartTime;
      Serial.print("Tempo atual fora de cadência: ");
      printTime(currentNonCadenceTime / 1000); // Exibe o tempo no formato MM:SS
    }

    // Exibe os tempos acumulados
   // Serial.print("Tempo total em cadência: ");
   // printTime(cadenceTotalTime / 1000); // Exibe o tempo no formato MM:SS

   // Serial.print("Tempo total fora de cadência: ");
   // printTime(nonCadenceTotalTime / 1000); // Exibe o tempo no formato MM:SS

    Serial.println("--------------------------");
  }
}

// Função para exibir o tempo no formato MM:SS
void printTime(unsigned long totalSeconds) {
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");
  if (seconds < 10) Serial.print("0");
  Serial.println(seconds);
}
