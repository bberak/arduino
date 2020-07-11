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
#define ENCODER_A 2
#define ENCODER_B 3
#define MAX7219DIN 4
#define MAX7219CS 5
#define MAX7219CLK 6

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

int currentFmStation = 0;
volatile int targetFmStation = 10360;
unsigned long n=9999;

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit Radio - Si4713");

  //-- Init LED digit screen
  initDisplay();
  setDisplayBrightness(1);

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

  radio.setTXpower(115);  //-- dBuV, 88-115 max

  tune(targetFmStation);

  //-- Begin the RDS/RDBS transmission
  radio.beginRDS();
  radio.setRDSstation("AdaRadio");
  radio.setRDSbuffer("Plug me in!");
  Serial.println("RDS on!");  

  //-- Set GP1 and GP2 to output
  radio.setGPIOctrl(_BV(1) | _BV(2));

  //-- Listen to rotary encoder
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoder, FALLING);
}

void encoder() {
  if (digitalRead(ENCODER_A) == digitalRead(ENCODER_B))
    targetFmStation -= 10;
  else
    targetFmStation += 10;
}

void tune(int freq) {
  if (currentFmStation != freq) {
    currentFmStation = freq;
    Serial.print("\nTuning into "); 
    Serial.print(currentFmStation/100); 
    Serial.print('.'); 
    Serial.println(currentFmStation % 100);
    radio.tuneFM(currentFmStation);
    displayNumber(currentFmStation);
  }
}

void initDisplay(){
  pinMode(MAX7219DIN,OUTPUT);
  pinMode(MAX7219CS,OUTPUT);
  pinMode(MAX7219CLK,OUTPUT);
  digitalWrite(MAX7219CS,HIGH);   //CS off
  digitalWrite(MAX7219CLK,LOW);   //CLK low
  sendDataToDisplay(15,0);        //test mode off
  sendDataToDisplay(12,1);        //display on
  sendDataToDisplay(9,255);       //decode all digits
  sendDataToDisplay(11,7);        //scan all
  for(int i=1;i<9;i++){
    sendDataToDisplay(i,0);       //blank all
  }
}

void setDisplayBrightness(byte b){  //0-15 is range high nybble is ignored
  sendDataToDisplay(10,b);        //intensity  
}

void sendDataToDisplay(byte reg, byte data){
  digitalWrite(MAX7219CS,LOW);   //CS on
  for(int i=128;i>0;i=i>>1){
    if(i&reg){
      digitalWrite(MAX7219DIN,HIGH);
    }else{
      digitalWrite(MAX7219DIN,LOW);      
    }
  digitalWrite(MAX7219CLK,HIGH);   
  digitalWrite(MAX7219CLK,LOW);   //CLK toggle    
  }
  for(int i=128;i>0;i=i>>1){
    if(i&data){
      digitalWrite(MAX7219DIN,HIGH);
    }else{
      digitalWrite(MAX7219DIN,LOW);      
    }
  digitalWrite(MAX7219CLK,HIGH);   
  digitalWrite(MAX7219CLK,LOW);   //CLK toggle    
  }
  digitalWrite(MAX7219CS,HIGH);   //CS off
}

void displayNumber(unsigned long n){
  unsigned long k=n;
  byte blank=0;
  for(int i=1;i<9;i++){
    if(blank){
      sendDataToDisplay(i,15);        
    }else{
      sendDataToDisplay(i,k%10);
    }
    k=k/10;
    if(k==0){blank=1;}
  }
}

void loop() {
  tune(targetFmStation);
  
  radio.readASQ();
    
  //-- Toggle GPO1 and GPO2
  radio.setGPIO(_BV(1));
  delay(500);
  radio.setGPIO(_BV(2));
  delay(500);
}
