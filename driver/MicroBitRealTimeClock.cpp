// Copyright - MIT License -  Thomas Poulet - 2016

#include "MicroBitRealTimeClock.h"
#include "ErrorNo.h"


MicroBitRealTimeClock::MicroBitRealTimeClock(MicroBitI2C &_i2c, uint16_t address) : i2c(_i2c), address(address)
{
}

int MicroBitRealTimeClock::init()
{
	uint8_t command = 0b10000000;

	writeCommand(MCP7940M_RTCSEC, command);

	return MICROBIT_OK;
}

int MicroBitRealTimeClock::getTime(Time *time)
{
	// TODO: Optimize by doing only on i2c call

	// Read the seconds
	uint8_t secondBuffer;
	readCommand(MCP7940M_RTCSEC, &secondBuffer, 1);
	uint8_t secone = secondBuffer & MCP7940M_SECONE;
	uint8_t secten = (( secondBuffer & MCP7940M_SECTEN ) >> 4) * 10;

	time->seconds = secten + secone;

	// Read the minutes
	uint8_t minuteBuffer;
	readCommand(MCP7940M_RTCMIN, &minuteBuffer, 1);
	uint8_t minone = minuteBuffer & MCP7940M_MINONE;
	uint8_t minten = (( minuteBuffer & MCP7940M_MINTEN ) >> 4) * 10;

	time->minutes = minten + minone;

	// Read the hours
	uint8_t hourBuffer;
	readCommand(MCP7940M_RTCHOUR, &hourBuffer, 1);
	uint8_t formatBit = ( hourBuffer & MCP7940M_1224 ) >> 6;
	uint8_t hrone = hourBuffer & MCP7940M_HRONE;
	time->TimeFormat = (bool)formatBit;

	uint8_t hrten = 0;
	if( formatBit )
	{
		hrten = (( hourBuffer & MCP7940M_HRTEN12) >> 4) * 10;
		uint8_t AMPM = ( hourBuffer & MCP7940M_AMPM ) >> 5;

		time->AMPM = (bool)AMPM;
	}
	else
	{
		hrten = (( hourBuffer & MCP7940M_HRTEN24) >> 4) * 10;
	}

	time->hours = hrone + hrten;

	return MICROBIT_OK;
}

int MicroBitRealTimeClock::getDate(Date *date)
{
	// TODO: Optimize to do only one i2c call

	// Read the days
	uint8_t dayBuffer;
	readCommand(MCP7940M_RTCDATE, &dayBuffer, 1);
	uint8_t dateone = dayBuffer & MCP7940M_DATEONE;
	uint8_t dateten = (( dayBuffer & MCP7940M_DATETEN ) >> 4) * 10;

	date->days = dateten + dateone;

	// Read the months
	uint8_t monthBuffer;
	readCommand(MCP7940M_RTCMTH, &monthBuffer, 1);
	uint8_t monthone = monthBuffer & MCP7940M_MTHONE;
	uint8_t monthten = (( monthBuffer & MCP7940M_MTHTEN )  >> 4) * 10;

	date->months = monthten + monthone;

	// Read the years
	uint8_t yearBuffer;
	readCommand(MCP7940M_RTCYEAR, &yearBuffer, 1);
	uint8_t yearone = yearBuffer & MCP7940M_YRONE;
	uint8_t yearten = (( yearBuffer & MCP7940M_YRTEN ) >> 4) * 10;

	date->years = yearten + yearone;

	// Get if leap year
	bool leapYear = ( monthBuffer & MCP7940M_LPYR ) >> 5;
	date->leapYear;
}

int MicroBitRealTimeClock::setTime(const Time &time)
{
	// Split the second to BCD
	uint8_t secten = (time.seconds / 10) << 4;
	uint8_t secone = time.seconds % 10;

	uint8_t secbuffer = secten | secone;

	writeCommand(MCP7940M_RTCSEC, secbuffer);

	// Split the minutes to BCD
	uint8_t minten = (time.minutes / 10 ) << 4;
	uint8_t minone = time.minutes % 10;

	uint8_t minbuffer = minten | minone;

	writeCommand(MCP7940M_RTCMIN, minbuffer);

	// Prepare the hour buffer
	uint8_t hourBuffer = 0;

	if( time.TimeFormat )
		hourBuffer |= MCP7940M_1224;

	if( time.AMPM )
		hourBuffer |= MCP7940M_AMPM;

	uint8_t hourone = time.hours % 10;
	uint8_t hourten = (time.hours / 10) << 4;

	hourBuffer |= hourone | hourten;

	writeCommand(MCP7940M_RTCHOUR, hourBuffer);

	return MICROBIT_OK;
}

int MicroBitRealTimeClock::readCommand(uint8_t reg, uint8_t* buffer, int length)
{
	int result;

	if (buffer == NULL || length <= 0 )
		return MICROBIT_INVALID_PARAMETER;

	result = i2c.write(address, (const char *)&reg, 1, true);
	if (result !=0)
		return MICROBIT_I2C_ERROR;

	result = i2c.read(address, (char *)buffer, length);
	if (result !=0)
		return MICROBIT_I2C_ERROR;

	return MICROBIT_OK;
}

int MicroBitRealTimeClock::writeCommand(uint8_t reg, uint8_t value)
{
	uint8_t command[2];
	command[0] = reg;
	command[1] = value;

	return i2c.write(address, (const char*)command, 2);
}
