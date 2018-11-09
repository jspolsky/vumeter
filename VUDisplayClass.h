#include "Arduino.h"


class VUDisplayClass
{

    public:
        VUDisplayClass(Stream& s) : Serial(s) {};
        virtual ~VUDisplayClass() {};

        void doSomething();


    private:
        Stream& Serial;

};
