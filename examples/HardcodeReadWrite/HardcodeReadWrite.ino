#include <MCP7940.h>

#define SET_TIME //Comment out to turn off time set on startup

MCP7940 RTC; //Instatiate RTC 

const unsigned long Period = 5000; //Number of ms to wait between prints 

void setup() {
	Serial.begin(9600); //Setup serial for prints
	Serial.println("Welcome to the hourglass of time..."); 

	RTC.Begin(); //Setup basic functions
	#if defined (SET_TIME)
		RTC.SetTime(2021, 06, 10, 12, 0, 0); //Set arbitrary time (incept date)
	#endif
}

void loop() {
	String DisplayModes[] = {"Scientific", "Civilian", "US", "ISO 8601"}; //Setup array of name tags for display styles
	Serial.println("Current Time:");
	for(int i =0; i < 3; i++) { //Print out all display methods 
		Serial.print(RTC.GetTime(i)); 
		Serial.print("\t");
		Serial.println(DisplayModes[i]);
	}
	Serial.print("\n\n"); //Print return
	delay(Period); //Wait a bit
}