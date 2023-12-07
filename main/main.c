// main.c
// TODO
// Add rotary encoder to the biblography

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <esp_sleep.h>

#include "debug/debug.h"
#include "rotary_encoder/rotary_encoder.h"
#include "nvs_manager/nvs_manager.h"
#include "ir_sensor/ir_sensor.h"

#define GPIO_LED
#define LED_NUM 8

#define GPIO_ROTARY_CLK 25
#define GPIO_ROTARY_DT 26
#define GPIO_ROTARY_BTN 27

#define GPIO_IR_SENSOR 14

SemaphoreHandle_t led_mutex;

volatile bool test_flag = false;

static void IRAM_ATTR isr_handler(void* arg) {
    test_flag = true;
}

void app_main(void)
{
    ESP_LOGI("START", "Config");
    monitoring_init();
    // start_digital_monitoring_task(GPIO_ROTARY_BTN, PULL_UP);

    ESP_LOGI("START", "Rotary encoder config");
    rotary_encoder_t encoder;
    encoder.gpio_a = GPIO_ROTARY_CLK;
    encoder.gpio_b = GPIO_ROTARY_DT;
    encoder.gpio_btn = GPIO_ROTARY_BTN;
    ESP_ERROR_CHECK(rotary_encoder_init(&encoder, 4));
    ESP_ERROR_CHECK(rotary_encoder_setup_button_isr(&encoder, isr_handler, PULL_UP, GPIO_INTR_NEGEDGE));

    ESP_LOGI("START", "IR sensor config");
    ir_init(GPIO_IR_SENSOR, PULL_UP);

    ESP_LOGI("START", "LED config");
    led_mutex = xSemaphoreCreateMutex();
    if (led_mutex == NULL)
    {
        ESP_LOGE("LED Mutex", "Failed to create LED mutex");
    }
    int effect = 0;

    ESP_LOGI("MAIN", "Main part");

    while (1)
    {
        if (test_flag) {
            ESP_LOGI("MAIN", "Test flag");
            test_flag = false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}