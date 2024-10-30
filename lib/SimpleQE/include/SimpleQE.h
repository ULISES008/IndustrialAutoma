#ifndef __SIMPLEQE_H__
#define __SIMPLEQE_H__

#include <driver/gpio.h>
#include <esp_timer.h>
#include <esp_attr.h>

class SimpleQE
{
public:
    SimpleQE();  // Constructor
    ~SimpleQE(); // Destructor

    // Public methods
    void setup(uint8_t gpio_num[], float degrees_per_edge, int64_t timeout_us = 100000);
    float getAngle();   // Get angular displacement
    float getSpeed();   // Get motor speed
    int8_t getDirection(); // Get motor rotation direction
    void setAngle(float angle); // Set angle manually

private:
    // Private interrupt handler
    void IRAM_ATTR handler();

    // Private variables
    gpio_num_t _gpio_num[2];     // GPIO pins for the quadrature encoder
    float _degrees_per_edge;     // Degrees per signal edge
    float _speed;       // Speed of motor (degrees/second)
    volatile int64_t _counts;    // Counter for tracking edges
    volatile int64_t _timeout_us; // Timeout for signal updates
    volatile int64_t _prev_micros; // Time of last signal edge
    volatile int64_t _delta_micros; // Time difference between two edges
    volatile uint8_t _states;    // Current state of the quadrature encoder channels
    volatile int8_t _direction;  // Direction of rotation (-1, 0, 1)

    // Lookup table for edge transitions
    const int8_t _edge_lut[16] = {0, 0, 0, -1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 0};
};

#endif // __SIMPLE_QE_H__
