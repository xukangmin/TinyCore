/*
 * i2c_simple.c
 *
 * Created: 1/12/2019 4:11:24 PM
 *  Author: kangmin
 */ 
#include "i2c_master.h"

typedef enum {
	I2C_IDLE,
	I2C_BUS_SEND_WADDR,
	I2C_BUS_SEND_RADDR,
	I2C_BUS_SEND_DATA,
	I2C_BUS_STOP,
	I2C_WRITE,
	I2C_READ,
	I2C_BUSY,
	I2C_BUS_ERROR
} i2c_bus_status;

typedef enum {
	I2C_OK,
	I2C_NO_RESPONSE
} i2c_error;

volatile i2c_bus_status bus_status = I2C_IDLE;

i2c_error err = I2C_OK;

static uint32_t twi_baud_rate = TWI_FREQ;

typedef struct {
	size_t data_len;
	uint8_t *data_ptr;
	size_t data_proced;
	uint8_t addr;
} i2c_data_t;

i2c_data_t i2c_data = {0};

uint8_t check_slave_response()
{
	if (TWI0.MSTATUS & TWI_RXACK_bm) // no slave response
	{
		err = I2C_NO_RESPONSE;	
		
	}
	else {
		err = I2C_OK;
	}
	
	return (TWI0.MSTATUS & TWI_RXACK_bm);
	
}

uint8_t get_error() {
	return err;
}

void i2c_stop() {
	TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
	bus_status = I2C_IDLE;
}

void i2c_nack_stop(){
	TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
	i2c_stop();
}

void bus_act(i2c_bus_status s) {
	switch(s){
		case I2C_IDLE:
			break;
		case I2C_BUS_SEND_WADDR:
			TWI0_MADDR = i2c_data.addr << 1;
  			bus_status = I2C_WRITE;
			break;
		case I2C_BUS_SEND_RADDR:
			TWI0_MADDR = i2c_data.addr << 1 | 1;
			bus_status = I2C_READ;
			break;
		case I2C_READ:
			if (check_slave_response() && i2c_data.data_proced == 0)
			{
				i2c_stop();
			} 
			else {
				i2c_data.data_proced++;
				if (i2c_data.data_proced == i2c_data.data_len)
				{
					i2c_nack_stop();
					*i2c_data.data_ptr = TWI0_MDATA;
				} else {
					TWI0.MCTRLB &= ~(1 << TWI_ACKACT_bp);
					*i2c_data.data_ptr = TWI0_MDATA;
					i2c_data.data_ptr++;
					TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;
					bus_status = I2C_READ;
				}
			}
			break;
		case I2C_WRITE:
			if (check_slave_response())
			{
				i2c_stop();	
			}
			else {
				TWI0_MDATA = *i2c_data.data_ptr++;
				i2c_data.data_proced++;
				if (i2c_data.data_proced == i2c_data.data_len) {
					bus_status = I2C_BUS_STOP;	
				} else {
					bus_status = I2C_WRITE;
				}
			}

			break;
		case I2C_BUS_STOP:
			i2c_stop();
			break;
		default:
			break;
	}
}

ISR(TWI0_TWIM_vect)
{
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
	
	bus_act(bus_status);
}



void i2c_master_init() {
	
		PORTB_DIR |= (1 << 0);
		PORTB_DIR |= (1 << 1);
		
		PORTB_OUT &= ~(1 << 0);
		PORTB_OUT &= ~(1 << 1);
		// TWI0.CTRLA = 0 << TWI_FMPEN_bp /* FM Plus Enable: disabled */
		//		 | TWI_SDAHOLD_OFF_gc /* SDA hold time off */
		//		 | TWI_SDASETUP_4CYC_gc; /* SDA setup time is 4 clock cycles */

		// TWI0.DBGCTRL = 0 << TWI_DBGRUN_bp; /* Debug Run: disabled */

		TWI0.MBAUD = (uint8_t)TWI0_BAUD(twi_baud_rate); /* set MBAUD register */

		TWI0.MCTRLA = 1 << TWI_ENABLE_bp        /* Enable TWI Master: enabled */
		| 0 << TWI_QCEN_bp        /* Quick Command Enable: disabled */
		| 1 << TWI_RIEN_bp        /* Read Interrupt Enable: enabled */
		| 0 << TWI_SMEN_bp        /* Smart Mode Enable: disabled */
		| TWI_TIMEOUT_DISABLED_gc /* Bus Timeout Disabled */
		| 1 << TWI_WIEN_bp;       /* Write Interrupt Enable: enabled */

		bus_status = I2C_IDLE;
		TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;		
}

void i2c_master_uninit(){
		TWI0.MCTRLA = 0x00;
}


void setFrequency(uint32_t clock) {
	i2c_master_uninit();
	twi_baud_rate = clock;
	i2c_master_init();
}

void i2c_master_write(uint8_t addr, uint8_t *data, uint8_t len) {
	
	i2c_data.addr = addr;
	i2c_data.data_ptr = data;
	i2c_data.data_proced = 0;
	i2c_data.data_len = len;
	bus_status = I2C_BUS_SEND_WADDR;
	bus_act(bus_status);
	while(bus_status != I2C_IDLE);
}

void i2c_master_read(uint8_t addr, uint8_t *data, uint8_t len) {
	i2c_data.addr = addr;
	i2c_data.data_ptr = data;
	i2c_data.data_proced = 0;
	i2c_data.data_len = len;
	bus_status = I2C_BUS_SEND_RADDR;
	bus_act(bus_status);
	while(bus_status != I2C_IDLE);
}