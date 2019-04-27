#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Stream.h"

#define USART0_BAUD_RATE(BAUD_RATE) ((float)(CLK_PER * 64 / (16 * (float)BAUD_RATE)))

#if !defined(SERIAL_TX_BUFFER_SIZE)
#if ((RAMEND - RAMSTART) < 1023)
#define SERIAL_TX_BUFFER_SIZE 16
#else
#define SERIAL_TX_BUFFER_SIZE 64
#endif
#endif
#if !defined(SERIAL_RX_BUFFER_SIZE)
#if ((RAMEND - RAMSTART) < 1023)
#define SERIAL_RX_BUFFER_SIZE 16
#else
#define SERIAL_RX_BUFFER_SIZE 64
#endif
#endif

#if (SERIAL_TX_BUFFER_SIZE>256)
typedef uint16_t tx_buffer_index_t;
#else
typedef uint8_t tx_buffer_index_t;
#endif
#if  (SERIAL_RX_BUFFER_SIZE>256)
typedef uint16_t rx_buffer_index_t;
#else
typedef uint8_t rx_buffer_index_t;
#endif

// Define config for Serial.begin(baud, config);
#define SERIAL_5N1 0x00
#define SERIAL_6N1 0x01
#define SERIAL_7N1 0x02
#define SERIAL_8N1 0x03
#define SERIAL_5N2 0x08
#define SERIAL_6N2 0x09
#define SERIAL_7N2 0x0A
#define SERIAL_8N2 0x0B
#define SERIAL_5E1 0x20
#define SERIAL_6E1 0x21
#define SERIAL_7E1 0x22
#define SERIAL_8E1 0x23
#define SERIAL_5E2 0x28
#define SERIAL_6E2 0x29
#define SERIAL_7E2 0x2A
#define SERIAL_8E2 0x2B
#define SERIAL_5O1 0x30
#define SERIAL_6O1 0x31
#define SERIAL_7O1 0x32
#define SERIAL_8O1 0x33
#define SERIAL_5O2 0x38
#define SERIAL_6O2 0x39
#define SERIAL_7O2 0x3A
#define SERIAL_8O2 0x3B

class HardwareSerial : public Stream
{
  protected:
    // Has any byte been written to the UART since begin()
    bool _written;

    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;

    // Don't put any members after these buffers, since only the first
    // 32 bytes of this struct can be accessed quickly using the ldd
    // instruction.
    unsigned char _rx_buffer[SERIAL_RX_BUFFER_SIZE];
    unsigned char _tx_buffer[SERIAL_TX_BUFFER_SIZE];

  public:
	inline HardwareSerial();
    void begin(unsigned long baud) { begin(baud, SERIAL_8N1); }
    void begin(unsigned long, uint8_t);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual int availableForWrite(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    
    // Interrupt handlers - Not intended to be called externally
    inline void _rx_complete_irq(void);
    void _tx_udr_empty_irq(void);

     operator bool() { return true; }
};

void HardwareSerial::_rx_complete_irq(void)
{
	if (bit_is_clear(USART0_RXDATAH, USART_PERR_bp)) {
		// No Parity error, read byte and store it in the buffer if there is
		// room
		unsigned char c = USART0_RXDATAL;
		rx_buffer_index_t i = (unsigned int)(_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;

		// if we should be storing the received character into the location
		// just before the tail (meaning that the head would advance to the
		// current location of the tail), we're about to overflow the buffer
		// and so we don't write the character or advance the head.
		if (i != _rx_buffer_tail) {
			_rx_buffer[_rx_buffer_head] = c;
			_rx_buffer_head = i;
		}
		} else {
		// Parity error, read byte but discard it
		USART0_RXDATAL;
	};
}


extern HardwareSerial Serial;
#define HAVE_HWSERIAL0



#endif
