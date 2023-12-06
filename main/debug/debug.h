// debug.h

#ifndef DEBUG_H
#define DEBUG_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <esp_log.h>

typedef enum {
    PULL_NONE = 0,
    PULL_UP,
    PULL_DOWN
} pull_mode_t;

typedef struct {
    gpio_num_t pin;
    pull_mode_t pull_mode;
} digital_monitor_task_args_t;

void monitoring_init();
void start_digital_monitoring_task(gpio_num_t pin, pull_mode_t pull_mode);
void start_analog_monitoring_task(adc1_channel_t channel);

#endif
