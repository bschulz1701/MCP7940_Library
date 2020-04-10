/******************************************************************************
MCP7940.cpp
A simplified library for the DS3231, focused on data logger applications
Bobby Schulz @ Northern Widget LLC
4/4/2018

The DS3231 is a high accuracy tempurature compensated RTC. This chip allows for time to be accurately kept
over long periods of time, and waking up a logger device when required to take measurments or tend to sensors

"That's not fair. That's not fair at all. There was time now. There was, was all the time I needed..."
-Henery Bemis

Distributed as-is; no warranty is given.
******************************************************************************/

// #include "Arduino.h"
#include "MCP7940.h"

#define RETRO_ON_MANUAL //Debug include 


MCP7940::MCP7940()
{

}

int MCP7940::Begin(void)
{
	Wire.begin();


	// Wire.beginTransmission(ADR);
	// Wire.write(0x0E); //Write values to Control reg
	// Wire.write(0x24); //Start oscilator, turn off BBSQW, Turn off alarms, turn on convert
	// return Wire.endTransmission(); //return result of begin, reading is optional
	SetBit(TimeRegs::WeekDay, 3); //Turn backup battery enable

	bool OscError = StartOsc();
	Serial.print("Oscillator State:"); //DEBUG!
	Serial.println(OscError); //DEBUG! 
	// Serial.print("Reg Sates:"); //DEBUG!
	// Serial.print(ReadByte(Control), HEX); //DEBUG!
	// Serial.print(","); //DEBUG!
	// Serial.print(ReadByte(TimeRegs::Seconds), HEX); //DEBUG!
	// Serial.print(","); //DEBUG!
	// Serial.println(ReadByte(TimeRegs::WeekDay), HEX); //DEBUG!
}

int MCP7940::SetTime(int Year, int Month, int Day, int DoW, int Hour, int Min, int Sec)
{
	if(Year > 999) {
		Year = Year - 2000; //FIX! Add compnesation for centry 
	}
	int TimeDate [7]={Sec,Min,Hour,DoW,Day,Month,Year};
	for(int i=0; i<=6;i++){
		if(i == 3) {
			uint8_t DoW_Temp = ReadByte(TimeRegs::WeekDay); //Read in current value
			DoW_Temp = DoW_Temp & 0xF8; //Clear lower 3 bits (day of week portion of register)
			DoW_Temp = DoW_Temp | (DoW & 0x07); //Set lower 3 bits from DoW input
			TimeDate[i] = DoW_Temp; //Return value  
		}
		else { //Otherwise write method for other regs
			int b = TimeDate[i]/10;
			int a = TimeDate[i]-b*10;
			if(i == 2){
				if (b==2)
					b=B00000010;
				else if (b==1)
					b=B00000001;
			}	
			TimeDate[i]= a+(b<<4);
			if(i == 0) TimeDate[i] = TimeDate[i] | 0x80; //Set ST bit to keep oscilator running 

			//FIX! Test for leap year and set LPYR bit
		}
		
		#if defined(RETRO_ON_MANUAL)
			Serial.print(i);
			Serial.print(":");
			Serial.println(TimeDate[i], HEX);
		#endif
		WriteByte(i, TimeDate[i]); //Write value back
		// Wire.beginTransmission(ADR);
		// Wire.write(i); //Write values starting at reg 0x00
		// Wire.write(TimeDate[i]); //Write time date values into regs
		// Wire.endTransmission(); //return result of begin, reading is optional
  }

  //Read back time to test result of write??
}

int MCP7940::SetTime(int Year, int Month, int Day, int Hour, int Min, int Sec)
{
	return SetTime(Year, Month, Day, 0, Hour, Min, Sec); //Pass to full funciton, force WeekDay to zero 
}

