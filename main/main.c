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
#include "ir_sensor/ir_sensor.h"

#define GPIO_LED
#define LED_NUM 8

#define GPIO_ROTARY_CLK 25
#define GPIO_ROTARY_DT 26
#define GPIO_ROTARY_BTN 27

#define GPIO_IR_SENSOR 14

SemaphoreHandle_t rotary_btn_semaphore;

volatile int effect = 0;
volatile bool is_selection_mode = false;

static void IRAM_ATTR isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(rotary_btn_semaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void button_task(void *arg)
{
    while (1)
    {
        if (xSemaphoreTake(rotary_btn_semaphore, portMAX_DELAY))
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            if (gpio_get_level(GPIO_ROTARY_BTN) == 0)
            {
                is_selection_mode = !is_selection_mode;
                ESP_LOGI("Button Task", "Selection mode: %s", is_selection_mode ? "ON" : "OFF");

                while (gpio_get_level(GPIO_ROTARY_BTN) == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                vTaskDelay(pdMS_TO_TICKS(50));
            }
        }
    }
}

void display_effect(int effect)
{
    switch (effect)
    {
    case 0:
        ESP_LOGI("EFFECT", "OFF");
        break;

    case 1:
        ESP_LOGI("EFFECT", "RAINBOW");
        break;

    default:
        ESP_LOGI("EFFECT", "UNKNOWN");
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void app_main(void)
{
    ESP_LOGI("START", "Config");
    // monitoring_init();
    // start_digital_monitoring_task(GPIO_ROTARY_BTN, PULL_UP);

    ESP_LOGI("START", "Rotary encoder config");
    rotary_encoder_t encoder;
    encoder.gpio_a = GPIO_ROTARY_CLK;
    encoder.gpio_b = GPIO_ROTARY_DT;
    encoder.gpio_btn = GPIO_ROTARY_BTN;
    ESP_ERROR_CHECK(rotary_encoder_init(&encoder, 4));
    ESP_ERROR_CHECK(rotary_encoder_setup_button_isr(&encoder, isr_handler, PULL_UP, GPIO_INTR_NEGEDGE));

    rotary_btn_semaphore = xSemaphoreCreateBinary();
    if (rotary_btn_semaphore == NULL)
    {
        ESP_LOGE("Rotary Button Mutex", "Failed to create mutex");
    }

    ESP_LOGI("START", "IR sensor config");
    ir_sensor_t ir = ir_init(GPIO_IR_SENSOR, PULL_UP);

    ESP_LOGI("START", "LED config");

    ESP_LOGI("MAIN", "Main part");

    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);

    while (1)
    {
        while (1)
        {
            if (is_selection_mode)
            {
                // Mode for changing effect using rotary encoder
                int new_effect;
                rotary_encoder_get_count(&encoder, &new_effect);
                if (new_effect != effect)
                {
                    effect = new_effect;
                    display_effect(effect);
                }
            }
            else
            {
                // Default mode: IR sensor active
                if (ir_sensor_is_triggered(ir))
                {
                    display_effect(effect);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}