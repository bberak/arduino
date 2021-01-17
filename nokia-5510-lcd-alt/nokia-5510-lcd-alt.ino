#define PIN_SCE   4
#define PIN_DC    5
#define PIN_SDIN  6
#define PIN_SCLK  7
#define LCD_C     LOW
#define LCD_D     HIGH

void LcdWrite(byte dc, byte data)
{ 
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}

void setup(void)
{ 
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  
  LcdWrite(LCD_C, 0x21 );  // LCD Extended Commands.
  LcdWrite(LCD_C, 0xB5 );  // Set LCD Vop (Contrast).
  LcdWrite(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_C, 0x20 );
  LcdWrite(LCD_C, 0x0C );

  LcdWrite(LCD_D, 0xFF);
  LcdWrite(LCD_D, 0xF0);
  LcdWrite(LCD_D, 0x0F);
}

void loop(void) { }
