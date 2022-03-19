# NanoSC
A brushless DC motor driver/speed controller based around the Arduino Nano

Note: there are 3 firmware versions using different timing schemes. There is BLDC_driver, which just uses delays and millis() for the timing events, and there are BLDC_driver_timer and BLDC_driver_timer_2 that use the ATMega328P's hardware timers and interrupts for the time events. BLDC_driver_timer uses only timer interrupts for its timing events and BLDC_driver_timer_2 uses timer interrupts for the commutation loop but micros() for the PWM signal generation.

Paradoxically, BLDC_driver seems to be the best at driving motors right now.
