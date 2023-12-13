#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdlib.h>
#include <driver/gpio.h>
#include <driver/pulse_cnt.h>
#include <esp_log.h>

#include "../types.h"


typedef struct {
    gpio_num_t gpio_a;
    gpio_num_t gpio_b;
    gpio_num_t gpio_btn;
    pcnt_unit_handle_t pcnt_unit;
    int step_size;
    int range;
} rotary_encoder_t;

esp_err_t rotary_encoder_init(rotary_encoder_t *encoder);
esp_err_t rotary_encoder_get_count(rotary_encoder_t *encoder, int *count);
esp_err_t rotary_encoder_setup_button_isr(rotary_encoder_t *encoder, gpio_isr_t isr_handler, pull_mode_t pull_mode, gpio_int_type_t edge_type);

#endif
