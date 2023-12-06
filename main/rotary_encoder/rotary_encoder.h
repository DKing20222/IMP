#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H


#include <driver/gpio.h>
#include <driver/pulse_cnt.h>
#include <esp_log.h>

#define ROTARY_ENCODER_PCNT_HIGH_LIMIT 100
#define ROTARY_ENCODER_PCNT_LOW_LIMIT  -100

typedef struct {
    gpio_num_t gpio_a;
    gpio_num_t gpio_b;
    pcnt_unit_handle_t pcnt_unit;
    int step_size;
} rotary_encoder_t;

esp_err_t rotary_encoder_init(rotary_encoder_t *encoder, int step_size);
esp_err_t rotary_encoder_get_count(const rotary_encoder_t *encoder, int *count);


#endif // ROTARY_ENCODER_H
