#include <MCP7940.h>

#define ALARM_PIN 2 //Pin used for alarm interrupt, must be an interrupt compattible pin! 
//See https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/

//Setup which variables will be tested, any variable set to 1 will be incremented over, others will use a single base value 
#define INCREMENT_YEAR 0
#define INCREMENT_MONTH 1
#define INCREMENT_DAY 0
#define INCREMENT_HOUR 1
#define INCREMENT_MIN 0
#define INCREMENT_SEC 1

MCP7940 rtc; //Instantiate RTC instance 
volatile bool NewAlarm = false; //Flag to indicate a new interrupt, cleared in loop, set in ISR
const unsigned long Period = 2; //Period for alarm0 [seconds]
const unsigned long Timeout = 3; //Number of seconds to wait before declaring a failure 
const unsigned long Timeout_ms = Timeout*1000; //Number of milli-seconds to wait before declaring a failure, prevents in line math later 
// const unsigned long Period1 = 33; //Period for alarm1 [seconds]

const int MonthDay[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};  //Use??
const String MonthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

const int YearVals[] = {2020, 2021, 2099, 2100}; //Teat leap year, non-leap year, centry roll over
const int MonthVals[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}; //DEBUG!
// const int MonthVals[] = {1, 2, 3}; //DEBUG!
const int DayVals[] = {31, 29, 28, 15, 1};
const int HourVals[] = {23, 9, 0};
const int MinVals[] = {59, 30, 0};
const int SecVals[] = {59, 30, 0};


//Calculate length of all test arrays for N values 
//FIX! Is this advisable? Using sizeof() division
int YearNum = 1;
// if(INCREMENT_YEAR) (sizeof(YearVals)/sizeof(YearVals[0])); //Default values to skip over (1), only change if set to iterate 
int MonthNum = 1;
// if(INCREMENT_MONTH) (sizeof(MonthVals)/sizeof(MonthVals[0])); 
int DayNum = 1;
// if(INCREMENT_DAY) (sizeof(DayVals)/sizeof(DayVals[0]));
int HourNum = 1;
// if(INCREMENT_HOUR) (sizeof(HourVals)/sizeof(HourVals[0]));
int MinNum = 1;
// if(INCREMENT_MIN) (sizeof(MinVals)/sizeof(MinVals[0]));
int SecNum = 1;
// if(INCREMENT_SEC) (sizeof(SecVals)/sizeof(SecVals[0])); 

void setup() {
	//FIX! Switch to pre-compile 
	if(INCREMENT_YEAR) YearNum = (sizeof(YearVals)/sizeof(YearVals[0])); //Default values to skip over (1), only change if set to iterate 
	if(INCREMENT_MONTH) MonthNum = (sizeof(MonthVals)/sizeof(MonthVals[0])); 
	if(INCREMENT_DAY) DayNum = (sizeof(DayVals)/sizeof(DayVals[0]));
	if(INCREMENT_HOUR) HourNum = (sizeof(HourVals)/sizeof(HourVals[0]));
	if(INCREMENT_MIN) MinNum = (sizeof(MinVals)/sizeof(MinVals[0]));
	if(INCREMENT_SEC) SecNum = (sizeof(SecVals)/sizeof(SecVals[0])); 

	Serial.begin(115200); //Begin serial for reporting
	while(!Serial); //Wait for serial to begin, required for some systems

	Serial.println(YearNum); //DEBUG!
	Serial.println("Begin Setup...");
	Serial.println(rtc.Begin()); //Inialize rtc //FIX! Test?? //DEBUG! Remove print
	// rtc.SetTime(2049, 12, 15, 1, 8, 15, 30); //FIX! Set via serial //Force time set for testing 
	// Serial.print("Current Time: ");
	// Serial.println(rtc.GetTime(MCP7940::Format::ISO_8601)); //Print ISO formatted time
	// Serial.print("Alarm0 Period: ");
	// Serial.print(Period0);
	// Serial.print("s\n");
	// Serial.print("Alarm1 Period: ");
	// Serial.print(Period1);
	// Serial.print("s\n");
	Serial.println("...End Setup"); 

	attachInterrupt(digitalPinToInterrupt(ALARM_PIN), RTC_Alarm, FALLING); //Attach the interrupt for the alarm, trigger on a falling edge

	// rtc.SetAlarm(Period0, 0); //Setup alarm0
	// rtc.SetAlarm(Period1, 1); //Setup alarm1
}

