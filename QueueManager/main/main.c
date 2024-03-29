#include "main.h"

led_strip_t led_strip;


rotary_encoder_t encoder = {
    .gpio_a = GPIO_ROTARY_CLK,
    .gpio_b = GPIO_ROTARY_DT,
    .gpio_btn = GPIO_ROTARY_BTN,
    .step_size = 4,
    .range = LED_NUM};

SemaphoreHandle_t rotary_btn_semaphore;
SemaphoreHandle_t led_semaphore;
SemaphoreHandle_t semaphore;

volatile int employees = 0;
volatile int customers = 0;
volatile stage_t stage = SET_EMPLOYEE;

bool running = false;

static void IRAM_ATTR isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(rotary_btn_semaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void button_task(void *arg)
{
    ESP_LOGI("START", "Setup button task");
    while (1)
    {
        if (xSemaphoreTake(rotary_btn_semaphore, portMAX_DELAY))
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            if (gpio_get_level(GPIO_ROTARY_BTN) == 0)
            {
                if (stage == SET_EMPLOYEE)
                {
                    running = true;
                    setup_employee_tasks(employees);
                }

                if (stage < SERVE)
                {
                    stage++;
                }
                
                while (gpio_get_level(GPIO_ROTARY_BTN) == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                vTaskDelay(pdMS_TO_TICKS(50));
            }
        }
    }
}

void rotary_encoder_task(void *arg)
{
    ESP_LOGI("START", "Setup rotary encoder task");
    while (1)
    {
        
        switch (stage)
        {
        case SET_EMPLOYEE:
            rotary_encoder_get_count(&encoder, &employees);
            break;

        case SET_CUSTOMER:
            rotary_encoder_get_count(&encoder, &customers);
            break;

        default:
            break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void led_task(void *arg)
{
    ESP_LOGI("START", "Setup led task");
    while (1)
    {
        
        switch (stage)
        {
        case SET_EMPLOYEE:
            led_strip_manager_display(led_strip, employees);
            break;
        case SET_CUSTOMER:
            led_strip_manager_display(led_strip, customers);
            break;
        case SERVE:
            xSemaphoreTake(led_semaphore, portMAX_DELAY);
            if (customers > 0)
            {
                led_strip_manager_serve(led_strip, employees, customers);
            }
            else
            {
                led_strip_manager_reset_led(led_strip);
            }
            xSemaphoreGive(led_semaphore);
            break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void employee_task(void *arg)
{
    ESP_LOGI("START", "Setup employee task %d", (int)arg);
    int employee_id = (int)arg;
    while (1)
    {
        
        if (stage == SERVE)
        {
            xSemaphoreTake(semaphore, portMAX_DELAY);
            if (customers > 0)
            {
                xSemaphoreTake(led_semaphore, portMAX_DELAY);
                customers--;
                ESP_LOGI("Employee", "Employee %d served a customer, remaining %d", employee_id, customers);
                xSemaphoreGive(led_semaphore);
            }
            else
            {
                employees--;
                ESP_LOGI("Employee", "Employee %d leaving, number of customers remaining: %d", employee_id, customers);
                xSemaphoreGive(semaphore);
                vTaskDelete(NULL);
            }
            xSemaphoreGive(semaphore);
            int serveTime = (esp_random() % 5000) + 100;
            vTaskDelay(pdMS_TO_TICKS(serveTime));
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup_employee_tasks(int employee_count)
{
    ESP_LOGI("START", "Setup employee tasks");

    for (int i = 0; i < employee_count; i++)
    {
        xTaskCreate(employee_task, "Employee_Task", 2048, (void *)i, 1, NULL);
    }
}

void setup_process()
{
    ESP_LOGI("START", "Semaphore config");
    semaphore = xSemaphoreCreateMutex();
    if (semaphore == NULL)
    {
        ESP_LOGE("Semaphore", "Failed to create semaphore");
    }

    led_semaphore = xSemaphoreCreateMutex();
    if (led_semaphore == NULL)
    {
        ESP_LOGE("LED Semaphore", "Failed to create semaphore");
    }

    ESP_LOGI("START", "Setup tasks");
    xTaskCreate(rotary_encoder_task, "rotary_encoder_task", 2048, NULL, 1, NULL);
    xTaskCreate(button_task, "button_task", 2048, NULL, 1, NULL);
    xTaskCreate(led_task, "led_task", 4096, NULL, 1, NULL);
}

void app_main(void)
{
    ESP_LOGI("START", "Config");
    ESP_LOGI("START", "LED config");
    led_strip = led_strip_manager_init(GPIO_LED, LED_NUM);

    led_strip_manager_display(led_strip, customers);

    ESP_LOGI("START", "Rotary encoder config");
    ESP_ERROR_CHECK(rotary_encoder_init(&encoder));
    ESP_ERROR_CHECK(rotary_encoder_setup_button_isr(&encoder, isr_handler, PULL_UP, GPIO_INTR_NEGEDGE));

    rotary_btn_semaphore = xSemaphoreCreateBinary();
    if (rotary_btn_semaphore == NULL)
    {
        ESP_LOGE("Rotary Button Semaphore", "Failed to create semaphore");
    }

    ESP_LOGI("START", "Main part");
    setup_process();
    while(1)
    {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        if (running)
        {if ((customers == 0) && (employees == 0))
        {
            running = false;
            stage = SET_EMPLOYEE;
            ESP_LOGI("END", "All customers served and employees left");
        }}
        xSemaphoreGive(semaphore);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}