#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H


#include <driver/gpio.h>
#include <driver/pulse_cnt.h>
#include <esp_log.h>

#include "../types.h"

#define ROTARY_ENCODER_PCNT_HIGH_LIMIT 5
#define ROTARY_ENCODER_PCNT_LOW_LIMIT  -5

typedef struct {
    gpio_num_t gpio_a;
    gpio_num_t gpio_b;
    gpio_num_t gpio_btn;
    pcnt_unit_handle_t pcnt_unit;
    int step_size;
} rotary_encoder_t;

esp_err_t rotary_encoder_init(rotary_encoder_t *encoder, int step_size);
esp_err_t rotary_encoder_get_count(const rotary_encoder_t *encoder, int *count);
esp_err_t rotary_encoder_setup_button_isr(rotary_encoder_t *encoder, gpio_isr_t isr_handler, pull_mode_t pull_mode, gpio_int_type_t edge_type);

#endif // ROTARY_ENCODER_H
