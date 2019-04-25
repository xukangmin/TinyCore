#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino.h"

void pinMode(uint8_t pin, uint8_t mode)
{
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *reg, *out, *ctrl_reg;

	if (port == NOT_A_PIN) return;

	// JWS: can I let the optimizer do this?
	reg = portModeRegister(port);
	out = portOutputRegister(port);
  ctrl_reg = pinToCtrlRegister(pin);
	if (mode == INPUT) {
		uint8_t oldSREG = SREG;
    cli();
		*reg &= ~bit;
		*out &= ~bit;
		SREG = oldSREG;
	}
  else if (mode == INPUT_PULLUP) {
		uint8_t oldSREG = SREG;
    cli();
		*reg &= ~bit;
    *ctrl_reg |= (1 << PORT_PULLUPEN_bp);   // page 157, pullup enable
		*out |= bit;
		SREG = oldSREG;
	}
  else {
		uint8_t oldSREG = SREG;
    cli();
		*reg |= bit;
		SREG = oldSREG;
	}
}

// Forcing this inline keeps the callers from having to push their own stuff
// on the stack. It is a good performance win and only takes 1 more byte per
// user than calling. (It will take more bytes on the 168.)
//
// But shouldn't this be moved into pinMode? Seems silly to check and do on
// each digitalread or write.
//
// Mark Sproul:
// - Removed inline. Save 170 bytes on atmega1280
// - changed to a switch statment; added 32 bytes but much easier to read and maintain.
// - Added more #ifdefs, now compiles for atmega645
//
//static inline void turnOffPWM(uint8_t timer) __attribute__ ((always_inline));
//static inline void turnOffPWM(uint8_t timer)
static void turnOffPWM(uint8_t timer)
{
	switch (timer)
	{
    case W00:
      cbi(TCA0_SPLIT_CTRLB, TCA_SPLIT_LCMP0EN_bp);
      break;
    case W01:
      cbi(TCA0_SPLIT_CTRLB, TCA_SPLIT_LCMP1EN_bp);
      break;
    case W02:
      cbi(TCA0_SPLIT_CTRLB, TCA_SPLIT_LCMP2EN_bp);
      break;
    case W03:
      cbi(TCA0_SPLIT_CTRLB, TCA_SPLIT_HCMP0EN_bp);
      break;
    case W04:
      cbi(TCA0_SPLIT_CTRLB, TCA_SPLIT_HCMP1EN_bp);
      break;
    case W05:
      cbi(TCA0_SPLIT_CTRLB, TCA_SPLIT_HCMP2EN_bp);
      break;
	}
}

void digitalWrite(uint8_t pin, uint8_t val)
{
	uint8_t timer = digitalPinToTimer(pin);
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *out;

	if (port == NOT_A_PIN) return;

	// If the pin that support PWM output, we need to turn it off
	// before doing a digital write.
	if (timer != NOT_ON_TIMER) turnOffPWM(timer);

	out = portOutputRegister(port);

	uint8_t oldSREG = SREG;
	cli();

	if (val == LOW) {
		*out &= ~bit;
	} else {
		*out |= bit;
	}

	SREG = oldSREG;
}

int digitalRead(uint8_t pin)
{
	uint8_t timer = digitalPinToTimer(pin);
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);

	if (port == NOT_A_PIN) return LOW;

	// If the pin that support PWM output, we need to turn it off
	// before getting a digital reading.
	if (timer != NOT_ON_TIMER) turnOffPWM(timer);

	if (*portInputRegister(port) & bit) return HIGH;
	return LOW;
}
