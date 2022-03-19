/*
 * Step: 1 2 3 4 5 6
 * High: A B B C C A
 * Low:  C C A A B B
 * 
 * Used this video as a reference for implementing timers: https://www.youtube.com/watch?v=IdL0_ZJ7V2s
 * Also referenced the datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
 */

#define LOW_A 9
#define LOW_B 8
#define LOW_C 4

#define HIGH_A 5
#define HIGH_B 6
#define HIGH_C 7

// MIN_FREQ, MAX_FREQ, and PULSE_FREQ in Hz
// NOTE: With the current timer configuration, 100 Hz seems like a hard maximum for the commutation frequency
#define MIN_FREQ 1
#define MAX_FREQ 100
#define PULSE_FREQ 1000UL

#define F_CLOCK 16000000UL
#define PRSC0 64
#define PRSC1 256

const int highPins[] = {HIGH_A, HIGH_B, HIGH_B, HIGH_C, HIGH_C, HIGH_A};
const int lowPins[] = {LOW_C, LOW_C, LOW_A, LOW_A, LOW_B, LOW_B};

int commStep = 0; // 0 to 5 corresponding with the steps in the comment above
int targetFreq = MIN_FREQ; // target commutation frequency
int dutyCycle = 50;

void setup() {
  // low side pins
  pinMode(LOW_A, OUTPUT); // A phase
  pinMode(LOW_B, OUTPUT); // B phase
  pinMode(LOW_C, OUTPUT); // C phase

  // high side pins
  pinMode(HIGH_A, OUTPUT); // A phase
  pinMode(HIGH_B, OUTPUT); // B phase
  pinMode(HIGH_C, OUTPUT); // C phase

  // Timer 0 configuration
  TCCR0A = 0;
  TCCR0B = 0;
  OCR0A = F_CLOCK/(PRSC0*PULSE_FREQ);
  OCR0B = OCR0A-1-(OCR0A-1)*dutyCycle/100;
  TCNT0 = 0;
  TCCR0B |= (1 << CS01) | (1 << CS00) | (1 << WGM02); // prescaler 64, CTC mode
  TIMSK0 |= (1 << OCIE0A) | (1 << OCIE0B); // enable compare match interrupt

  // Timer 1 configuration
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 65536 - F_CLOCK/(PRSC1*targetFreq);
  TCCR1B |= (1 << CS12); // prescaler 256
  TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
  
  setPinsToDefault();
}
ISR(TIMER0_COMPA_vect) { // set pins low when timer 0 hits OCR0A
  OCR0B = OCR0A-1-(OCR0A-1)*dutyCycle/100;
  setPinsToDefault();
}
ISR(TIMER0_COMPB_vect) { // set pins high when timer 0 hits OCR0B
  digitalWrite(highPins[commStep], LOW);
  digitalWrite(lowPins[commStep], HIGH);
}
ISR(TIMER1_OVF_vect) { // step commutation
  if (++commStep > 5) {
    commStep = 0;
  }
  TCNT1 = 65536 - F_CLOCK/(PRSC1*targetFreq);
}
void loop() {
  dutyCycle = map(analogRead(A0), 0, 1023, 0, 100); // the time that the MOSFETs will be conducting
  targetFreq = map(analogRead(A1), 0, 1023, MIN_FREQ, MAX_FREQ); // the frequency that the commutation switches (in Hz)
}
void setPinsToDefault(void) {
  digitalWrite(LOW_A, LOW);
  digitalWrite(LOW_B, LOW);
  digitalWrite(LOW_C, LOW);

  digitalWrite(HIGH_A, HIGH);
  digitalWrite(HIGH_B, HIGH);
  digitalWrite(HIGH_C, HIGH);
}
