//Programa: Sensor de temperatura I2C MLX90614 Arduino
//Autor: Arduino e Cia
#include <Wire.h>
#include <Adafruit_MLX90614.h>
//#include <LiquidCrystal_I2C.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


const byte SCREEN_WIDTH = 128; // OLED display width, in pixels
const byte SCREEN_HEIGHT = 32; // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
const int OLED_RESET = -1; // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



//Define o endereco I2C do display e qtde de colunas e linhas
//LiquidCrystal_I2C lcd(0x3B, 16, 2);
//Array que desenha o simbolo de grau
/*byte grau[8] = {B00110, B01001, B01001, B00110,
                B00000, B00000, B00000, B00000,};  */
double temp_amb;
double temp_obj;
void setup()
{

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.begin(9600);
  Serial.println("Sensor de temperatura MLX90614");

  display.clearDisplay();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  
 /* Serial.begin(9600);
  Serial.println("Sensor de temperatura MLX90614");
  //Inicializa o display LCD I2C
  lcd.init();
  lcd.backlight();
  //Atribui a "1" o valor do array "grau", que desenha o simbolo de grau
  lcd.createChar(1, grau); */
  
  //Inicializa o MLX90614
  mlx.begin();
}
void loop()
{
  //Leitura da temperatura ambiente e do objeto
  //(para leitura dos valores em Fahrenheit, utilize
  //mlx.readAmbientTempF() e mlx.readObjectTempF() )
  temp_amb = mlx.readAmbientTempC();
  temp_obj = mlx.readObjectTempC();
  //Mostra as informacoes no display

   // Verifica se a leitura foi bem-sucedida
    if (isnan(temp_amb)) {
        Serial.println("Erro ao ler o sensor. Verifique as conexões.");
    } else {
        // Exibe no Serial Monitor
        Serial.print("AMBIENTE = ");
        Serial.print(temp_amb);
        Serial.println(" *C");

         Serial.print("OBJETO = ");
        Serial.print(temp_obj);
        Serial.println(" *C");

        // Limpa o display antes de atualizar
        display.clearDisplay();

        // Escreve a variável temp_amb no display
        display.setTextSize(1,2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.print("Ambiente: ");
        display.print(temp_amb); // Mostra o valor da variável temp_amb
        display.println(" C");
 

        display.setTextSize(1,2);
        display.setTextColor(WHITE);
        display.setCursor(0, 18);
        display.print("Garrafa: ");
        display.print(temp_obj); // Mostra o valor da variável temp_amb
        display.println(" C");

        // Atualiza o display com as informações novas
        display.display();
    }
 /* lcd.setCursor(0, 0);
  lcd.print("Ambiente:");
  lcd.setCursor(10, 0);
  lcd.print(temp_amb);
  lcd.setCursor(15, 0);
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.print("Objeto:");
  lcd.setCursor(10, 1);
  lcd.print(temp_obj);
  lcd.setCursor(15, 1);
  lcd.write(1);
  //Mostra as informacoes no Serial Monitor
  Serial.print("Ambiente = ");
  Serial.print(temp_amb);
  Serial.print("*CtObjeto = ");
  Serial.print(temp_obj); Serial.println("*C");
  //Aguarda 1 segundo ate nova leitura */
  delay(1000);
}
