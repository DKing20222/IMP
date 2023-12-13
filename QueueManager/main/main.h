#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_random.h>
#include <esp_system.h>
#include <esp_log.h>

#include "rotary_encoder/rotary_encoder.h"
#include "led_strip_manager/led_strip_manager.h"

// GPIO Definitions
#define GPIO_LED 12
#define LED_NUM 60
#define GPIO_ROTARY_CLK 25
#define GPIO_ROTARY_DT 26
#define GPIO_ROTARY_BTN 27
#define GPIO_IR_SENSOR 14

// Stage enum
typedef enum {
    SET_EMPLOYEE,
    SET_CUSTOMER,
    SERVE
} stage_t;

// Function Declarations
void app_main(void);
void setup_process(void);
void setup_employee_tasks(int employee_count);
void employee_task(void *arg);
void led_task(void *arg);
void rotary_encoder_task(void *arg);
void button_task(void *arg);

// External Variables
extern led_strip_t led_strip;
extern SemaphoreHandle_t led_semaphore;
extern rotary_encoder_t encoder;
extern SemaphoreHandle_t rotary_btn_semaphore;
extern SemaphoreHandle_t semaphore;
extern volatile int employees;
extern volatile int customers;
extern volatile stage_t stage;

#endif // MAIN_H
