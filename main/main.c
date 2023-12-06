// main.c
// TODO
// Add rotary encoder to the biblography

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "debug/debug.h"
#include "rotary_encoder/rotary_encoder.h"
#include "nvs_manager/nvs_manager.h"

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
    // TEST
    ESP_ERROR_CHECK(nvs_manager_init());
    int value_read = 0;
    esp_err_t ret = nvs_manager_read_int("mode", &value_read);
    ESP_LOGI("MAIN", "Count: %d", value_read);
    vTaskDelay(pdMS_TO_TICKS(10000));

    ESP_LOGI("START", "Rotary encoder config");
    rotary_encoder_t encoder;
    encoder.gpio_a = GPIO_ROTARY_CLK;
    encoder.gpio_b = GPIO_ROTARY_DT;
    ESP_ERROR_CHECK(rotary_encoder_init(&encoder, 4));

    ESP_LOGI("START", "IR sensor config");

    ESP_LOGI("START", "LED config");

    ESP_LOGI("START", "NVS config");
    ESP_ERROR_CHECK(nvs_manager_init());

    ESP_LOGI("MAIN", "Main loop");
    
    while (1) {
        int count;
        rotary_encoder_get_count(&encoder, &count);
        ESP_ERROR_CHECK(nvs_manager_write_int("mode", count));
        int value_read = 0;
        esp_err_t ret = nvs_manager_read_int("mode", &value_read);
        ESP_LOGI("MAIN", "Count: %d", value_read);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

}