#include "wiring_private.h"


#define TIMER0_TOP_VALUE 0xFF

#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(1 * TIMER0_TOP_VALUE))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

static void __empty() {
	// Empty
}
void yield(void) __attribute__ ((weak, alias("__empty")));

ISR(TCA0_LUNF_vect)
{
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;
	TCA0_SPLIT_INTFLAGS |= (1 << TCA_SPLIT_LUNF_bp);
}

unsigned long millis()
{
  unsigned long m;
  uint8_t oldSREG = SREG;

  // disable interrupts while we read millis_timer_millis or we might get an
  // inconsistent value (e.g. in the middle of a write to millis_timer_millis)
  cli();
  m = timer0_millis;
  SREG = oldSREG;

  return m;
}

unsigned long micros()
{
	unsigned long m;
	uint8_t oldSREG = SREG, t;
	// disable interrupts while we read millis_timer_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to millis_timer_millis)
	cli();

	t = TCA0_SPLIT_LCNT;

	m = timer0_overflow_count;

	SREG = oldSREG;

	return (m * TIMER0_TOP_VALUE + TIMER0_TOP_VALUE - t) / clockCyclesPerMicrosecond();
}

void delay(unsigned long ms)
{
	uint16_t start = (uint16_t)micros();

	while (ms > 0) {
		yield();
		while (((uint16_t)micros() - start) >= 1000 && ms) {
			ms--;
			start += 1000;
		}
	}
}

void delayMicroseconds(unsigned int us)
{
	// F_CPU = 5Mhz

	if (us <= 10) return;

	us += us >> 3; // us * 1.125

	us >>= 1;  // div / 2

	__asm__ __volatile__ (
	"1: sbiw %0,1" "\n\t" // 2 cycles
	"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
}

void init(void)
{
  CCP = CCP_IOREG_gc; // enable write protect

  CLKCTRL_MCLKCTRLB = (1 << CLKCTRL_PDIV0_bp) | (1 << CLKCTRL_PEN_bp); // change prescalar to 4, so it's 20Mhz/4 = 5MHz

  //CLKCTRL_MCLKCTRLB = 0x00;

  SREG |= (1 << CPU_I_bp); // global enable interrupt

  TCA0_SPLIT_CTRLD |= (1 << TCA_SPLIT_ENABLE_bp); // enable 2 8-bit split timer TCA0

  TCA0_SPLIT_LPER = TIMER0_TOP_VALUE;     // set TOP value for TCA0 Low bit

  TCA0_SPLIT_INTCTRL |= (1 << TCA_SPLIT_LUNF_bp); // enable interrupt for TCA0 low bit

  TCA0_SPLIT_CTRLA |= (1 << TCA_SPLIT_ENABLE_bp); // enable TCA0 timer

}
