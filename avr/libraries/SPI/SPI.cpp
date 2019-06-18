/*
 * spi.cpp
 *
 * Created: 1/20/2019 10:46:23 AM
 *  Author: kangmin
 */ 
#include "SPI.h"

SPIClass SPI;

void SPIClass::begin()
{
	PORTA_DIR |= (1 << 1); // PA1 MOSI OUT
	PORTA_DIR &= ~ (1 << 2); // PA2 MISO IN
	PORTA_DIR |= (1 << 3); // PA3 SCK OUT
	PORTA_DIR |= (1 << 4); // SS OUT
	
	PORTA_OUT &= ~(1 << 1);
	PORTA_OUT &= ~(1 << 2);
	PORTA_OUT &= ~(1 << 3);
	PORTA_OUT |= (1 << 4); 
	
	
	SPI0.CTRLA = 0 << SPI_CLK2X_bp    /* Enable Double Speed: disabled */
	| 0 << SPI_DORD_bp   /* Data Order Setting: disabled */
	| 1 << SPI_ENABLE_bp /* Enable Module: enabled */
	| 1 << SPI_MASTER_bp /* SPI module in master mode */
	| SPI_PRESC_DIV4_gc; /* System Clock / 4 */
	
}

uint8_t SPIClass::transfer(uint8_t data) {

		SPI0.DATA = data;
		while (!(SPI0.INTFLAGS & SPI_RXCIF_bm))
		;
		return SPI0.DATA;
	
}

void SPIClass::setClockDivider(uint8_t div) {
	
}

void SPIClass::end()
{
	SPI0.CTRLA &= ~SPI_ENABLE_bm;
}