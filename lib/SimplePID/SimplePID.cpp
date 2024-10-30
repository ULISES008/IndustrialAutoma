#include "SimplePID.h"

PID::PID()
{
}


PID::~PID()
{
}

void PID::setup(float gains[], float dt, float integral_saturation)
{
    _dt = dt;
    _saturation = integral_saturation;
    setGains(gains);
}

void PID::setGains(float gains[])
{
    for (unsigned char i=0; i<3; i++)
        _gains[i] = gains[i];    
}

float PID::calculate(float measurement, float reference)
{
    _error[0] = reference - measurement;
    float control_output = _gains[0] * _error[0];
    if (_gains[1] > 0.0001f)
    {
        _integral += (_dt / 2.0f) * (_error[0] + _error[1]);
        _integral = antiWindup(_integral);
        control_output += _gains[1] *_integral;
    }
    if (_gains[2] > 0.0001f)
        control_output += _gains[2] * ((_error[0] - _error[2])/(2 * _dt));
    for(unsigned char i = 2; i > 0; i--)
        _error[i] = _error[i-1];
    return control_output;
}

float PID::getError()
{
    return _error[0];
}


float PID::antiWindup(float integral)
{
    if (integral > _saturation)
        return  _saturation;  // Limitar el valor de la integral si es mayor a la saturación
    if (integral <  -_saturation)
        return - _saturation;
    return integral;
}