#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Arduino.h"
#include "pins_arduino.h"

volatile uint8_t *tone_out;
volatile uint8_t tone_bit;
volatile uint8_t tone_port;
volatile uint16_t tone_top_value;
volatile unsigned long timer1_overflow_count = 0;
ISR(TCB0_INT_vect)
{
  *tone_out ^= tone_bit;
  timer1_overflow_count++;
	TCB0_INTFLAGS |= (1 << TCB_CAPT_bp);
}

void tone(uint8_t _pin, unsigned long frequency, unsigned long duration)  // duration in milliseconds
{
  tone_bit = digitalPinToBitMask(_pin);
  tone_port = digitalPinToPort(_pin);

  tone_out = portOutputRegister(tone_port);

  pinMode(_pin, OUTPUT);

  tone_top_value = 1250000L / frequency; // 1.25Mhz / frequency

  TCB0_CCMP = tone_top_value;

  TCB0_INTCTRL |= (1 << TCB_CAPT_bp);  // enable interrupt
	TCB0_CTRLA |= (1 << TCB_CLKSEL0_bp); // use 5Mhz / 2 = 2.5Mhz  for PWM to reach very low 19 Hz
	TCB0_CTRLB |= (1 << TCB_CCMPEN_bp);  //  enable compare output
	TCB0_CTRLA |= (1 << TCB_ENABLE_bp); // enable timer B

  if (duration > 0)
  {
    timer1_overflow_count = 0;

    while(timer1_overflow_count * 500 / frequency <= duration);

    TCB0_CTRLA &= ~(1 << TCB_ENABLE_bp);
  }
}

void noTone(uint8_t _pin)
{
  TCB0_CTRLA &= ~(1 << TCB_ENABLE_bp);
}
