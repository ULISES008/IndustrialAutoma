#include "definitions.h"

static void timerInterrupt(void *arg)
{
    timer.setInterrupt();
} 
extern "C" void app_main()
{
    esp_task_wdt_deinit();

    // Señal PWM
    SERVO.setup(GPIO_NUM_5, LEDC_CHANNEL_1, 0, LEDC_TIMER_12_BIT, 50, LEDC_TIMER_0);
    SERVO.setDuty(6);
    //2-12

    motor.setup(motor_pins, motor_ch, 2.4);
    encoder.setup(encoder_pins, 0.333);
    timer.setup(timerInterrupt, "timer");
    timer.startPeriodic(dt_us);
    
    bt.begin("Batman");
    while(1)
    {
        
        if(timer.interruptAvailable())
        {
            float speed = encoder.getSpeed();
            motor.setSpeed(mensaje);
            message_length = sprintf(buffer, "%.2f\n", speed);
            uart.write(buffer, message_length);
        }  

        if (bt.available())
        {

            bt.read(buffer, 8);
            sscanf(buffer, "%d", &mensaje);
            //uart.write(buffer, prueba);
            //sprintf(buffer, "%d,%d\n", mensaje, prueba);
            //message_length = sprintf(buffer, "%f\n", mensaje);
        }
        SERVO.setDuty(10);
    }
}
