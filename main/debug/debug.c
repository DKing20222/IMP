// debug.c

#include "debug.h"

static SemaphoreHandle_t print_mutex;
static const char *TAG = "DEBUG";

static void digital_monitor_task(void *arg) {
    digital_monitor_task_args_t *args = (digital_monitor_task_args_t *)arg;
    gpio_num_t pin = args->pin;
    pull_mode_t pull_mode = args->pull_mode;

    // Pin configuration with pull-up/pull-down options
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = (pull_mode == PULL_UP) ? 1 : 0,
        .pull_down_en = (pull_mode == PULL_DOWN) ? 1 : 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    while (1) {
        if (xSemaphoreTake(print_mutex, portMAX_DELAY)) {
            int state = gpio_get_level(pin);
            ESP_LOGI(TAG, "Pin %d: %d", pin, state);
            xSemaphoreGive(print_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    free(args);
}

static void analog_monitor_task(void *arg) {
    adc1_channel_t channel = (adc1_channel_t)arg;

    // ADC configuration
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, ADC_ATTEN_DB_11);

    while (1) {
        if (xSemaphoreTake(print_mutex, portMAX_DELAY)) {
            int value = adc1_get_raw(channel);
            ESP_LOGI(TAG, "ADC Channel %d: %d", channel, value);
            xSemaphoreGive(print_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void start_digital_monitoring_task(gpio_num_t pin, pull_mode_t pull_mode) {
    digital_monitor_task_args_t *args = malloc(sizeof(digital_monitor_task_args_t));
    if (args == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for task arguments");
        return;
    }

    args->pin = pin;
    args->pull_mode = pull_mode;
    xTaskCreate(digital_monitor_task, "DigitalMonitor", 2048, args, 10, NULL);
}

void start_analog_monitoring_task(adc1_channel_t channel) {
    xTaskCreate(analog_monitor_task, "AnalogMonitor", 2048, (void *)channel, 10, NULL);
}

void monitoring_init() {
    print_mutex = xSemaphoreCreateMutex();
    if (print_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create the mutex");
    }
}
