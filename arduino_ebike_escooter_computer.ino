#include <Wire.h>
#include <hd44780.h> // 16x2 i2C display lib 
#include <hd44780ioClass/hd44780_I2Cexp.h> // include i/o class header
#include <OneWire.h> // OneWire lib to use 18b20 temp sensor

OneWire ds(2); // declare digital pin #2 to use as OneWire
hd44780_I2Cexp lcd; // declare lcd object: auto locate & config display for hd44780 chip


// A lot of code on 74HC5067 multiplxer was taken from  https://www.instructables.com/id/Tutorial-74HC4067-16-Channel-Analog-Multiplexer-De/
// Including comments. Why write your own comments if someone has already done a prettey good explanatory job
// Written by https://www.instructables.com/member/pmdwayhk/
// A lot of temperature sensor code was taken from https://arduinomaster.ru/datchiki-arduino/arduino-ds18b20/ some varibles renamed, comments translated RU -> EN
// connect 74HC4067 S0~S3 to Arduino D7~D4 respectively
// connect 74HC4067 pin 1 to Arduino A0
byte controlPins[] = {B00000000, 
                  B10000000,
                  B01000000,
                  B11000000,
                  B00100000,
                  B10100000,
                  B01100000,
                  B11100000,
                  B00010000,
                  B10010000,
                  B01010000,
                  B11010000,
                  B00110000,
                  B10110000,
                  B01110000,
                  B11110000 }; 

