/*

LED VU meter for Arduino and Adafruit NeoPixel LEDs.

Originally written by Adafruit Industries.  Distributed under the BSD license.
This paragraph must be included in any redistribution.

Rewritten quite a bit by Joel.
Current version supports 8x32 NeoPixel matrix 


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
 
 fscale function:
 Floating Point Autoscale Function V0.1
 Written by Paul Badger 2007
 Modified from code by Greg Shakar
 
 */

#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>

#include "VUDisplayClass.h"

#define PIN 6

#define HEIGHT 8
#define WIDTH 32
#define N_PIXELS (HEIGHT * WIDTH)

#define MIC_PIN   		A1  	// Microphone is attached to this analog pin
#define SAMPLE_WINDOW   10  	// Sample window for average level, in milliseconds -- try 10.

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint16_t peak = 0;      // Peak level of column; used for falling dots
byte peakCycle = 0;     // Toggles between 0 and 1. To prevent peak falling too fast, it only falls when this is 0

// LOOK YOU GUYS I HAVE AUTOMATIC GAIN CONTROL!
unsigned long millisAdjusted;   // the last time we adjusted the gain. Every second we'll reevaluate
float dbMax;					// max db recorded in the last second
float dbRange;					// current range of dbs which will be scaled to (0..WIDTH)

VUDisplayClass display(Serial);

void setup() 
{

	peak = peakCycle = 0;
	millisAdjusted = millis();
	dbMax = 0.0; dbRange = 2.0;

	Serial.begin(250000);
	display.doSomething();

	strip.begin();
	strip.setBrightness(5);
	
	analogReference(EXTERNAL);  // for 5V Arduinos like Uno, connect the mic to the AREF pin, and leave this on.
								// for 3.3v Arduinos like Flora, Gemma, comment this out.
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
	scaledLevel = min(WIDTH, (floor((db / dbRange * (float) WIDTH))));

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

	uint32_t offColor = strip.Color(0,0,0),
		     peakColor = strip.Color(0,0,255);

	for (uint16_t x = 0; x < WIDTH; x++)
	{
		uint32_t positionColor = Wheel(map(x,0,WIDTH,0,255));

		if (x == peak && peak != 0)
			positionColor = peakColor;
		else if (x >= scaledLevel)
			positionColor = offColor;
		
		for (uint16_t y = 0; y < HEIGHT; y++)
		{
				strip.setPixelColor(xy(x,y), positionColor);
		}
	}

	strip.show();
}


uint16_t xy(uint16_t x, uint16_t y)
{
	x = WIDTH - 1 - x;
	if (x & 1) y = HEIGHT - 1 - y;
	return (x*HEIGHT + y);
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


// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
	if(WheelPos < 85) {
		return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	} 
	else if(WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	} 
	else {
		WheelPos -= 170;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
}
