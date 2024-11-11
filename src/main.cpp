#include "definitions.h"

extern "C" void app_main()
{
    esp_task_wdt_deinit();
    prev_time = esp_timer_get_time();

    while (1)
    {
        current_time = esp_timer_get_time();
        if (current_time - prev_time >= dt_us)
        {
            prev_time = current_time;
            switch (sLinea)
            {
            case iz2:
                sl.setup(sl_pins[0]);
                measurement[0] = sl.read(ADC_READ_RAW);
                sLinea = iz;
                break;

            case iz:
                sl.setup(sl_pins[1]);
                measurement[1] = sl.read(ADC_READ_RAW);
                sLinea = de;
                break;

            case de:
                sl.setup(sl_pins[2]);
                measurement[2] = sl.read(ADC_READ_RAW);
                sLinea = de2;
                break;

            case de2:
                sl.setup(sl_pins[3]);
                measurement[3] = sl.read(ADC_READ_RAW);
                sLinea = iz2;
                message_length = sprintf(buffer, "%d,%d,%d,%d\n", measurement[0], measurement[1], measurement[2], measurement[3]);
                uart.write(buffer, message_length);
                break;
            }

            // message_length = sprintf(buffer, "%d,%d,%d,%d\n", measurement[0], measurement[1], measurement[2], measurement[3]);
            // uart.write(buffer, message_length);
        }
        /*if (uart.available())
        {
            uart.read(buffer, 16);
            int time_in;
            sscanf(buffer, "%d", &time_in);
            dt_us = time_in * 1000;
        }
        */
    }
}