#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <esp_sleep.h>

#include "debug/debug.h"
#include "rotary_encoder/rotary_encoder.h"
#include "ir_sensor/ir_sensor.h"
#include "led_strip_manager/led_strip_manager.h"

#define GPIO_LED 12
#define LED_NUM 60

#define GPIO_ROTARY_CLK 25
#define GPIO_ROTARY_DT 26
#define GPIO_ROTARY_BTN 27

#define GPIO_IR_SENSOR 14

SemaphoreHandle_t rotary_btn_semaphore;

TaskHandle_t led_task_handle = NULL;

led_strip_t led_strip;

volatile int effect = 0;
volatile bool selection_mode = false;
volatile bool ir_triggered = false;

// Task to handle IR sensor input
void ir_task(void *arg) {
    ir_sensor_t ir = *((ir_sensor_t *)arg);
    while (1) {
        if (!selection_mode && ir_sensor_is_triggered(ir)&& !ir_triggered) {
            ESP_LOGI("IR Task", "IR triggered");
            ir_triggered = true;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Task to handle LED strip effects
void led_task(void *arg) {
    while (1) {
        if (selection_mode)
        {
            led_strip_manager_display_effect(led_strip, effect);
        }
        else
        {
            if (ir_triggered)
            {
                led_strip_manager_display_effect(led_strip, effect);
                ir_triggered = false;
            }
            else
            {
                led_strip_manager_reset_led(led_strip);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void led_reset()
{
    vTaskDelete(led_task_handle);
    led_strip_manager_reset_led(led_strip);
    xTaskCreate(led_task, "LED_TASK", 2048, NULL, 5, &led_task_handle);
}

// ISR handler for the rotary encoder button
static void IRAM_ATTR isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(rotary_btn_semaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Task to handle rotary encoder input
void rotary_encoder_task(void *arg)
{
    rotary_encoder_t encoder = *((rotary_encoder_t *)arg);
    int last_count = 0;
    int new_count;

    while (1)
    {
        rotary_encoder_get_count(&encoder, &new_count);
        if (selection_mode && new_count != last_count)
        {
            last_count = new_count;
            effect = new_count;
            led_reset();
            ESP_LOGI("Rotary Encoder Task", "Effect: %d", effect);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Task to handle button input and debounce it
void button_task(void *arg)
{
    while (1)
    {
        if (xSemaphoreTake(rotary_btn_semaphore, portMAX_DELAY))
        {   
            vTaskDelay(pdMS_TO_TICKS(50));
            if (gpio_get_level(GPIO_ROTARY_BTN) == 0)
            {
                selection_mode = !selection_mode;
                ESP_LOGI("Button Task", "Selection mode: %s", selection_mode ? "ON" : "OFF");
                led_reset();
                while (gpio_get_level(GPIO_ROTARY_BTN) == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                vTaskDelay(pdMS_TO_TICKS(50));
            }
        }
    }
}

// Main application entry point
void app_main(void)
{
    ESP_LOGI("START", "Config");
    // Initialization code

    ESP_LOGI("START", "Rotary encoder config");
    rotary_encoder_t encoder = {
        .gpio_a = GPIO_ROTARY_CLK,
        .gpio_b = GPIO_ROTARY_DT,
        .gpio_btn = GPIO_ROTARY_BTN,
        .step_size = 4,
        .range = 10
        };
    ESP_ERROR_CHECK(rotary_encoder_init(&encoder));
    ESP_ERROR_CHECK(rotary_encoder_setup_button_isr(&encoder, isr_handler, PULL_UP, GPIO_INTR_NEGEDGE));

    rotary_btn_semaphore = xSemaphoreCreateBinary();
    if (rotary_btn_semaphore == NULL)
    {
        ESP_LOGE("Rotary Button Semaphore", "Failed to create semaphore");
    }

    ESP_LOGI("START", "IR sensor config");
    ir_sensor_t ir = ir_init(GPIO_IR_SENSOR, PULL_UP);

    ESP_LOGI("START", "LED config");
    led_strip = led_strip_manager_init(GPIO_LED, LED_NUM);

    ESP_LOGI("START", "Tasks setup");
    xTaskCreate(rotary_encoder_task, "rotary_encoder_task", 2048, &encoder, 10, NULL);
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);
    xTaskCreate(ir_task, "ir_task", 2048, &ir, 10, NULL);
    xTaskCreate(led_task, "led_task", 4096, NULL, 10, &led_task_handle);
}
