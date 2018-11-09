#include "VUDisplayClassVest.h"

void VUDisplayClassVest::setup()
{
    
    strip = Adafruit_NeoPixel(N_PIXELS, PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.setBrightness(30);

}

uint16_t VUDisplayClassVest::getRange()
{
    return N_PIXELS;
}

void VUDisplayClassVest::showMeter(uint16_t level, uint16_t peak)
{
	uint32_t offColor = strip.Color(0,0,0),
		     peakColor = strip.Color(0,0,255);

	for (uint16_t x = 0; x < N_PIXELS; x++)
	{
		uint32_t positionColor = Wheel(map(x,0,getRange(),0,255));

		if (x == peak && peak != 0)
			positionColor = peakColor;
		else if (x >= level)
			positionColor = offColor;
		
		strip.setPixelColor(x, positionColor);
	}

    strip.show();

}



// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t VUDisplayClassVest::Wheel(byte WheelPos) {
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
