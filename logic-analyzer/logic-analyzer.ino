const char ADDR[] = { 28, 26, 24, 22, 30, 32, 34, 36, 38, 40, 42, 44, 46,  48, 50, 52 }; // A15 -> A0
const char DATA[] = { 39, 41, 43, 45, 47, 49, 51, 53 }; // D7 -> D0
#define CLOCK 2
#define READ_WRITE 3

void setup() {
  // Prep address pints for reading
  for (int n = 0; n < 16; n++) {
    pinMode(ADDR[n], INPUT);
  }

  // Prep data pins for reading
  for (int n = 0; n < 8; n++) {
    pinMode(DATA[n], INPUT);
  }

  // Prep r/w pin for reading
  pinMode(READ_WRITE, INPUT);
  
  // Attach interrupt to clock signal (from clock generator or whatever)
  pinMode(CLOCK, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);

  Serial.begin(9600);
}

void onClock() {
  // Read address pins A15 -> A0
  unsigned int address = 0;
  for (int n = 0; n < 16; n++) {
    int bit = digitalRead(ADDR[n]) ? 1 : 0;
    address = (address << 1) + bit;
  }

  // Read data pins D7 -> D0
  unsigned int data = 0;
  for (int n = 0; n < 8; n++) {
    int bit = digitalRead(DATA[n]) ? 1 : 0;
    data = (data << 1) + bit;
  }

  // Read r/w flag
  char rw = digitalRead(READ_WRITE) ? 'r' : 'W';

  // Print address, data and flags to serial monitor
  char output[15];
  sprintf(output, "%04x  %c  %02x", address, rw, data);
  Serial.println(output);
}

void loop() {}
