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
#define RESETPIN 7
#define ENCODER_A 2
#define ENCODER_B 3
#define readA bitRead(PIND, ENCODER_A) //--faster than digitalRead()
#define readB bitRead(PIND, ENCODER_B) //-- faster than digitalRead()
#define MAX7219DIN 4
#define MAX7219CS 5
#define MAX7219CLK 6

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

int currentFmStation = 0;
volatile int targetFmStation = 10360;

void setup() {
  Serial.begin(9600);
  Serial.println("DIY Radio");

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
  radio.setRDSstation("DIYradio");
  radio.setRDSbuffer("Non-stop tunes, day and night!"); 

  //-- Set GP1 and GP2 to output
  radio.setGPIOctrl(_BV(1) | _BV(2));

  //-- Listen to rotary encoder
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), isrA, CHANGE);
}

void isrA() {
  if (readA != readB)
    targetFmStation -= 10;
  else
    targetFmStation += 10;

    displayNumber(targetFmStation);
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
  digitalWrite(MAX7219CS,HIGH); // CS off
  digitalWrite(MAX7219CLK,LOW); // CLK low
  sendDataToDisplay(15,0); // test mode off
  sendDataToDisplay(12,1); // display on
  sendDataToDisplay(9,255); // decode all digits
  sendDataToDisplay(11,7); // scan all
  //-- One-by-one, send zero to all the digits
  for(int i=1;i<9;i++){
    sendDataToDisplay(i,0); // blank all
  }
}

void setDisplayBrightness(byte b){  //0-15 is range high nybble is ignored
  sendDataToDisplay(10,b);        //intensity  
}

void sendDataToDisplay(byte reg, byte data){
  digitalWrite(MAX7219CS,LOW);   //CS on (chip select?)
  
  for(int i=128;i>0;i=i>>1){
    //-- Shifts bits by one each time through the loop
    //-- Eg: 128 -> 64 -> 32 -> 16 -> 8 -> 4 -> 2 -> 1 -> 0
    
    if(i&reg){
      //-- The reg variable holds the address of the
      //-- digit or register you want to send data to.
      //-- Digits range from 0001 to 1000

      //-- For example, if I you want to send data to the fifth digit, the
      //-- reg variable will contain 101. 

      //-- So performing bitwise operations on i and reg starting from i = 128 will look like:

      //-- 128 & 5
      //-- i:   10000000 & (128)
      //-- reg: 00000101 (5)
      //-- res: 00000000 (falsey)

      //-- ...

      //-- 4 & 5
      //-- i:   00000100 & (4)
      //-- reg: 00000101 (5)
      //-- res: 00000100 (truthy)

      //-- 2 & 5
      //-- i:   00000010 & (2)
      //-- reg: 00000101 (5)
      //-- res: 00000000 (falsey)

      //-- 1 & 5
      //-- i:   00000001 & (1)
      //-- reg: 00000101 (5)
      //-- res: 00000001 (truthy)
    
      //-- A truthy result means a 1 is sent down the data line.
      //-- A falsey result means a 0 is sent down the data line
      
      digitalWrite(MAX7219DIN,HIGH);
    } else {
      digitalWrite(MAX7219DIN,LOW);      
    }

    //-- The clock is toggled after each bit is sent
    //-- to signal the slave to read the data line
    digitalWrite(MAX7219CLK,HIGH);   
    digitalWrite(MAX7219CLK,LOW); // CLK toggle  
  }

  //-- After the above, the dataline would have received 8 bits: 0-0-0-0-0-1-0-1
  //-- Why are we doing this? Why can't we just send the bye directly?
  //-- Because SPI is a serial communication protocol and we need to send 1 bit at a time
  //-- down the line. Hence the loop..
  
  for(int i=128;i>0;i=i>>1){
    //-- The process in this loop is the same, except will
    //-- now compare it with the data that we want to send.
    //-- Again, this is necessary because we want to send a bit
    //-- at a time from the byte, and the loop with the bitwise
    //-- & operator helps us achieve this.
    
    if(i&data){
      digitalWrite(MAX7219DIN,HIGH);
    }else{
      digitalWrite(MAX7219DIN,LOW);      
    }

    //-- Again, toggle the clock to tell
    //-- the salve to read the data line
    digitalWrite(MAX7219CLK,HIGH);   
    digitalWrite(MAX7219CLK,LOW); // CLK toggle    
  }

  //-- We have sent the digit address and data down the line
  //-- [digit or address]-[data or number]
  //-- [0-0-0-0-0-1-0-1]-[1-0-0-0-0-0-0-0] 
  //-- Note: The above is not a physical representation
  //-- as the actual endianess of the bit may differ..

  //-- Tell the slave that the packet is complete.
  digitalWrite(MAX7219CS,HIGH); // CS off
}

void displayNumber(unsigned long n){
  //-- This function receives a number, say 10360
  //-- and has to display it on an 8 digit strip,
  //-- justified to the right.
  
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

  //-- k = 10360
  //-- blank = 0
  //-- blank is falsey
  //-- Send: 1, 10360 % 10 (0)
  //-- k = 10360/10 (1036)
  //-- blank is falsey
  //-- Send: 2, 1036 % 10 (6)
  //-- k = 1036/10 (103)
  //-- blank is falsey
  //-- Send: 3, 103 % 10 (3)
  //-- k = 103/10 (10)
  //-- blank is falsey
  //-- Send: 4, 10 % 10 (0)
  //-- k = 10/10 (1)
  //-- blank is falsey
  //-- Send: 5, 1 % 10 (1)
  //-- k = 1/10 (0)
  //-- blank = 1
  //-- blank is truthy
  //-- Send: 6, 15 (blank)
  //-- Send: 7, 15 (blank)
  //-- Send: 8, 15 (blank)

  //-- Timewise this sends the following 
  //-- data to display (rightmost being earliest):
  //-- blank-blank-blank-1-0-3-6-0
  //-- However, order of instructions is not necessarily
  //-- important here because the sendDataToDisplay function
  //-- takes care of serializing the register/address and data
  //-- into a continuous stream of bits for each call with the correct
  //-- endianess. Indeed, you can set the number to be displayed 
  //-- on the digits in any order you like.
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
