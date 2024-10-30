#include "definitions.h"
extern "C" void app_main()
{
    // Evitar mensajes no deseados en la consola
    esp_log_level_set("*", ESP_LOG_NONE);
    esp_task_wdt_deinit(); // Se deinicializa el Watchdog timer (alertas)
    // Señal PWM del LED infrarrojo
    LED_IR.setup(GPIO_NUM_32, LEDC_CHANNEL_0, 0, LEDC_TIMER_12_BIT, frec_ir, LEDC_TIMER_0);
    LED_IR.setDuty(50); // 1.65 V
    // Configuración de los tres filtros
    Filtro[0].setup(b1, a1, 3, 3);
    Filtro[1].setup(b2, a2, 3, 3);
    Filtro[2].setup(b3, a3, 4, 1);
    // Configuración ADC
    FT.setup(GPIO_NUM_15, ADC_BITWIDTH_12);
    // Tiempo
    prev_time = esp_timer_get_time();
    // Bluetooth
    bt.begin("TEST");
    while (1)
    {
        // Polling de tiempo transcurrido
        current_time = esp_timer_get_time();
        if (current_time - prev_time >= dt_us)
        {
            prev_time = current_time
            lectura = FT.read(ADC_READ_MV); // Se lee la señal en MV
            // Se aplican tres filtros a la lectura
            for (uint8_t i = 0; i < 3; i++)
                salida[i] = Filtro[i].apply(lectura);

            // bt:
            //int message_length_bt = bt.available();
            // Cambia la frecuencia de muestreo por medio de Bluetooth
            if (bt.available())
            {
                bt.read(buffer, 16);
                // int new_time;
                sscanf(buffer,"%ld,%lld", &frec_ir, &dt_us);
                // dt_us = new_time;
                LED_IR.setFrequency(frec_ir);
                // LED_IR.setup(GPIO_NUM_32, LEDC_CHANNEL_0, 0, LEDC_TIMER_12_BIT, frec_ir, LEDC_TIMER_0);
            }
        }
        // Se escriben valores en la terminal
        message_length = sprintf(buffer, "%.2f,%.2f,%.2f,%.2f\n", lectura, salida[0], salida[1], salida[2]);
        uart.write(buffer, message_length);
    }
}