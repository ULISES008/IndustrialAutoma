#include "RGB.h"

RGB::RGB()
{
}

void RGB::setup(uint8_t pin[3], uint8_t channel[3], unsigned int invert)
{
    for (uint8_t i = 0; i < 3; i++)
        pwm[i].setup(pin[i], channel[i], invert);
}

void RGB::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    pwm[0].setDigitalLevel(r);
    pwm[1].setDigitalLevel(g);
    pwm[2].setDigitalLevel(b);
}

void RGB::setColor(uint32_t rgb_code)
{
    setColor(rgb_code >> 16 & 0xFF, rgb_code >> 8 & 0xFF, rgb_code & 0xFF); // 0x para que lo condisere hexadecimal
}
