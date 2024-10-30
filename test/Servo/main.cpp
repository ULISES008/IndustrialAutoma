#include "definitions.h"

static void IRAM_ATTR handleButton(void *arg)
{
    button.setInterruptFlag();
}

#define GPIO_BUTTON_1 18

extern "C" void app_main()
{
    esp_task_wdt_deinit();

    button.setup(GPIO_BUTTON_1, GPI);
    button.addInterrupt(GPIO_INTR_NEGEDGE, handleButton, 3000);

    // Señal PWM
    SERVO.setup(GPIO_NUM_22, LEDC_CHANNEL_0, 0, LEDC_TIMER_12_BIT, frec, LEDC_TIMER_0);
    // SERVO.setDuty(serv);

    // Tiempo
    // prev_time = esp_timer_get_time();
    while (1)
    {
        current_time = esp_timer_get_time();
        if (current_time - prev_time >= delta)
        {
            prev_time = current_time;
            if (button.interruptAvailable())
            {
                SERVO.setDuty(10);
                //vTaskDelay(3000);
            }
            
        }
    }
}