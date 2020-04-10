/******************************************************************************
MCP7940.h
A simplified library for the DS3231, focused on data logger applications
Bobby Schulz @ Northern Widget LLC
4/4/2018

The DS3231 is a high accuracy tempurature compensated RTC. This chip allows for time to be accurately kept
over long periods of time, and waking up a logger device when required to take measurments or tend to sensors

"That's not fair. That's not fair at all. There was time now. There was, was all the time I needed..."
-Henery Bemis

Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef MCP7940_h
#define MCP7940_h

#include "Arduino.h"
#include <Wire.h>

#define SECOND 5
#define MINUTE 4
#define HOUR 3
#define DAY 2
#define MONTH 1
#define YEAR 0






class MCP7940
{
	public:
		MCP7940();
		int Begin(void);
		int SetTime(int Year, int Month, int Day, int DoW, int Hour, int Min, int Sec);
		int SetTime(int Year, int Month, int Day, int Hour, int Min, int Sec);
		String GetTime(int mode = 0);
		unsigned long GetTimeUnix(); 
		// float GetTemp();
		int GetValue(int n);
		int SetAlarm(unsigned int Seconds);
		int ClearAlarm();
		uint8_t ReadByte(int Reg); 

	private:
		bool StartOsc();
		
		int WriteByte(int Reg, uint8_t Val);
		bool ReadBit(int Reg, uint8_t Pos);
		int SetBit(int Reg, uint8_t Pos);
		int ClearBit(int Reg, uint8_t Pos);
		const int ADR = 0x6F; //Address of MCP7940 (non-variable)
		int Time_Date[6]; //Store date time values of integers 
		enum TimeRegs : uint8_t //Define time write/read registers
		{
			Seconds = 0x00,
			Minutes = 0x01,
			Hours = 0x02,
			WeekDay = 0x03,
			Date = 0x04,
			Month = 0x05,
			Year = 0x06,
		};
		const uint8_t Control = 0x07;

};

#endif