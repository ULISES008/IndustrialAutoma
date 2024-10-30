#include "definitions.h"

extern "C" void app_main()
{
    esp_task_wdt_deinit(); 
    rgb.setup(rgb_pins, rgb_ch, 0);
    ldr[0].setup(ldr_pins[0]);
    ldr[1].setup(ldr_pins[1]);
    prev_time = esp_timer_get_time();
    while (1)
    {
        current_time = esp_timer_get_time();
        if (current_time - prev_time >= dt_us)
        {
            prev_time = current_time;
            switch (color)
            {
            case R:
                measurement[2] = (ldr[0].read(ADC_READ_RAW) + ldr[1].read(ADC_READ_RAW)) / 2;
                rgb.setColor(0xFF0000);
                color = G;
                break;

            case G:
                measurement[0] = (ldr[0].read(ADC_READ_RAW) + ldr[1].read(ADC_READ_RAW)) / 2;
                rgb.setColor(0x00FF00);
                color = B;
                break;

            case B:
                measurement[1] = (ldr[0].read(ADC_READ_RAW) + ldr[1].read(ADC_READ_RAW)) / 2;
                rgb.setColor(0x0000FF);
                color = R;
                message_length = sprintf(buffer,"%d,%d,%d\n", measurement[0],measurement[1],measurement[2]);
                uart.write(buffer,message_length);
                
                break;
            }
        }
        if (uart.available())
        {
            uart.read(buffer,16);
            int time_in;
            sscanf(buffer, "%d", &time_in);
            dt_us = time_in * 1000; 
        }
    }
}