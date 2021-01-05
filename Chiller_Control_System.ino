/*
  OLED Display and DS18B20 Tempature Sensor
  oled-temp-meter-by-index.ino
  Displays results on 128 x 32 OLED display
  Uses DS18B20 Temperature sensor
  Uses Wire Library
  Uses Adafruit SSD1306 OLED Library
  Uses Adafruit GFX Graphics Library
  Uses OneWire Library
  Uses DallasTemperature Library
  
  J.Hancock 9/21/2020
  Developed from DroneBot Workshop 2019
  https://dronebotworkshop.com
*/
 
// Include Wire Library for I2C
#include <Wire.h>

// Include libraries for DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
 
// Include Adafruit Graphics & OLED libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 11 // Defines PWM port #

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

float temp1, temp2, temp3, temp4, temp5;
 
 // Reset pin not used but needed for library
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
 
void setup() {

Serial.begin(9600); // Sets up serial monitor output
  sensors.begin();
  
  // Start Wire library for I2C
  Wire.begin();
  
  // initialize OLED with I2C addr 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}
 
void displayTemp(){
  // Delay to allow sensor to stabalize
  delay(2000);

  // Clear the display
  display.clearDisplay();
  //Set the color - always use white despite actual display color
  display.setTextColor(WHITE);
  //Set the font size
  display.setTextSize(1);
  //Set the cursor coordinates
  display.setCursor(0,00); 
  display.print("Sensor 1: ");
  display.print(temp1); 
  display.print(" F");
    
  display.setCursor(0,10);
  display.print("Sensor 2: "); 
  display.print(temp2); 
  display.print(" F");

  display.setCursor(0,20);
  display.print("Sensor 3: "); 
  display.print(temp3); 
  display.print(" F");
}

void displayTemp2(){
  delay (2000);

  display.clearDisplay();

  display.setCursor(0,00); 
  display.print("Sensor 4: "); 
  display.print(temp4); 
  display.print(" F");
    
  display.setCursor(0,10);
  display.print("Sensor 5: "); 
  display.print(temp5); 
  display.print(" F");
}

void loop() {
  sensors.requestTemperatures(); 

  temp1 = sensors.getTempFByIndex(0);
  temp2 = sensors.getTempFByIndex(1);
  temp3 = sensors.getTempFByIndex(2);
  temp4 = sensors.getTempFByIndex(3);
  temp5 = sensors.getTempFByIndex(4);
    
  Serial.print(temp1); 
  Serial.print(" F  ");
  Serial.print(temp2);
  Serial.print(" F  ");
  Serial.print(temp3);
  Serial.print(" F  ");
  Serial.print(temp4);
  Serial.print(" F  ");
  Serial.print(temp5);
  Serial.println(" F  ");
  
  delay(1000);
  
  displayTemp();
  display.display();
  displayTemp2();
  display.display();
}
