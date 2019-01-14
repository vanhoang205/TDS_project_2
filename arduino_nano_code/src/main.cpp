//*************thu-vien***********************
#include <Arduino.h>                
#include "GravityTDS.h"             
#include <EEPROM.h>                 
#include <DallasTemperature.h>     
#include <OneWire.h>                
#include "DHT.h"                    
#include "Relayer.h"                
//********************************************

//**********KHAI BAO MACRO********************
#define TdsSensorPin A0                     
#define TempSensorPin PD2
#define DHTPIN PD5    
#define DHTTYPE DHT22
#define WATER_SENSOR 3
#define MOTOR_1 9
//********************************************


//**********KHAI BAO BIEN ********************
                   
OneWire oneWire(TempSensorPin);
DallasTemperature tempSensor(&oneWire);


GravityTDS gravityTds;
  
DHT dht(DHTPIN, DHTTYPE);
Relayer motor1(MOTOR_1);

volatile int flow_frequency;    // DO SO XUNG TU CAM BIEN LUU LUONG
unsigned int l_hour;            // LUU GIA TRI LUU LUONG
float temperature = 25,tdsValue = 0, dhtTemp, dhtHum;   

unsigned long cloopTime1000;    // CAP NHAT GIA TRI SAU 1S  
unsigned long cloopTime200;     // CAP NHAT GIA TRI SAU 200MS
bool interruptUpdate = false;   // BIEN CO NGAT SAU 1 MS
bool triggerTime = false;       // true : 6h - 20h  ** false : 20h - 6h

//*****HAM XU LY NGAT************
void flow()
{
	flow_frequency++;
}
//*******************************


void setup()
{

    Serial.begin(115200);
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  
    tempSensor.begin();
    dht.begin();
    pinMode(WATER_SENSOR, INPUT_PULLUP);
	attachInterrupt(1, flow, RISING);           // THIET LAP NGAT CHO KENH 1-PD3
    motor1.setSchedule(15*60*1000,30*60*1000);  // GIA TRI MAC DINH DE BAT TAT MOTOR 15MIN-30MIN
    dhtTemp = dht.readTemperature();            // DOC GIA TRI NHIET DO CHO LAN CHAY DAU TIEN CUA MOTOR
	sei(); // Enable interrupts
	cloopTime1000 = millis();
    cloopTime200 = cloopTime1000;
    OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);

}

SIGNAL(TIMER0_COMPA_vect)
{
	interruptUpdate = true;
}

void loop()
{
  if (interruptUpdate){
    
    if(triggerTime){
        if(dhtTemp > 28)
        motor1.setSchedule(15*60*1000,30*60*1000);
        else
        motor1.setSchedule(15*60*1000,45*60*1000);
    }
    else
    motor1.setSchedule(15*60*1000, 2*60*60*1000);

    motor1.Update();

    //	Every second, calculate and print litres/hour
	if ((millis()-cloopTime1000) > 1000)
	{
		cloopTime1000 = millis(); // Updates cloopTime
		// Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
		l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
		flow_frequency = 0; // Reset Counter
		Serial.print(l_hour, DEC); // Print litres/hour
		Serial.println(" L/hour");
	}

    if ((millis()-cloopTime200)> 200)
	{
		cloopTime200 = millis();
        tempSensor.requestTemperatures();    //add your temperature sensor and read it
        temperature = tempSensor.getTempCByIndex(0);
        gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
        tdsValue = gravityTds.getAverageValue(10);
        dhtTemp = dht.readTemperature();
        dhtHum = dht.readHumidity();

        Serial.print(tdsValue,0);
        Serial.println("ppm");

        Serial.print(temperature,0);
        Serial.println("do c");

        Serial.print(dhtTemp,0);
        Serial.println("do c - moi truong");
        
        Serial.print(dhtHum,0);
        Serial.println("do am - moi truong");

	}
    interruptUpdate = false;
    }
  
}