String MCP7940::GetTime(int mode)
{
	String temp;
		int TimeDate [7]; //second,minute,hour,null,day,month,year	
		Wire.beginTransmission(ADR); //Ask 1 byte of data 
		Wire.write(0x00); //Read values starting at reg 0x00
		Wire.endTransmission();
		Wire.requestFrom(ADR, 7);	//WAIT FOR DATA BACK FIX!!
		for(int i=0; i<=6;i++){
			if(i==3) {
				i++; //Throw away DoW value?
				Wire.read();
			}

			unsigned int n = Wire.read(); //Read value of reg

			//Process results
			int a=n & B00001111;    
			if(i==2){	
				int b=(n & B00110000)>>4; //24 hour mode
				if(b==B00000010)
					b=20;        
				else if(b==B00000001)
					b=10;
				TimeDate[i]=a+b;
			}
			else if(i==4){
				int b=(n & B00110000)>>4;
				TimeDate[i]=a+b*10;
			}
			else if(i==5){
				int b=(n & B00010000)>>4;
				TimeDate[i]=a+b*10;
			}
			else if(i==6){
				int b=(n & B11110000)>>4;
				TimeDate[i]=a+b*10;
			}
			else{	
				int b=(n & B01110000)>>4;
				TimeDate[i]=a+b*10;	
				}
		}

		Time_Date[0] = TimeDate[6];
		Time_Date[1] = TimeDate[5];
		Time_Date[2] = TimeDate[4];
		Time_Date[3] = TimeDate[2];
		Time_Date[4] = TimeDate[1];
		Time_Date[5] = TimeDate[0];

		String TimeDateStr[7];
		for(int i = 0; i < 6; i++) {
			TimeDateStr[i] = String(Time_Date[i]);
			if(TimeDateStr[i].length() < 2) {
				TimeDateStr[i] = "0" + TimeDateStr[i];
			}
			// Serial.println(TimeDateStr[i]); //DEBUG!
		}
		TimeDateStr[0] = "20" + TimeDateStr[0];

	//Format raw results into appropriate string
	if(mode == 0) //Return in order Year, Month, Day, Hour, Minute, Second (Scientific Style)
	{
		temp.concat(TimeDateStr[0]);
		temp.concat("/") ;
		temp.concat(TimeDateStr[1]);
		temp.concat("/") ;
		temp.concat(TimeDateStr[2]);
		temp.concat(" ") ;
		temp.concat(TimeDateStr[3]);
		temp.concat(":") ;
		temp.concat(TimeDateStr[4]);
		temp.concat(":") ;
		temp.concat(TimeDateStr[5]);
	  	return(temp);
	}

	if(mode == 1) //Return in order Month, Day, Year, Hour, Minute, Second (US Civilian Style)
	{

		temp.concat(TimeDateStr[1]);
		temp.concat("/") ;
		temp.concat(TimeDateStr[2]);
		temp.concat("/") ;
		temp.concat(TimeDateStr[0]);
		temp.concat(" ") ;
		temp.concat(TimeDateStr[3]);
		temp.concat(":") ;
		temp.concat(TimeDateStr[4]);
		temp.concat(":") ;
		temp.concat(TimeDateStr[5]);
	  	return(temp);
	}

	if(mode == 2) //Return in order Month, Day, Year, Hour (12 hour), Minute, Second
	{
		temp.concat(TimeDateStr[1]);
		temp.concat("/") ;
		temp.concat(TimeDateStr[2]);
		temp.concat("/") ;
		temp.concat(TimeDateStr[0]);
		temp.concat(" ") ;
		temp.concat(TimeDate[3] % 12);
		temp.concat(":") ;
		temp.concat(TimeDateStr[4]);
		temp.concat(":") ;
		temp.concat(TimeDateStr[5]);
		if(TimeDate[3] >= 12) temp.concat(" PM");
		else temp.concat(" AM");
	  	return(temp);
	}

	if(mode == 8601)
	{
		temp.concat(TimeDateStr[0]);
		temp.concat("-");
		temp.concat(TimeDateStr[1]);
		temp.concat("-");
		temp.concat(TimeDateStr[2]);
		temp.concat("T");
		temp.concat(TimeDateStr[3]);
		temp.concat(":");
		temp.concat(TimeDateStr[4]);
		temp.concat(":");
		temp.concat(TimeDateStr[5]);
		temp.concat("Z"); //FIX! Hard code for UTC time, allow for a fix??
		return(temp);
	}

	if(mode == 1701) //Returns in order Year, Day (of year), Hour, Minute, Second (Stardate)
	{
		int DayOfYear = 0;
		int MonthDay[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		if(TimeDate[6] % 4 == 0) MonthDay[2] = 29;

		for(int m = 1; m < TimeDate[5]; m++)
		{
			DayOfYear = DayOfYear + MonthDay[m];
		}
		DayOfYear = DayOfYear + TimeDate[4];

		temp.concat(TimeDateStr[6]);
		temp.concat(".") ;
		temp.concat(DayOfYear);
		temp.concat(" ") ;
		temp.concat(TimeDateStr[2]);
		temp.concat(".") ;
		temp.concat(TimeDateStr[1]);
		temp.concat(".") ;
		temp.concat(TimeDateStr[0]);
	  	return(temp);
	}

	else return("Invalid Input");
}

unsigned long MCP7940::GetTimeUnix()
{

}

// float MCP7940::GetTemp()
// {
// 	float Temp = 0;
// 	Wire.beginTransmission(ADR);
// 	Wire.write(0x11); //Read from reg 0x11
// 	Wire.endTransmission();

// 	Wire.requestFrom(ADR, 2);
// 	uint8_t TempHigh = Wire.read(); //Get high reg of temp data
// 	uint8_t TempLow = Wire.read();	//Get low reg of temp data

// 	if(bitRead(TempHigh, 7) == 1) {
// 	TempHigh = (~TempHigh) + 1;  //Take 2s complement of whole temp value
// 	Temp = -1.0*float(TempHigh + 0.25*float(TempLow >> 6)); //Temp = -(Whole + 2^-2 x Frac)
// 	}
// 	else Temp = float(TempHigh) + 0.25*float(TempLow >> 6);	//Temp = (Whole + 2^-2 x Frac)

// 	return Temp; 
// }

int MCP7940::GetValue(int n)	// n = 0:Year, 1:Month, 2:Day, 3:Hour, 4:Minute, 5:Second
{
	GetTime(0); //Update time
	return Time_Date[n]; //Return desired value 
}

int MCP7940::SetAlarm(unsigned int Seconds) { //Set alarm from current time to x seconds from current time 
	//DEFINE LIMITS FOR FUNCTION!!

	if(Seconds == 60) {
		uint8_t AlarmMask = 0x07; //nibble for A1Mx values

		// Wire.beginTransmission(ADR);
		// Wire.write(0x0E); //Write values to control reg
		// Wire.write(0x40); //Turn on 1 Hz square wave
		// Wire.endTransmission(); 

		Wire.beginTransmission(ADR);
		Wire.write(0x0E); //Write values to control reg
		Wire.write(0x06); //Turn on INTCN and Alarm 2
		Wire.endTransmission(); 

		//DEBUG!
		Wire.beginTransmission(ADR);
		Wire.write(0x0F); //Write values to control reg
		Wire.write(0x00); //Clear any alarm flags, set oscilator to run
		Wire.endTransmission(); 

		for(int i=0; i < 3;i++){
			Wire.beginTransmission(ADR);
			Wire.write(0x0B + i); //Write values starting at reg 0x0B
			// Wire.write(((AlarmMask & (1 << i)) << 8)); //Write time date values into regs
			Wire.write(0x80); 
			Wire.endTransmission(); //return result of begin, reading is optional
		}
	}

	else {
	//Currently can not set timer for more than 24 hours
	uint8_t AlarmMask = 0x08; //nibble for A1Mx values
	uint8_t DY = 0; //DY/DT value 
	GetTime(0);

	int AlarmTime[7] = {Time_Date[5], Time_Date[4], Time_Date[3], 0, Time_Date[2], Time_Date[1], Time_Date[0]};
	int AlarmVal[7] = {Seconds % 60, ((Seconds - (Seconds % 60))/60) % 60, ((Seconds - (Seconds % 3600))/3600) % 24, 0, ((Seconds - (Seconds % 86400))/86400), 0, 0};  //Remove unused elements?? FIX!
	int CarryIn = 0; //Carry value
	int CarryOut = 0; 
	int MonthDay[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};  //Use??
	if(AlarmTime[6] % 400 == 0) MonthDay[2] = 29; //If year is divisable by 400, is leap year, correct days in February
	else if((AlarmTime[6] % 4 == 0) && (AlarmTime[6] % 100 != 0)) MonthDay[2] = 29; //Otherwise, if IS dividsable by 4, but NOT multiple of 100, is leap year, correct days in February

	Wire.beginTransmission(ADR);
	Wire.write(0x0E); //Write values to control reg
	Wire.write(0x05); //Turn on INTCN and Alarm 1
	Wire.endTransmission(); 

	//Calc seconds
	if(AlarmTime[0] + AlarmVal[0] >= 60) CarryOut = 1;
	AlarmTime[0] = (AlarmTime[0] + AlarmVal[0]) % 60;
	CarryIn = CarryOut; //Copy over prevous carry

	//Calc minutes
	if(AlarmTime[1] + AlarmVal[1] + CarryIn >= 60) CarryOut = 1;
	else CarryOut = 0;
	AlarmTime[1] = (AlarmTime[1] + AlarmVal[1] + CarryIn) % 60;
	CarryIn = CarryOut; //Copy over prevous carry

	//Calc hours
	if(AlarmTime[2] + AlarmVal[2] + CarryIn >= 24) CarryOut = 1; //OUT OF RANGE??
	else CarryOut = 0;
	AlarmTime[2] = (AlarmTime[2] + AlarmVal[2] + CarryIn) % 24;
	CarryIn = CarryOut; //Copy over prevous carry

	//Calc days 
	if(AlarmTime[4] + AlarmVal[4] + CarryIn > MonthDay[AlarmTime[5]]) CarryOut = 1;  //Carry out if result pushes you beyond current month 
	else CarryOut = 0;
	AlarmTime[4] = (AlarmTime[4] + AlarmVal[4] + CarryIn) % (MonthDay[AlarmTime[5]] + 1);
	if(AlarmTime[4] == 0) AlarmTime[4] = 1; //FIX! Find more elegant way to do this


	//ADD FAILURE NOTIFICATION FOR OUT OF RANGE??

	int Offset = 0;
	for(int i=0; i<=6;i++){
		if(i==3) i++;
		int b= AlarmTime[i]/10;
		int a= AlarmTime[i]-b*10;
		if(i==2){
			if (b==2)
				b=B00000010;
			else if (b==1)
				b=B00000001;
		}	
		AlarmTime[i]= a+(b<<4);
		  
		Wire.beginTransmission(ADR);
		Wire.write(0x07 + Offset); //Write values starting at reg 0x07
		Wire.write(AlarmTime[i] | ((AlarmMask & (1 << Offset)) << 8)); //Write time date values into regs
		Wire.endTransmission(); //return result of begin, reading is optional
		Offset++;
	}
	}

  ClearAlarm();
}

int MCP7940::ClearAlarm() {  //Clear registers to stop alarm, must call SetAlarm again to get it to turn on again
	Wire.beginTransmission(ADR);
	Wire.write(0x0F); //Write values to status reg
	Wire.write(0x00); //Clear all flags
	Wire.endTransmission(); //return result of begin, reading is optional
}

bool MCP7940::StartOsc() //Turn on oscilator, returs TRUE if oscilator is set properly, false otherwise 
{
	uint8_t ControlTemp = ReadByte(Control);
	ControlTemp = ControlTemp & 0xF7; //Clear EXTOSC bit to enable and external oscilator 
	uint8_t SecTemp = ReadByte(TimeRegs::Seconds); //Read value from seconds register to use as mask
	SecTemp = SecTemp | 0x80; //Set ST bit to start oscilator
	WriteByte(Control, ControlTemp); //Write back value of temp control register
	WriteByte(TimeRegs::Seconds, SecTemp); //Write back value of seconds register (for ST bit)
	delay(5); //Wait for oscilator to start 
	// Serial.println(ControlTemp, HEX); //DEBUG!
	// Serial.println(SecTemp, HEX); //DEBUG!
	// Serial.println(Control, HEX); //DEBUG!
	// Serial.println(TimeRegs::Seconds, HEX); //DEBUG!
	return ReadBit(TimeRegs::WeekDay, 5); //Return the OSCRUN bit of the weekday register to test if oscilator is running
}

uint8_t MCP7940::ReadByte(int Reg)
{
	Wire.beginTransmission(ADR); //Point to desired register 
	Wire.write(Reg);
	Wire.endTransmission();

	Wire.requestFrom(ADR, 1); //Ask for 1 byte from RTC
	const unsigned long Timeout = 5; 
	unsigned long LocalTime = millis();
	while(Wire.available() < 1 && (millis() - LocalTime) < Timeout); //Wait at most 5ms for byte to arrive
	if(Wire.available() >= 1) return Wire.read(); //If got byte, return value
	else return 0; //Otherwise return zero 
}

int MCP7940::WriteByte(int Reg, uint8_t Val)
{
	Wire.beginTransmission(ADR);
	Wire.write(Reg);
	Wire.write(Val); //Write value to register
	return Wire.endTransmission(); //Return I2C status 
}

bool MCP7940::ReadBit(int Reg, uint8_t Pos)
{
	uint8_t Val = ReadByte(Reg);
	return (Val >> Pos) & 0x01; //Return single reguested bit
}

int MCP7940::SetBit(int Reg, uint8_t Pos)
{
	uint8_t ValTemp = ReadByte(Reg);
	ValTemp = ValTemp | (1 << Pos); //Set desired bit
	// Serial.println(ValTemp, HEX); //DEBUG!
	return WriteByte(Reg, ValTemp); //Write value back in place
}

int MCP7940::ClearBit(int Reg, uint8_t Pos)
{
	uint8_t ValTemp = ReadByte(Reg); //Grab register
	uint8_t Mask = ~(1 << Pos); //Creat mask to clear register
	ValTemp = ValTemp & Mask; //Clear desired bit
	return WriteByte(Reg, ValTemp); //Write value back
}