// Bibliotecas

#include <SimplePWM.h>
#include <esp_timer.h>
#include <SimpleGPIO.h>
#include <esp_task_wdt.h>



SimpleGPIO button;

gpio_mode_t mode = GPIO_MODE_INPUT_OUTPUT;


// Declaración de objetos
SimplePWM SERVO;

// Variables
int frec = 50;
// Tiempo
uint32_t prev_time, current_time;
uint32_t delta = 10000; // 100 Hz son 10 ms

