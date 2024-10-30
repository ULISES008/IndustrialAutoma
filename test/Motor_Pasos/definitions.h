
#include <stdio.h>
#include <esp_timer.h>
#include <SimpleGPIO.h>
#include <esp_task_wdt.h>
#include <SimpleADC.h>
#include <SimpleUART.h>
#include <SimplePWM.h>
#include <SimpleLCD.h>

uint8_t lcd_pins[] = {15, 2, 4, 16, 17, 5, 18, 19, 12, 14, 27};

SimpleUART uart(115200);
SimplePWM led;
SimpleADC pot;
SimpleGPIO button;
SimpleGPIO lcd_pin[11];
SimpleLCD lcd;

int64_t current_time, prev_time;
int64_t dt_us = 50000;

uint8_t pin = 15;
gpio_mode_t mode = GPIO_MODE_INPUT_OUTPUT;
char message[32];

float duty;
uint32_t freq;
int dato;
int32_t count = 0;