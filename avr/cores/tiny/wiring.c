#include "wiring_private.h"


#define TIMER0_TOP_VALUE 0xFF

#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(1 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

#define FRACT_INC 8   // calculated base on 12.8 microseoncds per time0 overflow // 256 / 20
#define FRACT_MAX 625

//#define MICROSECONDS_PER_TIMER0_OVERFLOW 12.8   // 256 / 20

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned int timer0_fract = 0;

static void __empty() {
	// Empty
}
void yield(void) __attribute__ ((weak, alias("__empty")));

ISR(TCA0_LUNF_vect)
{
	unsigned long m = timer0_millis;
	unsigned int f = timer0_fract;

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


	// __asm__ __volatile__ (
	// "nop" "\n\t"
	// "nop" "\n\t"
	// "nop" "\n\t"
	// "nop"); //just waiting 4 cycles
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes a 1/5 of a microsecond (4 cycles)
	// per iteration, so execute it five times for each microsecond of
	// delay requested.
	us += (us << 1); // x5 us, = 7 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 26 (28) cycles above, remove 7, (7*4=28)
	// us is at least 10 so we can substract 7
	us -= 7; // 2 cycles

	// F_CPU = 5Mhz

	// if (us <= 10) return;

	// us += us >> 3; // us * 1.125

	// us >>= 1;  // div / 2

	__asm__ __volatile__ (
	"1: sbiw %0,1" "\n\t" // 2 cycles
	"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
}

void init(void)
{
  CCP = CCP_IOREG_gc; // enable write protect

  //CLKCTRL_MCLKCTRLB = (1 << CLKCTRL_PDIV0_bp) | (1 << CLKCTRL_PEN_bp); // change prescalar to 4, so it's 20Mhz/4 = 5MHz

  CLKCTRL_MCLKCTRLB = 0x00;

  SREG |= (1 << CPU_I_bp); // global enable interrupt

  TCA0_SPLIT_CTRLD |= (1 << TCA_SPLIT_ENABLE_bp); // enable 2 8-bit split timer TCA0

  TCA0_SPLIT_LPER = TIMER0_TOP_VALUE;     // set TOP value for TCA0 Low bit

  TCA0_SPLIT_INTCTRL |= (1 << TCA_SPLIT_LUNF_bp); // enable interrupt for TCA0 low bit

  TCA0_SPLIT_CTRLA |= (1 << TCA_SPLIT_ENABLE_bp); // enable TCA0 timer

}
