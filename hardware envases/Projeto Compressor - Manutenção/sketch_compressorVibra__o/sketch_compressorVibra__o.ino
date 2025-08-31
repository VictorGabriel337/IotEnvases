// Diagnostico SW-420 no ESP8266 (NodeMCU)
// DO -> D2 (GPIO4) | VCC -> 3V3 | GND -> GND | (opcional AO -> A0)

#if !defined(ESP8266)
  #error "Selecione NodeMCU/ESP8266 na IDE para este sketch."
#endif

const int PIN_DO  = D2;    // GPIO4
const int PIN_AO  = A0;    // se não estiver ligado, vai ler ruído/0
const int PIN_LED = LED_BUILTIN;

const unsigned long SAMPLE_MS = 20;
unsigned long lastSample = 0;

void setup() {
  Serial.begin(115200);
  // Usar pull-up interna ajuda se a saída DO do módulo for open-collector
  pinMode(PIN_DO, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  delay(1000);
  Serial.println("Iniciando diagnostico SW-420...");
}

void loop() {
  int digRaw = digitalRead(PIN_DO);   // 0 ou 1
  int digInv = !digRaw;               // útil se o modulo for ativo-baixo
  int ana    = analogRead(PIN_AO);    // so faz sentido se AO estiver ligado

  // Considerar "vibracao" quando digRaw == LOW (modulos comuns são ativos-baixo)
  bool vibracao = (digRaw == LOW);
  digitalWrite(PIN_LED, vibracao ? HIGH : LOW);

  if (millis() - lastSample >= SAMPLE_MS) {
    lastSample = millis();
    // Envie 3 series para o Serial Plotter: digRaw, digInv, analogico
    Serial.print(digRaw);  Serial.print('\t');
    Serial.print(digInv);  Serial.print('\t');
    Serial.println(ana);
    delay(1000);
  }

  // Log quando o estado mudar
  static int prev = -1;
  if (digRaw != prev) {
    prev = digRaw;
    if (vibracao) Serial.println("[DO] Vibracao detectada (ativo-baixo)");
    else          Serial.println("[DO] Sem vibracao");
  }
}
