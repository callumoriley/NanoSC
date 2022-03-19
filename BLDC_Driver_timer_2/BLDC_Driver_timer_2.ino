/*
 * Step: 1 2 3 4 5 6
 * High: A B B C C A
 * Low:  C C A A B B
 * 
 * Used this video as a reference for implementing timers: https://www.youtube.com/watch?v=IdL0_ZJ7V2s
 * And referenced this datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
 * 
 */

#define LOW_A 9
#define LOW_B 8
#define LOW_C 4

#define HIGH_A 5
#define HIGH_B 6
#define HIGH_C 7

// PERIOD in microseconds (1000 us = 1 ms = 1khz signal)
#define PERIOD 1000
// In Hz
#define MIN_FREQ 1
#define MAX_FREQ 100

#define F_CLOCK 16000000UL
#define PRSC1 64

const int highPins[] = {HIGH_A, HIGH_B, HIGH_B, HIGH_C, HIGH_C, HIGH_A};
const int lowPins[] = {LOW_C, LOW_C, LOW_A, LOW_A, LOW_B, LOW_B};

int commStep = 0; // 0 to 5 corresponding with the steps in the comment above
int targetFreq = 10; // target commutation frequency
int dutyCycle = 50;

unsigned long startTime;

void setup() {
  // low side pins
  pinMode(LOW_A, OUTPUT); // A phase
  pinMode(LOW_B, OUTPUT); // B phase
  pinMode(LOW_C, OUTPUT); // C phase

  // high side pins
  pinMode(HIGH_A, OUTPUT); // A phase
  pinMode(HIGH_B, OUTPUT); // B phase
  pinMode(HIGH_C, OUTPUT); // C phase

  // Timer 1 configuration
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 65536 - F_CLOCK/(PRSC1*targetFreq);
  TCCR1B |= (1 << CS11) | (1 << CS10); // prescaler 64
  TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
  
  setPinsToDefault();
}
ISR(TIMER1_OVF_vect) { // increments commutation step with a timer interrupt
  if (++commStep > 5) {
    commStep = 0;
  }
  TCNT1 = 65536 - F_CLOCK/(PRSC1*targetFreq);
}
void loop() {
  startTime = micros();
  dutyCycle = map(analogRead(A0), 0, 1023, 0, 100); // the time that the MOSFETs will be conducting
  targetFreq = map(analogRead(A1), 0, 1023, MIN_FREQ, MAX_FREQ); // the frequency that the commutation switches (in Hz)

  commutate();
}
void commutate(void) { // replace this with FastPWM in timer 0? 
  digitalWrite(highPins[commStep], LOW);
  digitalWrite(lowPins[commStep], HIGH);
  while (micros()-startTime < PERIOD*dutyCycle/100);
  setPinsToDefault();
  while (micros()-startTime < PERIOD);
}
void setPinsToDefault(void) {
  digitalWrite(LOW_A, LOW);
  digitalWrite(LOW_B, LOW);
  digitalWrite(LOW_C, LOW);

  digitalWrite(HIGH_A, HIGH);
  digitalWrite(HIGH_B, HIGH);
  digitalWrite(HIGH_C, HIGH);
}
