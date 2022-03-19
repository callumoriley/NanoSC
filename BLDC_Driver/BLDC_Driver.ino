/*
 * Step: 1 2 3 4 5 6
 * High: A B B C C A
 * Low:  C C A A B B
 */

#define LOW_A 9
#define LOW_B 8
#define LOW_C 4

#define HIGH_A 5
#define HIGH_B 6
#define HIGH_C 7

// PERIOD in microseconds (1000 us = 1 ms = 1khz signal)
#define PERIOD 1000
// MAX_DURATION in milliseconds
#define MAX_DURATION 100

void setup() {
  // low side pins
  pinMode(LOW_A, OUTPUT); // A phase
  pinMode(LOW_B, OUTPUT); // B phase
  pinMode(LOW_C, OUTPUT); // C phase

  // high side pins
  pinMode(HIGH_A, OUTPUT); // A phase
  pinMode(HIGH_B, OUTPUT); // B phase
  pinMode(HIGH_C, OUTPUT); // C phase

  Serial.begin(9600);
}

void loop() {
  int timeHigh = map(analogRead(A0), 0, 1023, 0, PERIOD); // the time that the MOSFETs will be conducting
  int duration = map(analogRead(A1), 0, 1023, 0, MAX_DURATION); // the duration that the signal is running for (in milliseconds)

  Serial.print(timeHigh);
  Serial.print(' ');
  Serial.println(duration);
  
  setState(HIGH_A, LOW_C, timeHigh, PERIOD, duration); // motor commutation loop
  setState(HIGH_B, LOW_C, timeHigh, PERIOD, duration);
  setState(HIGH_B, LOW_A, timeHigh, PERIOD, duration);
  setState(HIGH_C, LOW_A, timeHigh, PERIOD, duration);
  setState(HIGH_C, LOW_B, timeHigh, PERIOD, duration);
  setState(HIGH_A, LOW_B, timeHigh, PERIOD, duration);
}
void setState(int highPin, int lowPin, int condTime, int period, int duration) { // forgive me, I don't know how to use timer interrupts on an AVR
  unsigned long startTime = millis();
  setPinsToDefault();
  int lowTime = period - condTime;

  while (millis() < (startTime + duration)) {
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
    delayMicroseconds(condTime); // please forgive me
    digitalWrite(highPin, HIGH);
    digitalWrite(lowPin, LOW);
    delayMicroseconds(lowTime); // I promise I know how to use them on the 8051, just not on this chip. I promise I will learn someday
  }

  setPinsToDefault();
}
void setPinsToDefault(void) {
  digitalWrite(LOW_A, LOW);
  digitalWrite(LOW_B, LOW);
  digitalWrite(LOW_C, LOW);

  digitalWrite(HIGH_A, HIGH);
  digitalWrite(HIGH_B, HIGH);
  digitalWrite(HIGH_C, HIGH);
}
