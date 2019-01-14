
#ifndef RELAYER_H
#define RELAYER_H

#include "Arduino.h"

class Relayer
{
public:
    Relayer(int pin);
    ~Relayer();

    void Update();
    bool getState();
    void setSchedule( unsigned long on, unsigned long off);

private:
   // Class Member Variables
	// These are initialized at startup
	int ledPin;      // the number of the LED pin
	unsigned long OnTime;     // milliseconds of on-time
	unsigned long OffTime;    // milliseconds of off-time
 
	// These maintain the current state
	int ledState;             		// ledState used to set the LED
	unsigned long previousMillis;  	// will store last time LED was updated
 
  // Constructor - creates a Flasher 
  // and initializes the member variables and state
};  

#endif
