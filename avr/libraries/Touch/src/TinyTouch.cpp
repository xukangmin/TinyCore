
#include "TinyTouch.h"
#include "Arduino.h"

TinyTouch::TinyTouch(/* args */)
{
  	while (RTC.STATUS > 0) { /* Wait for all register to be synchronized */
	}

	// RTC.CMP = 0x0; /* Compare: 0x0 */

	// RTC.CNT = 0x0; /* Counter: 0x0 */

	RTC.CTRLA = RTC_PRESCALER_DIV1_gc   /* 1 */
	            | 1 << RTC_RTCEN_bp     /* Enable: enabled */
	            | 1 << RTC_RUNSTDBY_bp; /* Run In Standby: enabled */

	// RTC.PER = 0xffff; /* Period: 0xffff */

	// RTC.CLKSEL = RTC_CLKSEL_INT32K_gc; /* 32KHz Internal Ultra Low Power Oscillator (OSCULP32K) */

	// RTC.DBGCTRL = 0 << RTC_DBGRUN_bp; /* Run in debug: disabled */

	RTC.INTCTRL = 1 << RTC_CMP_bp    /* Compare Match Interrupt enable: enabled */
	              | 0 << RTC_OVF_bp; /* Overflow Interrupt enable: disabled */
}

TinyTouch::~TinyTouch()
{
}

void TinyTouch::init()
{
  touch_init();
}

void TinyTouch::TouchHandle() {
  touch_process();
}

uint16_t TinyTouch::get_value(uint8_t sensor_node) {
  return get_sensor_node_signal(sensor_node);
}
