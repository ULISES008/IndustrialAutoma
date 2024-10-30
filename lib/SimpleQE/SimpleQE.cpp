#include "SimpleQE.h"

SimpleQE::SimpleQE()
{
}

SimpleQE::~SimpleQE()
{
}

void SimpleQE::setup(uint8_t gpio_num[], float degrees_per_edge, int64_t timeout_us)
{
    _gpio_num[0] = (gpio_num_t)gpio_num[0];
    _gpio_num[1] = (gpio_num_t)gpio_num[1];
    _degrees_per_edge = degrees_per_edge;
    _timeout_us = timeout_us;
    
    // Configure GPIO
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pin_bit_mask = (1ULL << _gpio_num[0]);
    gpio_config(&io_conf);
    
    // ISR setup for Channel A
    gpio_uninstall_isr_service();
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(_gpio_num[0], [](void *arg) { static_cast<SimpleQE *>(arg)->handler(); }, this);
    
    // Disable interrupts for Channel B
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL << _gpio_num[1]);
    gpio_config(&io_conf);
}

float SimpleQE::getAngle()
{
    // Calculate the angle based on the counts and degrees per edge
    return _counts * _degrees_per_edge;  // Modification
}

float SimpleQE::getSpeed()
{
    // Calculate speed (degrees per second) based on the time difference between edges
    if (_delta_micros > 0)
    {
        return _direction*(_degrees_per_edge * 1000000.0) / _delta_micros;  // Speed in degrees per second // Modification
    }
    return 0;
}

int8_t SimpleQE::getDirection()
{
    // Return the direction based on the last calculated value
    return _direction; // Modification
}

void SimpleQE::setAngle(float angle)
{
    // Set the counts based on the given angle
    _counts = angle / _degrees_per_edge;  // Modification
}

void IRAM_ATTR SimpleQE::handler()
{
    int64_t current_micros = esp_timer_get_time(); // Modification
    _delta_micros = current_micros - _prev_micros; // Modification
    _prev_micros = current_micros; // Modification

    // Update channel states and calculate direction
    _states = (_states << 2) & 0x0F; // Modification
    _states |= gpio_get_level(_gpio_num[0]) << 1; // Modification
    _states |= gpio_get_level(_gpio_num[1]); // Modification
    
    // Update direction using lookup table
    _direction = _edge_lut[_states]; // Modification
    
    // Increment or decrement the counts based on direction
    _counts += _direction; // Modification
}
