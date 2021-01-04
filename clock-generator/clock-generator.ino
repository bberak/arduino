bool signal = 0;
int maxHz = 100;
int maxPeriod = 1000;
int minPeriod = maxPeriod / maxHz;

void setup()
{
  pinMode(9, OUTPUT);
  pinMode(A6, INPUT);
}

void loop()
{
  //-- Toggle signal
  signal = !signal;

  //-- Read analog input and scale to maxHz
  int raw = analogRead(A6);
  int period = map(raw, 0, 1023, 1, maxHz) * minPeriod;
  
  //-- Only output high if the signal is on and the period is less than maxPeriod.
  //-- A maxPeriod indicates the slowest speed setting (potentiometer is
  //-- wound all the way anti-clockwise) and it's at this point that the user
  //-- can perform manualy clock stepping using the physical switch. 
  digitalWrite(9, signal && period < maxPeriod ? 1 : 0);

  //-- Sleep based on the selected period (in ms)
  delay(period);
}