void loop() {
	// static unsigned long Alarm0Time = millis(); //Keep track of how long since last alarm0
	// static unsigned long Alarm1Time = millis(); //Keep track of how long since last alarm1

	for(int Year = 0; Year < YearNum; Year++) {
		Serial.println(YearVals[Year]); //Print year val
		for(int Month = 0; Month < MonthNum; Month++) {
			Serial.print("\t");
			Serial.println(MonthNames[Month]); //Print month val under year header 
			for(int Day = 0; Day < DayNum; Day++) {
				int DayVar = DayVals[Day]; //Don't set date to an impossible day of month
				if(DayVar > MonthDay[Month]) DayVar = MonthDay[Month]; //If 
				DaySim(YearVals[Year], MonthVals[Month], DayVar); //Pass off to single day simulation
			}
		}
	}
	Serial.println("Test Complete...");
	while(1); //Sit and wait once single run completed 
	// if(NewAlarm) {
	// 	Serial.println("Alarm Interrupt!"); //Call out when a hardware interrupt occours 
	// 	NewAlarm = false; //Clear flag
	// }

	// if(rtc.ReadAlarm(0)) {
	// 	Serial.println("Alarm0 - Triggered");
	// 	Serial.print("\tTime Since Last Alarm0:");
	// 	Serial.println(millis() - Alarm0Time);
	// 	Alarm0Time = millis(); //Update time
	// 	rtc.SetAlarm(Period0, 0); //Reset alarm
	// }

	// if(rtc.ReadAlarm(1)) {
	// 	Serial.println("Alarm1 - Triggered");
	// 	Serial.print("\tTime Since Last Alarm1:");
	// 	Serial.println(millis() - Alarm1Time);
	// 	Alarm1Time = millis(); //Update time
	// 	rtc.SetAlarm(Period1, 1); //Reset alarm
	// }

}

//FIX! Add day of week
void DaySim(int Year, int Month, int Day) {
	unsigned long AlarmTime = millis(); //Keep track of how long since last 
	for(int Hour = 0; Hour < HourNum; Hour++) {
		for(int Min = 0; Min < MinNum; Min++) {
			for(int Sec = 0; Sec < SecNum; Sec++) {
				rtc.SetTime(Year, Month, Day, 1, HourVals[Hour], MinVals[Min], SecVals[Sec]); //Set desired test time
				rtc.SetAlarm(Period, 0); //Reset alarm
				AlarmTime = millis(); //Update time of alarm set
				while(!NewAlarm && (millis() - AlarmTime) < Timeout_ms); //Wait for trigger to go off
				// Serial.println(NewAlarm);
				if(!rtc.ReadAlarm(0) || (millis() - AlarmTime) > Timeout_ms || !NewAlarm) { //If alarm not triggered OR timeout has occoured 
					Serial.print("\t\tFAIL: ");
					Serial.println(String(Year) + "-" + String(Month) + "-" + String(Day) + "\t" + String(HourVals[Hour]) + ":" + String(MinVals[Min]) + ":" + String(SecVals[Sec]));
				}
				else {
					Serial.print("\t\tPASS: "); //DEBUG!
					Serial.println(String(Year) + "-" + String(Month) + "-" + String(Day) + "\t" + String(HourVals[Hour]) + ":" + String(MinVals[Min]) + ":" + String(SecVals[Sec])); //DEBUG!
				}
				NewAlarm = false; //Clear everytime 
			}
		}
	}
}


void RTC_Alarm() //ISR for alarm interrupt 
{
	NewAlarm = true; //Set flag
}