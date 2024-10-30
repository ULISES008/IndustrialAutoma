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

// Variables
float speed;

uint8_t motor_pins[3] = {21,23,22};
uint8_t motor_ch = 0;
uint8_t encoder_pins[2] = {18,19};

uint32_t dt_us = 50; // define un intervalo


// buffer
char buffer[64];
int message_length;

int mensaje;
int prueba;


