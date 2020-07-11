#define encoderA 2
#define encoderB 3

volatile int encoderValue = 0;

void setup() {
  Serial.begin(9600);
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderA), encoder, FALLING);
}

void loop() {
  Serial.println(encoderValue);
  delay(100);
}

void encoder() {
  if (digitalRead(encoderA) == digitalRead(encoderB))
    encoderValue++;
  else
    encoderValue--;
}
