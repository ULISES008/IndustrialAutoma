// Header
#include "Filter.h"

// Constructor
Filter::Filter()
{
}
// Destructor
Filter::~Filter()
{
}
// Configuración del filtro (recibe valores)
void Filter::setup(float b[], float a[], unsigned char num_coeff_b, unsigned char num_coeff_a, float x_init, float y_init)
{
    // Arreglo que guarda el número de coeficientes de b y a
    _num_coeff[0] = num_coeff_b;
    _num_coeff[1] = num_coeff_a;
    // Se guardan las b y a en variables privadas
    _b = b;
    _a = a;
    // Se inicializan condiciones iniciales (b se relaciona con x)
    for (unsigned char i = 0; i < num_coeff_b; i++)
    {
        _x[i] = x_init;
    }

    // Se inicializan condiciones iniciales (a se relaciona con y)
    for (unsigned char i = 0; i < num_coeff_a; i++)
    {
        _y[i] = y_init;
    }
}

// Se aplica el filtro a la señal de entrada
// Se utiliza la siguiente ecuación en diferencias:
// a0y[n] = b0x[n] + b1x[n-1] + ... - a1y[n-1] - a2y[n-2]
// a0 generalmente es 1
float Filter::apply(float input_data)
{
    // Se desplazan los datos anteriores de x (que se relaciona con b)
    for (unsigned char i = _num_coeff[0] - 1; i > 0; i--)
        _x[i] = _x[i - 1];
    // Se guarda el valor recién leído
    _x[0] = input_data;
    // Se desplazan los datos anteriores de y (que se relaciona con a)
    for (unsigned char i = _num_coeff[1] - 1; i > 0; i--)
        _y[i] = _y[i - 1];
    // Se crea e inicializa en 0 la variable de salida del filtro
    float output_data = 0.0;
    // Cálculo para las b (se suman)
    for (unsigned char i = 0; i < _num_coeff[0]; i++)
        output_data += _b[i] * _x[i];
    // Cálculo para las a (se restan)
    // No se incluye a0, por eso el ciclo empieza en 1
    for (unsigned char i = 1; i < _num_coeff[1]; i++)
        output_data -= _a[i] * _y[i];
    // Salida es el valor actual de y
    _y[0] = output_data / _a[0];
    // Se regresa la salida
    return output_data;
}