// Copyright - MIT License -  Thomas Poulet - 2016
//
// This module is used to interface the bbc micro:bit with a realtime
// clock chip such as the MCP7940M
// http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf

#pragma once
#include "mbed.h"
#include "MicroBitConfig.h"
#include "MicroBitI2C.h"
#include "MicroBitComponent.h"


// MCP7940M Constants
#define MCP7940M_ADDRESS	0xde

// MCP7940M Registry map
#define MCP7940M_RTCSEC		0x00
#define MCP7940M_RTCMIN		0x01
#define MCP7940M_RTCHOUR	0x02

// Second bit masks
#define MCP7940M_SECONE		0x0f
#define MCP7940M_SECTEN		0x70

// Minute bit masks
#define MCP7940M_MINONE		0x0f
#define MCP7940M_MINTEN		0x70

// Hour bit masks
#define MCP7940M_HRONE		0x0f
#define MCP7940M_HRTEN12	0x10
#define MCP7940M_HRTEN24	0x30
#define MCP7940M_AMPM		0x20
#define MCP7940M_1224		0x40

// Date time structures
struct Time
{
	// Time format, 12 = true 24 = false
	bool TimeFormat;

	bool AMPM;

	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
};

class MicroBitRealTimeClock : public MicroBitComponent
{
public:
	MicroBitRealTimeClock(MicroBitI2C &_i2c, uint16_t address = MCP7940M_ADDRESS);

	int init();
	int getTime(Time *time);
	int setTime(const Time &time);

private:
	int readCommand(uint8_t reg, uint8_t *buffer, int length);
	int writeCommand(uint8_t reg, uint8_t value);


	uint16_t		address;	// I2C address of the real time clock
	MicroBitI2C&	i2c;		// I2C interface to use
};

