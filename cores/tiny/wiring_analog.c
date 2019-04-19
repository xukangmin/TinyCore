/*
  wiring_analog.c - analog input and output
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  Modified 28 September 2010 by Mark Sproul
*/

#include "wiring_private.h"
#include "pins_arduino.h"

uint8_t analog_reference = DEFAULT;

void analogReference(uint8_t mode)
{
	// can't actually set the register here because the default setting
	// will connect AVCC and the AREF pin, which would cause a short if
	// there's something connected to AREF.
	analog_reference = mode;
}

int analogRead(uint8_t pin)
{
	uint8_t low, high;

  ADC0_CTRLC = 0x00;
  VREF_CTRLA = 0x00;

  switch (analog_reference) {
    case DEFAULT:
      ADC0_CTRLC |= ADC_REFSEL_VDDREF_gc;
      break;
    case INTERNAL0V55:
      VREF_CTRLA |= VREF_ADC0REFSEL_0V55_gc;
      ADC0_CTRLC |= ADC_REFSEL_INTREF_gc;
      break;
    case INTERNAL1V1:
      VREF_CTRLA |= VREF_ADC0REFSEL_1V1_gc;
      ADC0_CTRLC |= ADC_REFSEL_INTREF_gc;
      break;
    case INTERNAL2V5:
      VREF_CTRLA |= VREF_ADC0REFSEL_2V5_gc;
      ADC0_CTRLC |= ADC_REFSEL_INTREF_gc;
      break;
    case INTERNAL4V34:
      VREF_CTRLA |= VREF_ADC0REFSEL_4V34_gc;
      ADC0_CTRLC |= ADC_REFSEL_INTREF_gc;
      break;
    case INTERNAL1V5:
      VREF_CTRLA |= VREF_ADC0REFSEL_1V5_gc;
      ADC0_CTRLC |= ADC_REFSEL_INTREF_gc;
      break;
  }

  ADC0_CTRLC |= ADC_PRESC_DIV4_gc;  // 1.25MHz

  switch (pin) {  // AIN0 is used for programming
    case PIN_A0:
      ADC0_MUXPOS = ADC_MUXPOS_AIN1_gc;
      break;
    case PIN_A1:
      ADC0_MUXPOS = ADC_MUXPOS_AIN2_gc;
      break;
    case PIN_A2:
      ADC0_MUXPOS = ADC_MUXPOS_AIN3_gc;
      break;
    case PIN_A3:
      ADC0_MUXPOS = ADC_MUXPOS_AIN4_gc;
      break;
    case PIN_A4:
      ADC0_MUXPOS = ADC_MUXPOS_AIN5_gc;
      break;
    case PIN_A5:
      ADC0_MUXPOS = ADC_MUXPOS_AIN6_gc;
      break;
    case PIN_A6:
      ADC0_MUXPOS = ADC_MUXPOS_AIN7_gc;
      break;
    case PIN_A7:
      ADC0_MUXPOS = ADC_MUXPOS_AIN8_gc;
      break;
    case PIN_A8:
      ADC0_MUXPOS = ADC_MUXPOS_AIN9_gc;
      break;
    case PIN_A9:
      ADC0_MUXPOS = ADC_MUXPOS_AIN10_gc;
      break;
    case PIN_A10:
      ADC0_MUXPOS = ADC_MUXPOS_AIN11_gc;
      break;
  }

	ADC0_CTRLA |= (1 << ADC_ENABLE_bp);

  ADC0_COMMAND |= (1 << ADC_STCONV_bp);

  while (bit_is_set(ADC0_COMMAND, ADC_STCONV_bp));

	low  = ADC0_RESL;
	high = ADC0_RESH;

	// combine the two bytes
	return (high << 8) | low;
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint8_t pin, int val)
{
	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.  Also, make sure the pin is in output mode
	// for consistenty with Wiring, which doesn't require a pinMode
	// call for the analog output pins.
	pinMode(pin, OUTPUT);
	if (val == 0)
	{
		digitalWrite(pin, LOW);
	}
	else if (val == 255)
	{
		digitalWrite(pin, HIGH);
	}
	else
	{
		switch(digitalPinToTimer(pin))
		{
			case W00:
				TCA0_SPLIT_LCMP0 = val;
				TCA0_SPLIT_CTRLB |= (1 << TCA_SPLIT_LCMP0EN_bp);
				break;
			case W01:
				TCA0_SPLIT_LCMP1 = val;
				TCA0_SPLIT_CTRLB |= (1 << TCA_SPLIT_LCMP1EN_bp);
				break;
			case W02:
				TCA0_SPLIT_LCMP2 = val;
				TCA0_SPLIT_CTRLB |= (1 << TCA_SPLIT_LCMP2EN_bp);
				break;
			case W03:
				TCA0_SPLIT_HCMP0 = val;
				TCA0_SPLIT_CTRLB |= (1 << TCA_SPLIT_HCMP0EN_bp);
				break;
			case W04:
				TCA0_SPLIT_HCMP1 = val;
				TCA0_SPLIT_CTRLB |= (1 << TCA_SPLIT_HCMP1EN_bp);
				break;
			case W05:
				TCA0_SPLIT_HCMP2 = val;
				TCA0_SPLIT_CTRLB |= (1 << TCA_SPLIT_HCMP2EN_bp);
				break;
			case DAC0:
				VREF_CTRLA = VREF_DAC0REFSEL_2V5_gc;
				DAC0_CTRLA |= (1 << DAC_OUTEN_bp);
				DAC0_DATA = val;
				DAC0_CTRLA |= (1 << DAC_ENABLE_bp);
				break;
			case NOT_ON_TIMER:
			default:
				if (val < 128) {
					digitalWrite(pin, LOW);
				} else {
					digitalWrite(pin, HIGH);
				}
		}
	}
}
