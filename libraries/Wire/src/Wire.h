/*
 * Wire.h
 *
 * Created: 1/14/2019 7:55:15 PM
 *  Author: kangmin
 */ 


#ifndef TwoWire_h
#define TwoWire_h

#include "Arduino.h"
#include <inttypes.h>

#define BUFFER_LENGTH 32

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

class TwoWire
{
	private:
	static uint8_t rxBuffer[];
	static uint8_t rxBufferIndex;
	static uint8_t rxBufferLength;

	static uint8_t txAddress;
	static uint8_t txBuffer[];
	static uint8_t txBufferIndex;
	static uint8_t txBufferLength;

	static uint8_t transmitting;
	static void (*user_onRequest)(void);
	static void (*user_onReceive)(int);
	static void onRequestService(uint8_t **, uint8_t **);
	static void onReceiveService(int, uint8_t);
	public:
	TwoWire();
	void begin();
	void begin(uint8_t);
	void begin(int);
	void end();
	void setClock(uint32_t);
	void beginTransmission(uint8_t);
	void beginTransmission(int);
	uint8_t endTransmission(void);
	uint8_t endTransmission(uint8_t);
	uint8_t requestFrom(uint8_t, uint8_t);
	uint8_t requestFrom(int, int);
	virtual size_t write(uint8_t);
	virtual size_t write(uint8_t *, size_t);
	virtual int available(void);
	virtual int read(void);
	virtual int peek(void);
	virtual void flush(void);
	void onReceive( void (*)(int) );
	void onRequest( void (*)(void) );

	inline size_t write(unsigned long n) { return write((uint8_t)n); }
	inline size_t write(long n) { return write((uint8_t)n); }
	inline size_t write(unsigned int n) { return write((uint8_t)n); }
	inline size_t write(int n) { return write((uint8_t)n); }
};

extern TwoWire Wire;



#endif /* WIRE_H_ */