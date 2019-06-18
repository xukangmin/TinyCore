/*
 * spi.h
 *
 * Created: 1/20/2019 10:46:32 AM
 *  Author: kangmin
 */ 


#ifndef SPI_H_
#define SPI_H_
#include "Arduino.h"
#include <stdint.h>

class SPIClass {
	public:
		
		static void begin();
		
		static uint8_t transfer(uint8_t data);
		
		static void setClockDivider(uint8_t div);
		
		static void end();
};

extern SPIClass SPI;

#endif /* SPI_H_ */