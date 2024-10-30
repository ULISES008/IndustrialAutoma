// Incluir librerias.
#include <SimpleADC.h>    //Conversion analogica a digital
#include <SimpleUART.h>   // Funciones UART
#include <esp_timer.h>    // temporizador
#include <esp_task_wdt.h> //Vigilancia de tareas
#include <esp_log.h>      //Registro de eventos
#include <SimpleQE.h>
#include <driver/gpio.h>
#include <esp_attr.h>
#include <stdio.h>
#include <SimpleHB.h>
#include <SimplePID.h>
#include <SimpleTimer.h>
#include <SimplePWM.h>
#include <SimpleSerialBT.h>


SerialBT bt;
SimpleUART uart(115200);



// Declaración de objetos
SimplePWM SERVO;


SimpleHB motor;
SimpleQE encoder;
SimpleTimer timer; 


SimpleGPIO trig;
SimpleGPIO echo; 


// Variables
float speed;

uint8_t motor_pins[3] = {21,23,22};
uint8_t motor_ch = 0;
uint8_t encoder_pins[2] = {18,19};
uint32_t dt_us = 100; // define un intervalo
uint32_t start_time, end_time, time, prev_time, current_time;


// buffer
char buffer[64];
int message_length;

float valor;
float ang;
float ang2;
float vel;

int x;
float dis;
int n;
float d;


