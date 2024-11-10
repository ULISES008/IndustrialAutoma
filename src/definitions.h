// Bibliotecas


#include <SimpleADC.h>
#include <esp_timer.h>
#include <esp_task_wdt.h>
#include <esp_log.h>

// Declaración de objetos
SimpleADC FT;            // Lectura del fototransistor

// Variables
// Entrada de señal
float lectura;
int message_length; // Longitud del mensaje a enviar
// Tiempo
uint32_t prev_time, current_time;
uint32_t dt_us = 10000; // 100 Hz son 10 ms