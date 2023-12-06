// main.c
// TODO
// Add rotary encoder to the biblography

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "debug/debug.h"
#include "rotary_encoder/rotary_encoder.h"

#define GPIO_LED
#define LED_NUM 8

#define GPIO_ROTARY_CLK 25
#define GPIO_ROTARY_DT 26
#define GPIO_ROTARY_BTN 27

#define GPIO_IR_SENSOR

void app_main(void)
{
    ESP_LOGI("START", "Config");
    monitoring_init();

    ESP_LOGI("START", "Rotary encoder config");
    rotary_encoder_t encoder;
    encoder.gpio_a = GPIO_ROTARY_CLK;
    encoder.gpio_b = GPIO_ROTARY_DT;
    ESP_ERROR_CHECK(rotary_encoder_init(&encoder, 4));

    ESP_LOGI("START", "IR sensor config");

    ESP_LOGI("START", "LED config");

    ESP_LOGI("MAIN", "Main loop");
    while (1)
    {
        int count;
        rotary_encoder_get_count(&encoder, &count);
        ESP_LOGI("MAIN", "Count: %d", count);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}