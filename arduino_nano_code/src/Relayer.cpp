#include"Relayer.h"
#include<Arduino.h>

Relayer::~Relayer()
{
}

Relayer::Relayer(int pin)
{
	ledPin = pin;
	pinMode(ledPin, OUTPUT);     
	
	ledState = LOW; 
	previousMillis = 0;
}

void Relayer::setSchedule( unsigned long on, unsigned long off){
    this->OnTime = on;
    this->OffTime = off;
}

bool Relayer::getState(){
    return digitalRead(this->ledPin);
}

void Relayer::Update(){
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
     
    if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
    {
        ledState = LOW;  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);  // Update the actual LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
    {
      ledState = HIGH;  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(ledPin, ledState);	  // Update the actual LED
    }
}

