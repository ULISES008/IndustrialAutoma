#include "definitions.h"

static void IRAM_ATTR handleButton(void *arg)
{
    button.setInterruptFlag();
}

#define GPIO_BUTTON_1 16
#define estados 4

SimpleGPIO motor_pins[4];
uint8_t pins[] = {5, 19, 18, 21};
uint8_t secuencia[] = {9, 3, 6, 12};
int8_t p = 0;
int8_t a = -1;

extern "C" void app_main()
{


    for (int i = 0; i < 4; i++)
    {
        motor_pins[i].setup(pins[i], GPO);
    }

    uint64_t delta = 5000; // 50000 uS
    esp_task_wdt_deinit();

    button.setup(GPIO_BUTTON_1, GPI);
    button.addInterrupt(GPIO_INTR_NEGEDGE, handleButton, 30000);

    prev_time = esp_timer_get_time();
    while (1)
    {
        
        // accuracy of 1 us,
        current_time = esp_timer_get_time();
        if (current_time - prev_time >= delta)
        {
            if (p > 3)
            {
                p = 0;
            }
            else if (p < 0)
            {
                p = 3;
            }

            for (int pin = 0; pin < estados; pin++)
            {
                if (secuencia[p] & 1 << pin)
                {
                    motor_pins[pin].set(1);
                }
                else
                {
                    motor_pins[pin].set(0);
                }
            }
            p = p + a;
            prev_time = current_time;
            if (button.interruptAvailable())
            {
                int message_length = sprintf(message, "Pressed button at pin %d\n", GPIO_BUTTON_1);
                uart.write(message, message_length);
                a = a * (-1);
            }
        }
    }
}