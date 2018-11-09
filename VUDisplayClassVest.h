#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN 1 
#define N_PIXELS 60


class VUDisplayClassVest
{

    public:
        VUDisplayClassVest(Stream& s) : Serial(s) {};
        virtual ~VUDisplayClassVest() {};

        void setup();
        uint16_t getRange();

        void showMeter(uint16_t level, uint16_t peak);

    private:
        Stream& Serial;

        uint32_t Wheel(byte WheelPos);
    
    public:    
        Adafruit_NeoPixel strip;

};
