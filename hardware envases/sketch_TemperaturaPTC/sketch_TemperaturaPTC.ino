int sensorPin = A0;
float Rfixo = 1000.0;   // resistor fixo do divisor (ohms)
float R25   = 1000.0;   // resistência nominal do PTC a 25 °C
float alpha = 0.0039;   // coeficiente do PTC (fração por °C)

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Leitura do PTC em graus Celsius...");
}

void loop() {
  int leituraADC = analogRead(sensorPin);
  float tensao = leituraADC * (3.3 / 1023.0);

  // Calcula a resistência do PTC a partir do divisor
  float Rptc = Rfixo * ((tensao / (3.3 - tensao)));

  // Converte para temperatura (°C)
  float temperatura = ((Rptc - R25) / (R25 * alpha)) + 27.0;

  Serial.print("ADC: ");
  Serial.print(leituraADC);
  Serial.print(" | Rptc: ");
  Serial.print(Rptc, 1);
  Serial.print(" ohms | Temp: ");
  Serial.print(temperatura, 1);
  Serial.println(" C");

  delay(1000);
}
