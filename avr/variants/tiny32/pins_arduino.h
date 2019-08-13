
#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

// QFN 20-pin Version

#define NUM_DIGITAL_PINS            21
#define NUM_ANALOG_INPUTS           11
//#define analogInputToDigitalPin(p)  -1

#define digitalPinHasPWM(p)         ((p) == 1 || (p) == 2 || (p) == 3 || (p) == 12 || (p) == 13 || (p) == 14)

#define EXTERNAL_NUM_INTERRUPTS     22
#define NUM_TOTAL_PINS              22
//This part has a USI, not an SPI module. Accordingly, there is no MISO/MOSI in hardware. There's a DI and a DO. When the chip is used as master, DI is used as MISO, DO is MOSI; the defines here specify the pins for master mode, as SPI master is much more commonly used in Arduino-land than SPI slave, and these defines are required for compatibility. Be aware of this when using the USI SPI fucntionality (and also, be aware that the MISO and MOSI markings on the pinout diagram in the datasheet are for ISP programming, where the chip is a slave. The pinout diagram included with this core attempts to clarify this)

#define PIN_SPI_SS    (13)
#define PIN_SPI_MOSI  (10)
#define PIN_SPI_MISO  (11)
#define PIN_SPI_SCK   (12)

#define LED_BUILTIN   (15)

static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#define PIN_WIRE_SDA        (2)
#define PIN_WIRE_SCL        (3)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;


#define PIN_A0   (10)
#define PIN_A1   (11)
#define PIN_A2   (12)
#define PIN_A3   (13)
#define PIN_A4   (14)
#define PIN_A5   (15)
#define PIN_A6   (16)
#define PIN_A7   (5)
#define PIN_A8 	 (4)
#define PIN_A9	 (2)
#define PIN_A10  (3)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;
static const uint8_t A6 = PIN_A6;
static const uint8_t A7 = PIN_A7;
static const uint8_t A8 = PIN_A8;
static const uint8_t A9 = PIN_A9;
static const uint8_t A10 = PIN_A10;

//#define digitalPinToPCICR(p)    (((p) >= 0 && (p) <= 21) ? (&PCICR) : ((uint8_t *)0))
//#define digitalPinToPCICRbit(p) (((p) <= 7) ? 2 : (((p) <= 13) ? 0 : 1))
//#define digitalPinToPCMSK(p)    (((p) <= 7) ? (&PCMSK2) : (((p) <= 13) ? (&PCMSK0) : (((p) <= 21) ? (&PCMSK1) : ((uint8_t *)0))))
//#define digitalPinToPCMSKbit(p) (((p) <= 7) ? (p) : (((p) <= 13) ? ((p) - 8) : ((p) - 14)))

//#define digitalPinToInterrupt(p)  ((p) == 2 ? 0 : ((p) == 3 ? 1 : NOT_AN_INTERRUPT))

#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PORTA_DIR,
	(uint16_t) &PORTB_DIR,
	(uint16_t) &PORTC_DIR,
	NOT_A_PORT,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PORTA_OUT,
	(uint16_t) &PORTB_OUT,
	(uint16_t) &PORTC_OUT,
	NOT_A_PORT,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PORTA_IN,
	(uint16_t) &PORTB_IN,
	(uint16_t) &PORTC_IN,
	NOT_A_PORT,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	PB, /* 0 */
	PB,
	PB,
	PB,
	PB,
	PB,
	PC,
	PC,
	PC, /* 8 */
	PC,
	PA,
	PA,
	PA,
	PA,
	PA, /* 14 */
	PA,
	PA, /* 16 */
	PB,
	PB,
	PC,
	PC, /* 20 */
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	_BV(3), /* 0 */
	_BV(2),
	_BV(1),
	_BV(0),
	_BV(4),
	_BV(5),
	_BV(0),
	_BV(1),
	_BV(2), /* 8 */
	_BV(3),
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5), /* 14 */
	_BV(6),
	_BV(7), /* 16 */
	_BV(6), 
	_BV(7),
	_BV(4),
	_BV(5), /* 20 */
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	NOT_ON_TIMER, /* 0 */
	W02,
	W01,
	W00,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER, /* 8 */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	W03,
	W04,
	W05, /* 14 */
	DAC0,
	NOT_ON_TIMER, /* 16 */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER, /* 20 */
};

const uint8_t PROGMEM pin_to_ctrl_PGM[] = {
	&PORTB_PIN3CTRL, /* 0 */
	&PORTB_PIN2CTRL,
	&PORTB_PIN1CTRL,
	&PORTB_PIN0CTRL,
	&PORTB_PIN4CTRL,
	&PORTB_PIN5CTRL,
	&PORTC_PIN0CTRL,
	&PORTC_PIN1CTRL,
	&PORTC_PIN2CTRL, /* 8 */
	&PORTC_PIN3CTRL,
	&PORTA_PIN0CTRL,
	&PORTA_PIN1CTRL,
	&PORTA_PIN2CTRL,
	&PORTA_PIN3CTRL,
	&PORTA_PIN4CTRL, /* 14 */
	&PORTA_PIN5CTRL,
	&PORTA_PIN6CTRL, /* 16 */
	&PORTB_PIN6CTRL,
	&PORTB_PIN7CTRL,
	&PORTC_PIN4CTRL,
	&PORTC_PIN5CTRL, /* 20 */
};

/* Use this for accessing PINnCTRL register */
const uint8_t PROGMEM digital_pin_to_bit_position[] = {
	// Left side, top to bottom
	PIN4_bp, // 0  PA4
	PIN5_bp, // 1  PA5
	PIN6_bp, // 2  PA6
	PIN7_bp, // 3  PA7
	PIN7_bp, // 4  PB7
	PIN6_bp, // 5  PB6
	PIN5_bp, // 6  PB5
	PIN4_bp, // 7  PB4
	PIN3_bp, // 8  PB3
	PIN2_bp, // 9  PB2
	PIN1_bp, // 10 PB1
	PIN0_bp, // 11 PB0
	PIN0_bp, // 12 PC0
	PIN1_bp, // 13 PC1
	PIN2_bp, // 14 PC2
	PIN3_bp, // 15 PC3
	PIN4_bp, // 16 PA4
	PIN5_bp, // 17 PA5
	PIN1_bp, // 18 PA1
	PIN2_bp, // 19 PA2
	PIN3_bp, // 20 PA3
	PIN0_bp  // 21 PA0
};

/* Use this for accessing PINnCTRL register */
const uint8_t PROGMEM digital_pin_to_bit_mask[] = {	
	PIN4_bm, // 0  PA4
	PIN5_bm, // 1  PA5
	PIN6_bm, // 2  PA6
	PIN7_bm, // 3  PA7
	PIN7_bm, // 4  PB7
	PIN6_bm, // 5  PB6
	PIN5_bm, // 6  PB5
	PIN4_bm, // 7  PB4
	PIN3_bm, // 8  PB3
	PIN2_bm, // 9  PB2
	PIN1_bm, // 10 PB1
	PIN0_bm, // 11 PB0
	PIN0_bm, // 12 PC0
	PIN1_bm, // 13 PC1
	PIN2_bm, // 14 PC2
	PIN3_bm, // 15 PC3
	PIN4_bm, // 16 PA4
	PIN5_bm, // 17 PA5
	PIN1_bm, // 18 PA1
	PIN2_bm, // 19 PA2
	PIN3_bm, // 20 PA3
	PIN0_bm  // 21 PA0
};

#endif

// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.
#define SERIAL_PORT_MONITOR   Serial
#define SERIAL_PORT_HARDWARE  Serial

#endif
