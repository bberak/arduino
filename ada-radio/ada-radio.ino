/*************************************************** 
  This is an example for the Si4713 FM Radio Transmitter with RDS

  Designed specifically to work with the Si4713 breakout in the
  adafruit shop
  ----> https://www.adafruit.com/products/1958

  These transmitters use I2C to communicate, plus reset pin. 
  3 pins are required to interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution

  Many thx to https://github.com/phrm/fmtx/blob/master/firmware/firmware.ino !

 ****************************************************/

#include <Wire.h>
#include <Adafruit_Si4713.h>

#define _BV(n) (1 << n)
#define RESETPIN 12
#define FMSTATION 10360

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit Radio - Si4713");

  //-- Toggling reset pin
  pinMode(RESETPIN, OUTPUT);
  digitalWrite(RESETPIN, !digitalRead(RESETPIN));
  delay(500);

  if (!radio.begin()) {  //-- Begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    while (1);
  }

  //-- Uncomment to scan power of entire range from 87.5 to 108.0 MHz
  /*
  for (uint16_t f  = 8750; f<10800; f+=10) {
   radio.readTuneMeasure(f);
   Serial.print("Measuring "); Serial.print(f); Serial.print("...");
   radio.readTuneStatus();
   Serial.println(radio.currNoiseLevel);
  }
  */

  Serial.print("\nSet TX power");
  radio.setTXpower(115);  //-- dBuV, 88-115 max

  Serial.print("\nTuning into "); 
  Serial.print(FMSTATION/100); 
  Serial.print('.'); 
  Serial.println(FMSTATION % 100);
  radio.tuneFM(FMSTATION);

  //-- This will tell you the status in case you want to read it from the chip
  radio.readTuneStatus();
  Serial.print("\tCurr freq: "); 
  Serial.println(radio.currFreq);
  Serial.print("\tCurr freqdBuV:"); 
  Serial.println(radio.currdBuV);
  Serial.print("\tCurr ANTcap:"); 
  Serial.println(radio.currAntCap);

  //-- Begin the RDS/RDBS transmission
  radio.beginRDS();
  radio.setRDSstation("AdaRadio");
  radio.setRDSbuffer("Adafruit Radio!");

  Serial.println("RDS on!");  

  //-- Set GP1 and GP2 to output
  radio.setGPIOctrl(_BV(1) | _BV(2));  
}

void loop() {
  radio.readASQ();
  
  //Serial.print("\tCurr ASQ: 0x"); 
  //Serial.println(radio.currASQ, HEX);
  //Serial.print("\tCurr InLevel:"); 
  //Serial.println(radio.currInLevel);
  
  //-- Toggle GPO1 and GPO2
  radio.setGPIO(_BV(1));
  delay(500);
  radio.setGPIO(_BV(2));
  delay(500);
}
