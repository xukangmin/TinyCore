#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#include "HardwareSerial.h"

ISR(USART0_RXC_vect)
{
	Serial._rx_complete_irq();
}


HardwareSerial::HardwareSerial()
{
}

void HardwareSerial::_tx_udr_empty_irq()
{
	  // If interrupts are enabled, there must be more data in the output
  // buffer. Send the next byte
  unsigned char c = _tx_buffer[_tx_buffer_tail];
  _tx_buffer_tail = (_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;

  USART0_TXDATAL = c;

  // clear the TXC bit -- "can be cleared by writing a one to its bit
  // location". This makes sure flush() won't return until the bytes
  // actually got written. Other r/w bits are preserved, and zeroes
  // written to the rest.

  if (_tx_buffer_head == _tx_buffer_tail) {
    // Buffer empty, so disable interrupts
	USART0_CTRLA &= ~ (1 << USART_DREIE_bp);
  }

}

ISR(USART0_DRE_vect)
{
	Serial._tx_udr_empty_irq();
}



void HardwareSerial::begin(unsigned long baud, byte config)
{
  PORTB_DIR |= (1 << 2);
  PORTB_DIR &= ~(1 << 3);


  USART0.BAUD = (uint16_t)USART0_BAUD_RATE(baud); /* set baud rate register */

  USART0.CTRLB = 0 << USART_MPCM_bp      /* Multi-processor Communication Mode: disabled */
              	| 0 << USART_ODME_bp     /* Open Drain Mode Enable: disabled */
              	| 1 << USART_RXEN_bp     /* Reciever enable: enabled */
              	| USART_RXMODE_NORMAL_gc /* Normal mode */
              	| 0 << USART_SFDEN_bp    /* Start Frame Detection Enable: disabled */
              	| 1 << USART_TXEN_bp;    /* Transmitter Enable: enabled */

  USART0.CTRLC = config;

  //USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc /* Asynchronous Mode */
  //        		 | USART_CHSIZE_8BIT_gc /* Character size: 8 bit */
  //        		 | USART_PMODE_DISABLED_gc /* No Parity */
  //        		 | USART_SBMODE_1BIT_gc; /* 1 stop bit */

  USART0.CTRLA |= (1 << USART_RXCIE_bp);
	//USART0.CTRLA |= (1 << USART_DREIE_bp);

  _written = false;

}

void HardwareSerial::end()
{
  // wait for transmission of outgoing data
  flush();

  USART0.CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);

  // clear any received data
  _rx_buffer_head = _rx_buffer_tail;
}

int HardwareSerial::available(void)
{
  return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) % SERIAL_RX_BUFFER_SIZE;
}

int HardwareSerial::peek(void)
{
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    return _rx_buffer[_rx_buffer_tail];
  }
}

int HardwareSerial::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    unsigned char c = _rx_buffer[_rx_buffer_tail];
    _rx_buffer_tail = (rx_buffer_index_t)(_rx_buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
    return c;
  }
}

int HardwareSerial::availableForWrite(void)
{
  tx_buffer_index_t head;
  tx_buffer_index_t tail;

  head = _tx_buffer_head;
  tail = _tx_buffer_tail;

  if (head >= tail) return SERIAL_TX_BUFFER_SIZE - 1 - head + tail;
  return tail - head - 1;
}


void HardwareSerial::flush()
{
  // If we have never written a byte, no need to flush. This special
  // case is needed since there is no way to force the TXC (transmit
  // complete) bit to 1 during initialization
  if (!_written)
    return;

  while (!(USART0.STATUS & USART_DREIF_bm) && _tx_buffer_head == _tx_buffer_tail);
}

size_t HardwareSerial::write(uint8_t c)
{
  _written = true;

  tx_buffer_index_t i = (_tx_buffer_head + 1) % SERIAL_TX_BUFFER_SIZE;

    while (i == _tx_buffer_tail) {
    if (bit_is_clear(SREG, CPU_I_bp)) {
      // Interrupts are disabled, so we'll have to poll the data
      // register empty flag ourselves. If it is set, pretend an
      // interrupt has happened and call the handler to free up
      // space for us.
      if(bit_is_set(USART0.CTRLA, USART_DREIE_bp))
			_tx_udr_empty_irq();
    } else {
      // nop, the interrupt handler will free up space for us
    }
  }

  _tx_buffer[_tx_buffer_head] = c;

  _tx_buffer_head = i;
  USART0.CTRLA |= (1 << USART_DREIE_bp);

  return 1;
}

HardwareSerial Serial;
