/*
 * i2c_simple.h
 *
 * Created: 1/12/2019 4:11:47 PM
 *  Author: kangmin
 */ 


#ifndef I2C_SIMPLE_H_
#define I2C_SIMPLE_H_

#include "Arduino.h"
#include <stdint.h>


#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif

#define TWI0_BAUD(F_SCL) ((float)CLK_PER / (float)F_SCL / 2) - 5

#ifdef __cplusplus
extern "C" {
#endif



void i2c_master_init();

void i2c_master_write(uint8_t, uint8_t *, uint8_t);

void i2c_master_read(uint8_t, uint8_t *, uint8_t);

void i2c_master_uninit();

void setFrequency(uint32_t);

#ifdef __cplusplus
}
#endif


#endif /* I2C_SIMPLE_H_ */