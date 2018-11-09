#include "VUDisplayClassMatrix.h"

void VUDisplayClassMatrix::setup()
{
    
    strip = Adafruit_NeoPixel(N_PIXELS, PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.setBrightness(5);

}

uint16_t VUDisplayClassMatrix::getRange()
{
    return WIDTH;
}

void VUDisplayClassMatrix::showMeter(uint16_t level, uint16_t peak)
{
	uint32_t offColor = strip.Color(0,0,0),
		     peakColor = strip.Color(0,0,255);

	for (uint16_t x = 0; x < WIDTH; x++)
	{
		uint32_t positionColor = Wheel(map(x,0,WIDTH,0,255));

		if (x == peak && peak != 0)
			positionColor = peakColor;
		else if (x >= level)
			positionColor = offColor;
		
		for (uint16_t y = 0; y < HEIGHT; y++)
		{
				strip.setPixelColor(xy(x,y), positionColor);
		}
	}

    strip.show();

}



uint16_t VUDisplayClassMatrix::xy(uint16_t x, uint16_t y)
{
	x = WIDTH - 1 - x;
	if (x & 1) y = HEIGHT - 1 - y;
	return (x*HEIGHT + y);
}





// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t VUDisplayClassMatrix::Wheel(byte WheelPos) {
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