// Holds incoming values from 74HC4067                  
byte muxValues[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
String voltageStr;
String pinVoltage;
int speakerOut = 3;
float tempThreshold = 29.0;  
float voltageThreshold = 0.0;

// Function to be used to display voltage on the LCD in the correct way
String convertValue(byte byteValue){
  String pinVoltage =  String(byteValue * (5.0 /1023.0));   // Convert digit to analog value from ADC
  pinVoltage.remove(3); // Remove unused digits after point so that "3.75" becomes like "3.7"
  return pinVoltage;
}

// Function to be used to display temperature on the LCD in the correct way
String floatToString(float floatValue){
  String output = String(floatValue); // Convert float variable containing temperature from sensor to String
  output.remove(2); // Remove unnesessary digits
  return output;
}


// Prepare
void setup()
{
  Serial.begin(9600); // Initialise serial connection to PC via USB
  DDRD = B11111111; // set PORTD (digital 7~0) to outputs
  lcd.begin(16, 2); // Initialise LCD, tell Arduino its a 16 by 2 lcd. Replace numbers if you have a different screen
  pinMode(speakerOut,3); // set pin 3 to be used as PC buzzer output (connect though 100 Ohm resistor)
}

// Function to select pin on 74HC4067
void setPin(int outputPin)
{
  PORTD = controlPins[outputPin];
}

// Dumps captured data from array to serial monitor
void displayData()
{
  Serial.println();
  Serial.println("Values from multiplexer:");
  Serial.println("========================");
  for (int i = 0; i < 16; i++)
  {
    Serial.print("input I"); 
    Serial.print(i); 
    Serial.print(" = "); 
    float voltage = muxValues[i] * (5.0 / 1023.0); // Data from ADC comes a digit ranging from 0 to 1024. Convert it to usable voltage this way.
    voltageStr = String(voltage); // Convert it to string to be more usable (like remove unnesessary characters)
    Serial.println(voltage);
    if(voltage < voltageThreshold){
      lowVoltageAlarm(i, voltageStr);
      
    }
  
  }
  
  Serial.println("========================"); 

  // This part is for temperature detection
  byte data[2]; // Temp data goes here
  
  ds.reset(); // Start from resetting all previous stuff
  ds.write(0xCC); // We got only one temp sensor, skip address search command.
  ds.write(0x44); // Ask the temp sensor to measure temperature and save it to internal sensor memory.
  delay(1000); // It takes about 700ms to measure & save data inside the sensor. Lets wait a little more just in case. TODO: this is a really long delay, maybe another sensors would be better off  
  ds.reset(); // Prepare to get saved reading from the sensor
  ds.write(0xCC);  // We got only one temp sensor, skip address search command. 
  ds.write(0xBE); // Ask to give us the reading
  data[0] = ds.read(); // 
  data[1] = ds.read(); //
  float temperature =  ((data[1] << 8) | data[0]) * 0.0625; // Format data: first glue it all together and multiply by 0.0625 (same as stuff with ADC voltage conversion before)
  
  // Send temperature reading to serial
  Serial.print("temp is ");
  Serial.print(temperature);

  // Beep if temperature is more than tempThreshold
  if(temperature > tempThreshold){
    highTempAlarm(temperature);
  }

  // Ok, so things get messy here. Sorry for the spaghetti.
  lcd.setCursor(0, 0); // Start at the beginning of the first line of LCD
  lcd.print(convertValue(muxValues[0])); // Show voltage of the 0 multiplexer pin

  lcd.print(" ");
  lcd.print(convertValue(muxValues[1])); // Show voltage of the 1 multiplexer pin

  lcd.print(" ");
  lcd.print(convertValue(muxValues[2])); // Show voltage of the 2 multiplexer pin

  lcd.print(" ");
  lcd.print(convertValue(muxValues[3])); // Show voltage of the 3 multiplexer pin
 
  lcd.setCursor(0, 1); // Go to the beginning of the second line of LCD

  lcd.print(convertValue(muxValues[4])); // Show voltage of the 4 multiplexer pin

  lcd.print(" ");
  lcd.print(convertValue(muxValues[5])); // Show voltage of the 5 multiplexer pin

  lcd.print(" ");
  lcd.print(convertValue(muxValues[6])); // Show voltage of the 6 multiplexer pin

  // This section shows battery temperature (or any temperature you like) taken from 18B20 sensor
  lcd.print(" t");
  lcd.print(floatToString(temperature)); // convert to string & remove digits after zero. So that "27.0" becomes "27"
  lcd.print("\337C"); // add degree symbol after temperature reading
 
   
}

void loop()
{
  for (int i = 0; i < 16; i++)
  {
    setPin(i); // choose an input pin on the 74HC4067
    muxValues[i]=analogRead(0); // read the vlaue on that pin and store in array
  }

  // display captured data
  displayData();

}
// Do stuff if temp sensor temp is too high. Change to taste.
void highTempAlarm(float currentTemp){
  lcd.setCursor(0, 0);
  lcd.print("> TEMPERATURE! <");
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.print(currentTemp);
  lcd.print("\337C");
  lcd.print("       ");
  for(int i = 0; i<250;i++){
    digitalWrite(speakerOut, HIGH);
    delayMicroseconds(550);
    digitalWrite(speakerOut, LOW);
    delayMicroseconds(500); 
}
  delay(150);
  for(int i = 0; i<250;i++){
    digitalWrite(speakerOut, HIGH);
    delayMicroseconds(550);
    digitalWrite(speakerOut, LOW);
    delayMicroseconds(500); 
}
delay(150);
  for(int i = 0; i<250;i++){
    digitalWrite(speakerOut, HIGH);
    delayMicroseconds(550);
    digitalWrite(speakerOut, LOW);
    delayMicroseconds(500); 
}
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("                ");

}

// Do stuff if one of the cells voltage is too low. Change to taste.
void lowVoltageAlarm(int cellNumber,String cellVoltage){
  lcd.setCursor(0, 0);
  lcd.print("> LOW VOLTAGE! <");
  lcd.setCursor(0, 1);
  lcd.print(" batt ");
  lcd.print(cellNumber);
  lcd.print(" = ");
  lcd.print(cellVoltage);
  lcd.print("v <");
  delay(1000);
  for(int i = 0; i<250;i++){
    digitalWrite(speakerOut, HIGH);
    delayMicroseconds(550);
    digitalWrite(speakerOut, LOW);
    delayMicroseconds(500); 
}
  delay(150);
  for(int i = 0; i<250;i++){
    digitalWrite(speakerOut, HIGH);
    delayMicroseconds(550);
    digitalWrite(speakerOut, LOW);
    delayMicroseconds(500); 
}
  lcd.print("                ");
  
}
