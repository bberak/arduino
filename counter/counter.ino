#define MAX7219DIN 4
#define MAX7219CS 5
#define MAX7219CLK 6

unsigned long n=9999;

void setup() {
  MAX7219init();
  MAX7219brightness(15);
}
void loop() {
  MAX7219shownum(n);
  n--;
}
void MAX7219shownum(unsigned long n){
  unsigned long k=n;
  byte blank=0;
  for(int i=1;i<9;i++){
    if(blank){
      MAX7219senddata(i,15);        
    }else{
      MAX7219senddata(i,k%10);
    }
    k=k/10;
    if(k==0){blank=1;}
  }
}
void MAX7219brightness(byte b){  //0-15 is range high nybble is ignored
  MAX7219senddata(10,b);        //intensity  
}
void MAX7219init(){
  pinMode(MAX7219DIN,OUTPUT);
  pinMode(MAX7219CS,OUTPUT);
  pinMode(MAX7219CLK,OUTPUT);
  digitalWrite(MAX7219CS,HIGH);   //CS off
  digitalWrite(MAX7219CLK,LOW);   //CLK low
  MAX7219senddata(15,0);        //test mode off
  MAX7219senddata(12,1);        //display on
  MAX7219senddata(9,255);       //decode all digits
  MAX7219senddata(11,7);        //scan all
  for(int i=1;i<9;i++){
    MAX7219senddata(i,0);       //blank all
  }
}
void MAX7219senddata(byte reg, byte data){
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
