#include <RGB.h>
#include <SimpleADC.h>
#include <SimpleUART.h>
#include <SimpleGPIO.h>
#include <esp_timer.h>
#include <esp_task_wdt.h>

RGB rgb;
SimpleADC ldr[2];
SimpleUART uart(115200);

uint16_t measurement[3];

uint32_t prev_time, current_time;
uint32_t dt_us = 1000000;

uint8_t rgb_pins[]={14,12,27};
uint8_t rgb_ch[]={0,1,2};

uint8_t ldr_pins[]={32,33};

enum rgb_sequence{
    R,G,B
}color;

char buffer[20];
int message_length; 