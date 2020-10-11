bool signal = 0;

void setup()
{
  pinMode(9, OUTPUT);
  pinMode(A6, INPUT);
}

void loop()
{
  //-- Toggle signal
  signal = !signal;

  //-- Read analog input, and round into 20 bands.
  //-- Then multiply band by 50 to give a period between
  //-- 0ms and 1000ms (effectively this generates a max
  //-- clock cycle of 20hz).
  int raw = analogRead(A6);
  int period = map(raw, 0, 1023, 1, 20) * 50;
  
  //-- Only output high if the signal is on and the period is less than 1000.
  //-- A period of 1000ms indicates the slowest speed setting (potentiometer is
  //-- wound all the way anti-clockwise) and it's at this point that the user
  //-- can perform manualy clock stepping using the physical switch. 
  digitalWrite(9, signal && period < 1000 ? 1 : 0);

  //-- Sleep based on the selected period (in ms)
  delay(period);
}
