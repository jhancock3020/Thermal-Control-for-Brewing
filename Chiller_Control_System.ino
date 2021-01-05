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
  
  J.Hancock 10/12/2020
  Developed from DroneBot Workshop 2019
  https://dronebotworkshop.com
*/
 
// Include Wire Library for I2C
#include <Wire.h>

#include <pt.h>
// Include libraries for DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>

// Include the Keypad library
#include <Keypad.h>
 
// Include Adafruit Graphics & OLED libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 11 // Defines PWM port #

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

static struct pt pt1;

static struct pt pt2;

// Addresses of 5 DS18B20s
uint8_t sensor1[8] = { 0x28, 0xA9, 0xA9, 0x56, 0xB5, 0x01, 0x3C, 0x95 };
uint8_t sensor2[8] = { 0x28, 0x39, 0x00, 0x56, 0xB5, 0xFF, 0x3C, 0xA1 };
uint8_t sensor3[8] = { 0x28, 0xB7, 0x21, 0x56, 0xB5, 0x01, 0x3C, 0x28 };
uint8_t sensor4[8] = { 0x28, 0x7A, 0x6B, 0x56, 0xB5, 0x01, 0x3C, 0xA9 };
uint8_t sensor5[8] = { 0x28, 0xD3, 0x89, 0x56, 0xB5, 0x01, 0x3C, 0x3A };

float temp1, temp2, temp3;

// middle of the range where we want the temp to be
char setPoint[] = "";
 // Reset pin not used but needed for library
 #define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Constants for row and column sizes
const byte ROWS = 4;
const byte COLS = 4;

// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'.', '0', '#', 'D'}
};
static int currSen = 0;
// Connections to Arduino
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

static char customKey;
static int sensorNum;
static float currTemp;

static float tempArray[3];
static int tempHot[3];
static int setPointArray[3];

static int hotNum;
// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
 
void setup() {

  Serial.begin(9600); // Sets up serial monitor output
  sensors.begin();
  
  // Start Wire library for I2C
  Wire.begin();
  
  // initialize OLED with I2C addr 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  pinMode(13,OUTPUT);//Sensor 1 Cooling output
  pinMode(12,OUTPUT);//Sensor 2 Cooling output   
  pinMode(10,OUTPUT);//Sensor 3 Cooling output
 
  digitalWrite(4,LOW);

  PT_INIT(&pt1);
  PT_INIT(&pt2);

  tempHot[0] = 10;
  tempHot[1] = 12;
  tempHot[2] = 13;

  setPointArray[0] = 65;
  setPointArray[1] = 65;
  setPointArray[2] = 65;

}
void ptTrackSensorTemp(struct pt *pt){ 
  PT_BEGIN(pt);
  for(int i = 0; i<3; i++){
    
    //If temp1 val above setPoint + 2, mark as too hot
    if (tempArray[i] > setPointArray[i] + 2)
    {
      digitalWrite(tempHot[i],HIGH);
    }//Otherwise, mark it as okay
    else{
      digitalWrite(tempHot[i],LOW);
    }  
  }
  PT_END(pt);
}
void ptDisplay(struct pt *pt){
  PT_BEGIN(pt);
  if(customKey == '#'){
    if(currSen < 2){
      currSen += 1;    
    }
    else{
      currSen = 0;
    }
  }
  else if(isdigit(customKey)){
    //strncat(setPoint, &customKey,1);
    setPoint[strlen(setPoint)] = customKey;
    setPoint[strlen(setPoint)+1] = '\0';
    Serial.println(setPoint);
  }
//  else if(customKey == '.'){
//    setPointArray[currSen] = (int)setPoint;
//    memset(&setPoint[0], 0,sizeof(setPoint));
    //Serial.println(setPointArray[currSen]);
//  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,00); 
  display.print("Sensor ");
  display.print(currSen);
  display.print(": ");
  display.print(tempArray[currSen]);
  display.print(" F");
  display.setCursor(0,10); 
  display.print("Baseline: ");
  display.print(setPoint);
  display.print(" F");
  display.setCursor(0,20);
  display.display(); 
  PT_END(pt);
}
void loop() {
  
  sensors.requestTemperatures(); 
  temp1 = sensors.getTempF(sensor1);
  temp2 = sensors.getTempF(sensor2);
  temp3 = sensors.getTempF(sensor3);
  
  tempArray[0] = temp1;
  tempArray[1] = temp2;
  tempArray[2] = temp3;

  // Get key value if pressed
  customKey = customKeypad.getKey();
  ptDisplay(&pt1);
  //ptTrackSensorTemp(&pt2);
}
