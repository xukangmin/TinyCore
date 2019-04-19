/*
 * i2c_slave.c
 *
 * Created: 1/13/2019 4:33:23 PM
 *  Author: kangmin
 */ 
#include "i2c_slave.h"

typedef enum {
	I2C_SLAVE_IDLE,
	I2C_SLAVE_ADDR_OK,
	
} i2c_slave_status;

volatile i2c_slave_status sbus_status = I2C_SLAVE_IDLE;

/*
void slave_bus_act(i2c_slave_status s) {
	switch(s)
	{
		case I2C_SLAVE_ADDR_OK:
					
			break;
		
	}
}
*/

static void (*onSlaveTransmit)(uint8_t **, uint8_t **);
static void (*onSlaveReceive)(int, uint8_t);


uint8_t *tx_data_ptr;
uint8_t *tx_data_len;
volatile uint8_t tx_data_index;

volatile uint8_t tx_data[32] = {0};

volatile uint8_t rx_index = 0;
volatile uint8_t tx_index = 0;

volatile uint8_t a = 0;

ISR(TWI0_TWIS_vect)
{
	if (TWI0.SSTATUS & TWI_APIF_bm) {
		TWI0.SCTRLB |= TWI_SCMD_RESPONSE_gc;
		if (TWI0.SSTATUS & TWI_AP_bm)
		{
			// addr, start of data
			if (TWI0.SSTATUS & TWI_DIR_bm) { // read operation, prepare to send
				// setup ptr
				tx_data_index = 0;
				onSlaveTransmit(&tx_data_ptr, &tx_data_len);
			} else {				     // write operation, prepare to receive
				onSlaveReceive(0, 0);
			}
			
		} else {
			onSlaveReceive(2, 0);
			// stop end of data, start process
		}
		

	}
	
	if (TWI0.SSTATUS & TWI_DIF_bm) {
		if (TWI0.SSTATUS & TWI_DIR_bm){ // read operation, prepare to send data;
			
			if (tx_data_index < *tx_data_len) {
				TWI0.SDATA = *tx_data_ptr++;
				tx_data_index++;
			}
		}
		else{					        // write operation, prepare to receive data
			TWI0.SCTRLB |= (0 << TWI_ACKACT_bp);
			//rx_data[rx_index++] = TWI0.SDATA;
			//*rx_buffer_ptr++ = TWI0.SDATA;
			onSlaveReceive(1, TWI0.SDATA);
		}
	}
	
	
	TWI0.SSTATUS |= (TWI_DIF_bp | TWI_APIF_bp);
	
	
	
}

void i2c_slave_uninit() {
	TWI0.SCTRLA = 0x00;
}

void i2c_slave_init(uint8_t slave_address) {
	
	PORTB_DIR |= (1 << 0);
	PORTB_DIR |= (1 << 1);
	
	PORTB_OUT &= ~(1 << 0);
	PORTB_OUT &= ~(1 << 1);

	TWI0.SADDR = slave_address << 1 | 1;	

	TWI0.SADDRMASK = 0x00;

	TWI0.SCTRLA = 1 << TWI_DIEN_bp  /* Data Interrupt Enable */
	| 1 << TWI_APIEN_bp /* Address or Stop Interrupt Enable */
	| 1 << TWI_PIEN_bp  /* Stop Interrupt Enable */
	| 0 << TWI_PMEN_bp  /* Address Recognition Mode */
	| 0 << TWI_SMEN_bp  /* Smart Mode Enable */
	| 1 << TWI_ENABLE_bp; /* TWI Slave Enable */
}

void attachSlaveTxEvent( void (*function)(uint8_t **, uint8_t **) )
{
	onSlaveTransmit = function;
}

void attachSlaveRxEvent( void (*function)(int, uint8_t))
{
	onSlaveReceive = function;
}
