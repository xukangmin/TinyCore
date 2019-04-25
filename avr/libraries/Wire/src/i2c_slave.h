/*
 * i2c_slave.h
 *
 * Created: 1/13/2019 4:33:34 PM
 *  Author: kangmin
 */ 


#ifndef I2C_SLAVE_H_
#define I2C_SLAVE_H_

#include "Arduino.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
	#endif



	void i2c_slave_init(uint8_t);
	
	void i2c_slave_uninit();
	
	void attachSlaveTxEvent( void (*function)(uint8_t **, uint8_t **) );
	
	void attachSlaveRxEvent( void (*function)(int, uint8_t));
	
	#ifdef __cplusplus
}
#endif


#endif /* I2C_SLAVE_H_ */