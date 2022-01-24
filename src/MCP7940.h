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
// #include <cstdint>
#ifdef ARDUINO 
# define NO_CSTDINT 1  // AVR arduino has no <cstdint>; but we're coding to portable C++. So substitute.
#endif

// unless we know otherwise, use the compiler's <cstdint>
#ifndef NO_CSTDINT
# include <cstdint>
#else
// no <cstdint> -- make sure std:: contains the things we need.
# include <stdint.h>

namespace std {
  using ::int8_t;             
  using ::uint8_t;            
                     
  using ::int16_t;            
  using ::uint16_t;           
                     
  using ::int32_t;            
  using ::uint32_t;           
}

#endif

// enum class AlarmRegs : uint8_t //Define time write/read registers
// {
// 	Seconds = 0x0A,
// 	Minutes = 0x0B,
// 	Hours = 0x0C,
// 	WeekDay = 0x0D,
// 	Date = 0x0E,
// 	Month = 0x0F,
// };




class MCP7940
{
	public:
		enum class Format: int
		{
			Scientific = 0,
			Civilian = 1,
			US = 2,
			ISO_8601 = 3,
			Stardate = 1701
		};

		enum class Mode: int
		{
			Normal = 0,
			Inverted = 1
		};

		struct Timestamp {
			uint16_t year;  // e.g. 2020
			uint8_t  month; // 1-12
			uint8_t  mday;  // Day of the month, 1-31
			uint8_t  wday;  // Day of the week, 1-7
			uint8_t  hour;  // 0-23
			uint8_t  min;   // 0-59
			uint8_t  sec;   // 0-59
		};

		MCP7940();
		int Begin(bool UseExtOsc = false);
		int SetTime(int Year, int Month, int Day, int DoW, int Hour, int Min, int Sec);
		int SetTime(int Year, int Month, int Day, int Hour, int Min, int Sec);
		Timestamp GetRawTime();
		String GetTime(Format mode = Format::Scientific); //Default to scientifc
		unsigned long GetTimeUnix(); 
		// float GetTemp();
		int SetMode(Mode Val); 
		int GetValue(int n);
		int SetAlarm(unsigned int Seconds, bool AlarmNum = 0); //Default to ALM0
		int SetMinuteAlarm(unsigned int Offset, bool AlarmVal = 0); //Default to ALM0
		int SetHourAlarm(unsigned int Offset, bool AlarmVal = 0); //Default to ALM0
		int SetDayAlarm(unsigned int Offset, bool AlarmVal = 0); //Default to ALM0
		int EnableAlarm(bool State = true, bool AlarmVal = 0); //Default to ALM0, enable
		int ClearAlarm(bool AlarmVal = 0); //Default to ALM0
		bool ReadAlarm(bool AlarmVal = 0); //Default to ALM0

		uint8_t ReadByte(int Reg); //DEBUG! Make private
		

	private:
		bool StartOsc();
		
		int WriteByte(int Reg, uint8_t Val);
		bool ReadBit(int Reg, uint8_t Pos);
		int SetBit(int Reg, uint8_t Pos);
		int ClearBit(int Reg, uint8_t Pos);
		const int ADR = 0x6F; //Address of MCP7940 (non-variable)
		int Time_Date[6]; //Store date time values of integers 

		const uint8_t Control = 0x07;

};

#endif