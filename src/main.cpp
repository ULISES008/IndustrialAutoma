#include "definitions.h"
extern "C" void app_main()
{
    // Configuración ADC
    FT.setup(34, ADC_BITWIDTH_12);
    // Tiempo
    prev_time = esp_timer_get_time();
    while (1)
    {
        // Polling de tiempo transcurrido
        current_time = esp_timer_get_time();
        if (current_time - prev_time >= dt_us)
        {
            prev_time = current_time;
            lectura = FT.read(ADC_READ_MV); // Se lee la señal en MV
        }
        
        // Se escriben valores en la terminal
        printf("%.2f\n", lectura);
    }
}