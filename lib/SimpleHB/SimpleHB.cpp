#include "SimpleHB.h"

SimpleHB::SimpleHB()
{
}

SimpleHB::~SimpleHB()
{
}

void SimpleHB::setup(uint8_t pins[3], uint8_t channel, float threshold)
{
    _threshold = threshold;
    IN[0].setup(pins[0], GPIO_MODE_OUTPUT);
    IN[1].setup(pins[1], GPIO_MODE_OUTPUT);
    PWM.setup(pins[2], channel, 0, LEDC_TIMER_12_BIT);
}
void SimpleHB::setSpeed(float speed_percentage)
{
    // Map considerando el threshold
    _speed_percentage = ((speed_percentage) * (100 - _threshold) / (100.0)) + _threshold;
    if (_speed_percentage > 0)
    {
        IN[0].set(1);
        IN[1].set(0);
        PWM.setDuty(_speed_percentage);
    }
    else if (_speed_percentage < 0)
    {
        IN[0].set(0);
        IN[1].set(1);
        PWM.setDuty(-_speed_percentage);
    }
    else
    {
        IN[0].set(0);
        IN[1].set(0);
        PWM.setDuty(0);
    }
}