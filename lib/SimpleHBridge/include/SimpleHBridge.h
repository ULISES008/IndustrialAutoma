#ifndef _SIMPLEHBRIDGE_H
#define _SIMPLEHBRIDGE_H

#include <SimplePWM.h> // Se utiliza PWM

class SimpleHBridge
{
public:
    // Métodos
    SimpleHBridge();
    ~SimpleHBridge();
    void setup(uint8_t pins[2], uint8_t channel[2], float threshold);
    void setSpeed(float speed_percentage);
private:
    // Variables privadas
    float _threshold;
    float _speed_percentage;
    // Objetos PWM
    SimplePWM IN[2];
};

#endif//