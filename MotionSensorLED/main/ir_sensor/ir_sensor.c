#include "ir_sensor.h"

ir_sensor_t ir_init(gpio_num_t pin, pull_mode_t pull_mode) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = (pull_mode == PULL_UP) ? 1 : 0,
        .pull_down_en = (pull_mode == PULL_DOWN) ? 1 : 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    ir_sensor_t sensor ;
    sensor.pin = pin;
    sensor.pull_mode = pull_mode;
    return sensor;
}

bool ir_sensor_is_triggered(ir_sensor_t sensor) {
    if (sensor.pull_mode == PULL_UP) {
        return gpio_get_level(sensor.pin) == 0;   
    }
    return gpio_get_level(sensor.pin) == 1;
}

