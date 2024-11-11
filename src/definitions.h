#include <RGB.h>
#include <SimpleADC.h>
#include <SimpleUART.h>
#include <SimpleGPIO.h>
#include <esp_timer.h>
#include <esp_task_wdt.h>
 
SimpleADC sl; 
SimpleUART uart(115200);

uint16_t measurement[4];

uint32_t prev_time, current_time;
uint32_t dt_us = 1000000;

uint8_t sli2;
uint8_t sli;
uint8_t sld;
uint8_t sld2;
uint8_t sl_pins[]={32,35,34,36};

char buffer[60];
int message_length;

enum sl_sequence{
    iz2,iz,de,de2
}sLinea;