 /*******************************************************************************
*
*    Projeto: Hello Word! - COMUNICAÇÃO I2C
*    Data: 30/09/2020
*            http://squids.com.br/arduino
*
*******************************************************************************/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const byte SCREEN_WIDTH = 128; // OLED display width, in pixels
const byte SCREEN_HEIGHT = 32; // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
const int OLED_RESET = -1; // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {

// initialize with the I2C addr 0x3C
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  

// Clear the buffer.
display.clearDisplay();
  
// Display Text "Hello Word"
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,10);
display.println("KKKKKKKKKKKKKKKKK");
display.display();
}

void loop() {}
