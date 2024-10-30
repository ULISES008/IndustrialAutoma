#include "definitions.h"
//Uli prueba

static void timerInterrupt(void *arg)
{
    timer.setInterrupt();
}

int dly(int dt, int pt = esp_timer_get_time())
{
    int ct = esp_timer_get_time();
    while (ct - pt < dt)
    {
        ct = esp_timer_get_time();
    }
    pt = ct;
    return 0;
}

int impuls()
{
    trig.set(0);
    dly(10, esp_timer_get_time());
    trig.set(1);
    dly(10, esp_timer_get_time());
    trig.set(0);
    return 0;
}

extern "C" void app_main()
{
    esp_task_wdt_deinit();

    // Señal PWM
    SERVO.setup(GPIO_NUM_5, LEDC_CHANNEL_1, 0, LEDC_TIMER_12_BIT, 50, LEDC_TIMER_0);
    SERVO.setDuty(6);
    motor.setup(motor_pins, motor_ch, 2.4);
    encoder.setup(encoder_pins, 0.333);
    timer.setup(timerInterrupt, "timer");
    timer.startPeriodic(dt_us);
    bt.begin("Batman");

    trig.setup(GPIO_NUM_32, GPIO_MODE_OUTPUT, GPIO_PULLUP_PULLDOWN);
    echo.setup(GPIO_NUM_33, GPIO_MODE_INPUT, GPIO_PULLUP_PULLDOWN);
    trig.set(0);

    while (1)
    {
        current_time = esp_timer_get_time();

        if (bt.available())
        {
            bt.read(buffer, 17);
            sscanf(buffer, "[%f, %f, %f]", &ang, &vel, &valor);
            /*printf(buffer);
            printf("\n%f", ang);
            printf("\n%f", vel);*/

            ang2 = -(((ang) * 10) / 250) + 12;

            SERVO.setDuty(ang2);
            if(valor==1){
                motor.setSpeed(-vel);

            }
            else{
                motor.setSpeed(vel);
            }
        }
        if(timer.interruptAvailable())
        {
            float speed = encoder.getSpeed();
            //printf("\n%f", speed);
        }

        // Trig
        if (current_time - prev_time >= dt_us)
        {
            impuls();
        }
        // Echo
        if (echo.get() == 0)
        {
            start_time = esp_timer_get_time();
            end_time = start_time;
        }
        // Medir el tiempo mientras el pin Echo esté en alto (duración del pulso)
        else if (echo.get() == 1)
        {
            end_time = esp_timer_get_time();
            prev_time = esp_timer_get_time();
            time = end_time - start_time;
            dis = (time * .034) / 2;
            n = n+1;
        }
        if (n > 1000)
        {
            n = 0;
            
            printf("\n%f", dis);
            if (dis>22 && dis<32 && d<32)
            {
                printf("/////////////////////////////////////////////");
                bt.write("Alto", 5);
                motor.setSpeed(0);
                dly(1000000);
            }
            d=dis;
        }
    }
}
