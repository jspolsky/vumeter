#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define HEIGHT 8
#define WIDTH 32
#define N_PIXELS (HEIGHT * WIDTH)


class VUDisplayClass
{

    public:
        VUDisplayClass(Stream& s) : Serial(s) {};
        virtual ~VUDisplayClass() {};

        void setup();
        uint16_t getRange();

        void showMeter(uint16_t level, uint16_t peak);

    private:
        Stream& Serial;

        uint16_t xy(uint16_t x, uint16_t y);
        uint32_t Wheel(byte WheelPos);
    
    public:    
        Adafruit_NeoPixel strip;

};
