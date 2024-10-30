// Header
#include "QuadratureEncoder.h"
// Constructor
QuadratureEncoder::QuadratureEncoder()
{
}

// Destructor
QuadratureEncoder::~QuadratureEncoder()
{
}

void QuadratureEncoder::setup(uint8_t gpio_num[], float degrees_per_edge, int64_t timeout_us)
{
    _gpio_num[0] = (gpio_num_t)gpio_num[0];
    _gpio_num[1] = (gpio_num_t)gpio_num[1];
    _degrees_per_edge = degrees_per_edge;
    _timeout_us = timeout_us;
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pin_bit_mask = (1ULL << _gpio_num[0]);
    gpio_config(&io_conf);
    gpio_uninstall_isr_service();
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(_gpio_num[0], [](void *arg)
                         { static_cast<QuadratureEncoder *>(arg)->handler(); }, this);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL << _gpio_num[1]);
    gpio_config(&io_conf);
}
float QuadratureEncoder::getAngle()
{
    return _counts * _degrees_per_edge;
}
float QuadratureEncoder::getSpeed()
{
    int64_t current_micros = esp_timer_get_time();
    if ((current_micros - _prev_micros) < _timeout_us)
        _speed = _direction * (_degrees_per_edge * 1000000.0) / _delta_micros;
    else
        _speed = 0.0f;
    return _speed;
}
int8_t QuadratureEncoder::getDirection()
{
    return _direction;
}
void QuadratureEncoder::setAngle(float angle)
{
    _counts = angle / _degrees_per_edge;
}
void IRAM_ATTR QuadratureEncoder::handler()
{
    int64_t current_micros = esp_timer_get_time(); // Modification
    _delta_micros = current_micros - _prev_micros; // Modification
    _prev_micros = current_micros;                 // Modification

    // Update channel states and calculate direction
    _states = (_states << 2) & 0x0F;              // Modification
    _states |= gpio_get_level(_gpio_num[0]) << 1; // Modification
    _states |= gpio_get_level(_gpio_num[1]);      // Modification
    // Update direction using lookup table
    _direction = _edge_lut[_states]; // Modification
    // Increment or decrement the counts based on direction
    _counts += _direction;
}
