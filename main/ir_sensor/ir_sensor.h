#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include <driver/gpio.h>

#include "../types.h"

typedef struct {
    gpio_num_t pin;
    pull_mode_t pull_mode;
} ir_sensor_t;

ir_sensor_t ir_init(gpio_num_t pin, pull_mode_t pull_mode);
#endif
