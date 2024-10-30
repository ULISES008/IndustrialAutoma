#include "HBridge.h"
HBridge::HBridge()
{
}
HBridge::~HBridge()
{
}
void HBridge::setup(uint8_t pins[2], uint8_t channel[2], float threshold)
{
    _threshold = threshold;
    IN[0].setup(pins[0], channel[0], 0, LEDC_TIMER_10_BIT);
    IN[1].setup(pins[1], channel[1], 0, LEDC_TIMER_10_BIT);
}
void HBridge::setSpeed(float speed_percentage)
{
    // Map considerando el threshold
    _speed_percentage = ((speed_percentage) * (100 - _threshold)) / 100 + _threshold;
    if (_speed_percentage > 0)
    {
        IN[0].setDuty(_speed_percentage);
        IN[1].setDuty(0);
    }
    else if (_speed_percentage < 0)
    {
        IN[0].setDuty(0);
        IN[1].setDuty(-_speed_percentage);
    }
    else
    {
        IN[0].setDuty(0);
        IN[1].setDuty(0);
    }
}