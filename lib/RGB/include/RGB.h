#ifndef __RGB_H__
#define __RGB_H__

#include "SimplePWM.h"

class RGB
{
public:
    RGB();
    void setup(uint8_t pin[3], uint8_t channel[3], unsigned int invert);
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setColor(uint32_t rgb_code);

private:
    SimplePWM pwm[3];
};
#endif // __RGB_H__