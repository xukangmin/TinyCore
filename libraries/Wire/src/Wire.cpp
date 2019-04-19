/*
 * Wire.cpp
 *
 * Created: 1/14/2019 7:54:22 PM
 *  Author: kangmin
 */ 
extern "C" {
	#include <stdlib.h>
	#include <string.h>
	#include <inttypes.h>
}

#include "Wire.h"
#include "i2c_master.h"
#include "i2c_slave.h"

uint8_t TwoWire::rxBuffer[BUFFER_LENGTH];
uint8_t TwoWire::rxBufferIndex = 0;
uint8_t TwoWire::rxBufferLength = 0;

uint8_t TwoWire::txAddress = 0;
uint8_t TwoWire::txBuffer[BUFFER_LENGTH];
uint8_t TwoWire::txBufferIndex = 0;
uint8_t TwoWire::txBufferLength = 0;

uint8_t TwoWire::transmitting = 0;
void (*TwoWire::user_onRequest)(void);
void (*TwoWire::user_onReceive)(int);

// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire()
{
}

void TwoWire::begin(void)	 // init master
{
	transmitting = 1; // master mode
	rxBufferIndex = 0;
	rxBufferLength = 0;

	i2c_master_init();
	//twi_attachSlaveTxEvent(onRequestService); // default callback must exist
	//twi_attachSlaveRxEvent(onReceiveService); // default callback must exist
}

void TwoWire::begin(uint8_t address)
{
	transmitting = 0; // slave mode
	rxBufferIndex = 0;
	rxBufferLength = 0;
		
	i2c_slave_init(address);
	attachSlaveTxEvent(onRequestService); // default callback must exist
    attachSlaveRxEvent(onReceiveService); // default callback must exist
}

void TwoWire::begin(int address)
{
	begin((uint8_t)address);
}

void TwoWire::end(void)
{
	i2c_master_uninit();
	i2c_slave_uninit();
}

void TwoWire::setClock(uint32_t clock)
{
	setFrequency(clock);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity) // support only the simplest case to reduce complexity
{
	i2c_master_read(address, rxBuffer, quantity);
	
	rxBufferLength = quantity;
	
	rxBufferIndex = 0;
	
	return quantity;
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
	return requestFrom((uint8_t)address, (uint8_t)quantity);
}

void TwoWire::beginTransmission(uint8_t address)
{
	// indicate that we are transmitting
	transmitting = 1;
	// set address of targeted slave
	txAddress = address;
	// reset tx buffer iterator vars
}

void TwoWire::beginTransmission(int address)
{
	beginTransmission((uint8_t)address);
}

size_t TwoWire::write(uint8_t data)
{
	// in master transmitter mode
	// don't bother if buffer is full
	if(txBufferLength >= BUFFER_LENGTH){
		return 0;
	}
	// put byte in tx buffer
	txBuffer[txBufferIndex] = data;
	++txBufferIndex;
	// update amount in buffer
	txBufferLength = txBufferIndex;
	
	return 1;
}

size_t TwoWire::write(uint8_t *data, size_t quantity)
{
    for(size_t i = 0; i < quantity; ++i){
	    write(data[i]);
    }
		
	return quantity;
}


int TwoWire::available(void)
{
	return rxBufferLength - rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::read(void)
{
	int value = -1;
	
	// get each successive byte on each call
	if(rxBufferIndex < rxBufferLength){
		value = rxBuffer[rxBufferIndex];
		++rxBufferIndex;
	}

	return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::peek(void)
{
	int value = -1;
	
	if(rxBufferIndex < rxBufferLength){
		value = rxBuffer[rxBufferIndex];
	}

	return value;
}

uint8_t TwoWire::endTransmission(uint8_t sendStop)
{
	// transmit buffer (blocking)
	//uint8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, 1, sendStop);
	
	i2c_master_write(txAddress, txBuffer, txBufferLength);
	// reset tx buffer iterator vars
	txBufferIndex = 0;
	txBufferLength = 0;
	// indicate that we are done transmitting
	transmitting = 0;
	
	return 0;
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint8_t TwoWire::endTransmission(void)
{
	return endTransmission(true);
}


void TwoWire::onReceiveService(int status, uint8_t inByte)
{
  // don't bother if user hasn't registered a callback
  if(!user_onReceive){
    return;
  }
  switch(status) {
	  case 0: // start
		rxBufferIndex = 0;
		break;
	  case 1: // rx
		rxBuffer[rxBufferIndex] = inByte;
		rxBufferIndex++;
		break;
	  case 2:
		rxBufferLength = rxBufferIndex + 1;
		rxBufferIndex = 0;
		user_onReceive(rxBufferLength - 1);
		
		break; // stop
		
  }
  // alert user program
  
}

void TwoWire::onRequestService(uint8_t **tx_buffer_ptr, uint8_t **tx_data_length)
{
  // don't bother if user hasn't registered a callback
  if(!user_onRequest){
    return;
  }
  
  txBufferIndex = 0;
  
  user_onRequest();
  
  // set ptr
  *tx_buffer_ptr = txBuffer;
  *tx_data_length = &txBufferLength;
}

// sets function called on slave write
void TwoWire::onReceive( void (*function)(int) )
{
  user_onReceive = function;
}

// sets function called on slave read
void TwoWire::onRequest( void (*function)(void) )
{
  user_onRequest = function;
}

void TwoWire::flush(void)
{
	// XXX: to be implemented.
}


TwoWire Wire = TwoWire();