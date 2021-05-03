#include <MCP7940.h>

#define ALARM_PIN 2 //Pin used for alarm interrupt, must be an interrupt compattible pin! 
//See https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/

MCP7940 rtc; //Instantiate RTC instance 
volatile bool NewAlarm = false; //Flag to indicate a new interrupt, cleared in loop, set in ISR
const unsigned long Period0 = 2; //Period for alarm0 [seconds]
const unsigned long Period1 = 33; //Period for alarm1 [seconds]

void setup() {
	Serial.begin(115200); //Begin serial for reporting
	while(!Serial); //Wait for serial to begin, required for some systems

	Serial.println("Begin Setup...");
	Serial.println(rtc.Begin()); //Inialize rtc //FIX! Test?? //DEBUG! Remove print
	// rtc.SetTime(2049, 12, 15, 1, 8, 15, 30); //FIX! Set via serial //Force time set for testing 
	rtc.SetTime(2020, 3, 31, 1, 0, 59, 59); //FIX! Set via serial //Force time set for testing 
	Serial.print("Current Time: ");
	Serial.println(rtc.GetTime(MCP7940::Format::ISO_8601)); //Print ISO formatted time
	Serial.print("Alarm0 Period: ");
	Serial.print(Period0);
	Serial.print("s\n");
	Serial.print("Alarm1 Period: ");
	Serial.print(Period1);
	Serial.print("s\n");
	Serial.println("...End Setup"); 

	attachInterrupt(digitalPinToInterrupt(ALARM_PIN), RTC_Alarm, FALLING); //Attach the interrupt for the alarm, trigger on a falling edge

	rtc.SetAlarm(Period0, 0); //Setup alarm0
	// rtc.SetAlarm(Period1, 1); //Setup alarm1
}

void loop() {
	static unsigned long Alarm0Time = millis(); //Keep track of how long since last alarm0
	static unsigned long Alarm1Time = millis(); //Keep track of how long since last alarm1

	if(NewAlarm) {
		Serial.println("Alarm Interrupt!"); //Call out when a hardware interrupt occours 
		NewAlarm = false; //Clear flag
	}

	if(rtc.ReadAlarm(0)) {
		Serial.println("Alarm0 - Triggered");
		Serial.print("\tTime Since Last Alarm0:");
		Serial.println(millis() - Alarm0Time);
		Alarm0Time = millis(); //Update time
		rtc.SetAlarm(Period0, 0); //Reset alarm
	}

	if(rtc.ReadAlarm(1)) {
		Serial.println("Alarm1 - Triggered");
		Serial.print("\tTime Since Last Alarm1:");
		Serial.println(millis() - Alarm1Time);
		Alarm1Time = millis(); //Update time
		rtc.SetAlarm(Period1, 1); //Reset alarm
	}

}

void RTC_Alarm() //ISR for alarm interrupt 
{
	NewAlarm = true; //Set flag
}