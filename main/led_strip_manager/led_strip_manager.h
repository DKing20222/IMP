#ifndef LED_STRIP_MANAGER_H
#define LED_STRIP_MANAGER_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>
#include <esp_log.h>

#include "led_strip.h"

typedef struct
{
    led_strip_handle_t led_strip;
    gpio_num_t pin;
    uint32_t led_num;
} led_strip_t;

typedef struct {
    uint8_t h;  // Hue: 0-255
    uint8_t s;  // Saturation: 0-255
    uint8_t v;  // Value (Brightness): 0-255
} hsv_color_t;

#define HSV_RED    (hsv_color_t){0, 255, 255}
#define HSV_GREEN  (hsv_color_t){115, 255, 255}
#define HSV_BLUE   (hsv_color_t){235, 255, 255}
#define HSV_ORANGE (hsv_color_t){5, 255, 255}
#define HSV_NULL   (hsv_color_t){0, 0, 0} 

led_strip_t led_strip_manager_init(gpio_num_t pin, uint32_t led_num);

void led_strip_manager_display_effect(led_strip_t led_strip, int effect);
void led_strip_manager_reset_led(led_strip_t led_strip);

#endif