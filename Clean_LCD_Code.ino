#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "Timer.h"

#define NOFIELD 505L    // Analog output with no applied field, calibrate this

//This code calculates the number of seconds between two passings of the magnet
// Uncomment one of the lines below according to device in use A1301 or A1302
// This is used to convert the analog voltage reading to milliGauss
// For A1301: 2.5mV = 1Gauss, and 1024 analog steps = 5V, so 1 step = 1953mG
// For A1302: 1.3mV = 1Gauss, and 1024 analog steps = 5V, so 1 step = 3756mG

#define TOMILLIGAUSS 1.953125
// #define TOMILLIGAUSS 3.756010  

volatile int count = 0; // Number of rotations registered
unsigned long oldtime;
int hpin = 2; // Output pin of hall effect sensor
double RPM = 0.0; // RPM value
int updatedelay = 4000; // How often the RPM value updates

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3); // Initializing LCD screen object

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

void updateRPM() {
  if(millis() - oldtime > updatedelay) {
    detachInterrupt(digitalPinToInterrupt(hpin));
    RPM = count*(60/(updatedelay/1000));
    oldtime = millis();
    count = 0;
    attachInterrupt(digitalPinToInterrupt(hpin), countIncrease, FALLING);
  }
}

void initializeScreen() {
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(0,0);
}

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

void setup()   {
  pinMode(hpin, INPUT_PULLUP); // Setting pullup resistor for pin 2
  attachInterrupt(digitalPinToInterrupt(hpin), countIncrease, FALLING);
  Serial.begin(9600);
  display.begin();
  // init done
  display.setContrast(50); // Can change the contrast around to adapt the display for the best viewing!
  display.display(); // show splashscreen
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer
  initializeScreen();
  oldtime = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  updateRPM();
  // text display tests
  display.clearDisplay();
  display.print("count= ");
  display.println(count);
  display.print("RPM= ");
  display.println(RPM);
  //display.setTextColor(WHITE, BLACK); // 'inverted' text
  display.println("Battery Fully Charged");
  display.display();
}

void countIncrease() {
  count++;
}
