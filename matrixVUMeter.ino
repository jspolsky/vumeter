/*

LED VU meter for Arduino and Adafruit NeoPixel LEDs.

Requires:
	* 8x32 NeoPixel matrix (https://www.adafruit.com/product/2294)
	* Arduino Uno
	* Adafruit electret microphone (https://www.adafruit.com/product/1063)

Features:
	* Automatic gain control


 Hardware requirements:
 - Most Arduino or Arduino-compatible boards (ATmega 328P or better).
 - Adafruit Electret Microphone Amplifier (ID: 1063)
 - Adafruit Flora RGB Smart Pixels (ID: 1260)
 OR
 - Adafruit NeoPixel Digital LED strip (ID: 1138)
 - Optional: battery for portable use (else power through USB or adapter)
 Software requirements:
 - Adafruit NeoPixel library
 
 Connections:
 - 3.3V to mic amp +
 - GND to mic amp -
 - Analog pin to microphone output (configurable below)
 - Digital pin to LED data input (configurable below)

 See notes in setup() regarding 5V vs. 3.3V boards - there may be an
 extra connection to make and one line of code to enable or disable.
 
 */

#include <SPI.h> 
#include <math.h>

#define HARDWARE_VEST


#ifdef HARDWARE_VEST
#include "VUDisplayClassVest.h"
#endif

#ifdef MATRIX
#include "VUDisplayClassMatrix.h"
#endif


#define MIC_PIN   		A1  	// Microphone is attached to this analog pin



#define SAMPLE_WINDOW   10  	// Sample window for average level, in milliseconds -- try 10.


uint16_t peak = 0;      // Peak level of column; used for falling dots
byte peakCycle = 0;     // Toggles between 0 and 1. To prevent peak falling too fast, it only falls when this is 0

// LOOK YOU GUYS I HAVE AUTOMATIC GAIN CONTROL!
unsigned long millisAdjusted;   // the last time we adjusted the gain. Every second we'll reevaluate
float dbMax;					// max db recorded in the last second
float dbRange;					// current range of dbs which will be scaled to (0..WIDTH)

#ifdef MATRIX
	VUDisplayClassMatrix 
#endif
#ifdef HARDWARE_VEST
	VUDisplayClassVest
#endif

display(Serial);

void setup() 
{

	peak = peakCycle = 0;
	millisAdjusted = millis();
	dbMax = 0.0; dbRange = 2.0;

	Serial.begin(250000);
	display.setup();
	
#ifdef MATRIX
	analogReference(EXTERNAL);  // for 5V Arduinos like Uno, connect the mic to the AREF pin, and leave this on.
								// for 3.3v Arduinos like Flora, Gemma, comment this out.
#endif

}

void loop() 
{
	peakCycle = (peakCycle + 1) % 2;

	unsigned long startMillis = millis();  // Start of sample window

	uint16_t signalMax = 0;
	uint16_t signalMin = 1023;
	uint16_t scaledLevel = 0;

	float db = 0.0;

	// collect data for length of sample window (in mS)
	while (millis() - startMillis < SAMPLE_WINDOW)
	{
		uint16_t sample = analogRead(MIC_PIN);
		if (sample < 1024)  // toss out spurious readings
		{
			if (sample > signalMax)
			{
				signalMax = sample;  // save just the max levels
			}
			else if (sample < signalMin)
			{
				signalMin = sample;  // save just the min levels
			}
		}
	}

	db = dbScale(signalMax, signalMin, 0.5);
	dbMax = max(dbMax, db);
	scaledLevel = min(display.getRange(), (floor((db / dbRange * (float) display.getRange()))));

	// has 1 second elapsed since we last calculated scaling factor?
	if (millisAdjusted + 1000L < millis())
	{
		millisAdjusted = millis();
		dbRange = max(dbMax, 0.3);
		dbMax = 0.0;
	}

	if (scaledLevel > peak)
		peak = scaledLevel;
	else if (peak > 0 && peakCycle == 0)
		peak--;

	display.showMeter(scaledLevel, peak);

}

/*
 *		Calculates decibels based on the peak-to-peak voltage 
 *		Subtracts dbFloor so as to return 0 when it's basically quiet
 */
float dbScale( float signalMax, float signalMin, float dbFloor )
{
	float db = 20.0 * log10( signalMax / signalMin ); 
	float result = db - dbFloor;
	if (result < 0.0)
		result = 0.0;

	return result;
}

