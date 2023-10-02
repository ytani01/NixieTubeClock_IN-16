#include "Wire.h"
#include "PCF8563.h"
#define PIN_I2C_SDA 9
#define PIN_I2C_SCL 8

PCF8563 rtc;
TwoWire wire = TwoWire(0);

void setup(){
    Serial.begin(115200);
    while(!Serial);
    // delay(3000);
   
    Serial.println("Start");
    wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    rtc.begin(&wire);
}

void loop(){
  DateTime d;
  rtc.getTime(&d);
 
  Serial.print(d.year());
  Serial.print(" ");
  Serial.print(d.month());
  Serial.print(" ");
  Serial.print(d.day());
  Serial.print(" ");
  Serial.print(d.hour());
  Serial.print(" ");
  Serial.print(d.minute());
  Serial.print(" ");
  Serial.print(d.second());
  Serial.println("");

  delay(1000);
}