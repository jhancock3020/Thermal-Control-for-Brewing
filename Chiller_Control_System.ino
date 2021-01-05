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
  
  J.Hancock 11/17/2020
  
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

// Addresses of 5 DS18B20s
uint8_t sensor1[8] = { 0x28, 0xA9, 0xA9, 0x56, 0xB5, 0x01, 0x3C, 0x95 };
uint8_t sensor2[8] = { 0x28, 0x39, 0x00, 0x56, 0xB5, 0xFF, 0x3C, 0xA1 };
uint8_t sensor3[8] = { 0x28, 0xB7, 0x21, 0x56, 0xB5, 0x01, 0x3C, 0x28 };
uint8_t sensor4[8] = { 0x28, 0x7A, 0x6B, 0x56, 0xB5, 0x01, 0x3C, 0xA9 };
uint8_t sensor5[8] = { 0x28, 0xD3, 0x89, 0x56, 0xB5, 0x01, 0x3C, 0x3A };

//temps given by the sensors
float temp1, temp2, temp3;

// middle of the range where we want the temp to be
char setPoint[4];

//Adafruit_SSD1306 display(OLED_RESET);
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

// User input on the keypad
static char customKey;
// Array that holds sensor temps
static float tempArray[3];
// Array that holds "hot" oled address 
static int tempHot[3];
// Array that holds setPoints for each sensor
static int setPointArray[3];

// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void TCA9548A(uint8_t bus)
{
  Wire.beginTransmission(0x70);  // TCA9548A address is 0x70
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
}
void setup() {

  Serial.begin(9600); // Sets up serial monitor output
  sensors.begin();
  
  // Start Wire library for I2C
  Wire.begin();
  
  // Set multiplexer to channel 0 and initialize OLED-1 with I2C addr 0x3C
  TCA9548A(0);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 
  // Set multiplexer to channel 1 and initialize OLED-2 with I2C addr 0x3C
  TCA9548A(1);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  // Set multiplexer to channel 2 and initialize OLED-3 with I2C addr 0x3C
  TCA9548A(2);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Set multiplexer to channel 7 and initialize OLED-4 with I2C addr 0x3C
  TCA9548A(7);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  pinMode(13,OUTPUT);//Sensor 1 Cooling output
  pinMode(12,OUTPUT);//Sensor 2 Cooling output   
  pinMode(10,OUTPUT);//Sensor 3 Cooling output
 
  digitalWrite(4,LOW);

  // Fills in TempHot array with addresses
  tempHot[0] = 10;
  tempHot[1] = 12;
  tempHot[2] = 13;
  // Fills in SetPointArray with starting setPoints
  setPointArray[0] = 40.00;
  setPointArray[1] = 40.00;
  setPointArray[2] = 40.00;

  sprintf(setPoint,"%d",setPointArray[currSen]);
}

void ptTrackSensorTemp(){ 
  for(int i = 0; i<3; i++){
    //If temp1 val above setPoint + 2, mark as too hot
    if (tempArray[i] > setPointArray[i] + 2)
    {
      digitalWrite(tempHot[i],0);//When using PN2222A 0 Turns on FET
    }//If temp val is below setPoimt, unmark it
    else if (tempArray[i] < setPointArray[i]){
      digitalWrite(tempHot[i],1);//When using PN2222A 1 Turns off FET
    }  
  }
}

void ptDisplay(int curAdd){
// write to screen
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,00); 
  display.print("Sensor ");
  display.print(curAdd);
  display.print(": ");
  display.print(tempArray[curAdd]);
  display.print(" F");
  display.setCursor(0,10); 
  display.print("Baseline: ");
  if(curAdd == currSen){
    display.print(setPoint);
    display.print(" F");
    if(customKey == '.'){
      display.print(" Cleared");
    }
    else if(customKey == 'D'){
      display.print(" Updated");
    }
    display.setCursor(0,20);
    display.print("CURR");
  }
  else{
    display.print(setPointArray[curAdd]);
    display.print(" F");
  }
  display.display(); 
}
void userInput(){
  // if customKey is '#', add one to currSen,
  // thus changing the current sensor being looked at
  // if currSen is greater than amount of sensors, go back to first sensor
  if(customKey == '#'){
    if(currSen < 3){
      currSen += 1; 
    }
    else if(currSen == 3){
      currSen = 7;
    }
    else if(currSen == 7){
      currSen = 0;
    }
    sprintf(setPoint,"%d",setPointArray[currSen]);
  }// If user input is digit, add it to setPoint string
  else if(isdigit(customKey)){
    strncat(setPoint, &customKey,1);
    Serial.println(setPointArray[currSen]);
  }// If user input is '.'
  else if(customKey == '.'){
    // set setPoint string back to being empty
    memset(&setPoint[0], 0,sizeof(setPoint));
  }// if user input is 'D', display current setpointArray element
  else if (customKey == 'D'){
    // Convert string setPoint to an int value
    // and set current SetPointArray element
    // as that value
    static int i;
    i = atoi(setPoint);
    setPointArray[currSen] = i;
    sprintf(setPoint,"%d",setPointArray[currSen]);
  }
}
void loop() {
  // Get temps from sensors and set temp values to them
  sensors.requestTemperatures(); 
  temp1 = sensors.getTempF(sensor1);
  temp2 = sensors.getTempF(sensor2);
  temp3 = sensors.getTempF(sensor3);
  // set tempArray elements to temp values
  tempArray[0] = temp1;
  tempArray[1] = temp2;
  tempArray[2] = temp3;

  // Get key value if pressed
  customKey = customKeypad.getKey();
 // Set multiplexer to channel 0 and display "Display 1"
  TCA9548A(0);
  ptDisplay(0);
  display.display();
  // Set multiplexer to channel 1 and display "Display 2"
  TCA9548A(1);
  ptDisplay(1);
  display.display();
  // Set multiplexer to channel 2 and display "Display 2"
  TCA9548A(2);
  ptDisplay(2);
  display.display();
  // Set multiplexer to channel 7 and display "Display 2"
  TCA9548A(7);
  ptDisplay(7);
  display.display();
  userInput();
  ptTrackSensorTemp();
}
