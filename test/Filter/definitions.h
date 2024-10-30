// Bibliotecas

#include <Filter.h>
#include <SimpleADC.h>
#include <SimpleUART.h>
#include <SimplePWM.h>
#include <esp_timer.h>
#include <esp_task_wdt.h>
#include <esp_log.h>
#include <SimpleSerialBT.h>

// Declaración de objetos

SimplePWM LED_IR;        // Señal del LED infrarrojo
SimpleADC FT;            // Lectura del fototransistor
SimpleUART uart(115200); // Transmisión de datos a LabVIEW
Filter Filtro[3];        // Objeto con tres filtros
SerialBT bt;             // Objeto bluetooth

// Variables
// Frecuencia LED infrarrojo
int frec_ir = 4;
// Entrada de señal y salidas de los filtros
float lectura;
float salida[3]; // Son 3 filtros
// Coeficientes filtro 1 (LPF)
float b1[] = {0.0461, 0.0923, 0.0461};
float a1[] = {1, -1.3073, 0.4918};
// Coeficientes filtro 2 (HPF)
float b2[] = {1, -2, 1};
float a2[] = {1, -1.8264, 0.834};
// Coeficientes filtro 3 (Media móvil 4 muestras)
float b3[] = {0.25, 0.25, 0.25, 0.25};
float a3[] = {1};
char buffer[40];    // Buffer para escribir el mensaje a LabVIEW
int message_length; // Longitud del mensaje a enviar
// Tiempo
uint32_t prev_time, current_time;
uint32_t dt_us = 10000; // 100 Hz son 10 ms
// Bluetooth
char buffer[16]; // Buffer relacionado al tiempo (bluetooth)