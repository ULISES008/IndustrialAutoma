#ifndef _SIMPLEHB_H
#define _SIMPLEHB_H

#include <SimplePWM.h> // Se utiliza PWM
#include <SimpleGPIO.h>


class SimpleHB
{
public:
    // Métodos
    SimpleHB();
    ~SimpleHB();
    void setup(uint8_t pins[3], uint8_t channel, float threshold);
    void setSpeed(float speed_percentage);
private:
    // Variables privadas
    float _threshold;
    float _speed_percentage;
    // Objetos PWM
    SimplePWM PWM;
    SimpleGPIO IN[2];
};

#endif//