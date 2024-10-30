#ifndef __SIMPLEPID_H__
#define __SIMPLEPID_H__

#include <driver/gpio.h>
#include <esp_timer.h>
#include <esp_attr.h>

class PID
{   
public:
    PID();  // Constructor
    ~PID(); // Destructor

    // Public methods
    void setup(float gains[], float dt, float integral_saturation);
    void setGains(float gains[]);
    float calculate(float measurement, float reference); // Método para calcular la salida del PID
    float getError();  // Método para obtener el error actual

private:
    float _gains[3];           // Array que almacena Kp, Ki, Kd
    float _error[2];           // Array para almacenar el error actual y el anterior
    float _integral;           // Valor acumulado de la integral
    float _dt;                 // Intervalo de tiempo entre cálculos
    float _saturation;
    // Private helper method
    float antiWindup(float integral); // Método anti-windup para evitar que la integral crezca indefinidamente
};

#endif // __SIMPLEPID_H__
