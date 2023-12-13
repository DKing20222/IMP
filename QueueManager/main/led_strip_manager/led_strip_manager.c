#include "led_strip_manager.h"

led_strip_t led_strip_manager_init(gpio_num_t pin, uint32_t led_num)
{
    led_strip_handle_t led_strip;
    led_strip_config_t strip_config = {
        .strip_gpio_num = pin,
        .max_leds = led_num,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    led_strip_t strip = {
        .led_strip = led_strip,
        .pin = pin,
        .led_num = led_num};

    return strip;
}

void led_strip_manager_display(led_strip_t led_strip, int num)
{
    led_strip_manager_reset_led(led_strip);
    for (int i = 0; i < num; i++)
    {
        if (i < led_strip.led_num / 3)
        {
            led_strip_set_pixel_hsv(led_strip.led_strip, i, 100, 255, 255);
        }
        else if (i < led_strip.led_num * 2 / 3)
        {
            led_strip_set_pixel_hsv(led_strip.led_strip, i, 30, 255, 255);
        }
        else
        {
            led_strip_set_pixel_hsv(led_strip.led_strip, i, 0, 255, 255);
        }
    }
    led_strip_refresh(led_strip.led_strip);
    vTaskDelay(pdMS_TO_TICKS(10));
}

void led_strip_manager_serve(led_strip_t led_strip, int employees, int customers)
{
    // Flash the LED for each employee serving
    led_strip_set_pixel_hsv(led_strip.led_strip, 0, 0, 0, 0); // Turn off the first LED
    led_strip_refresh(led_strip.led_strip);
    vTaskDelay(pdMS_TO_TICKS(50));

    led_strip_set_pixel_hsv(led_strip.led_strip, 0, 100, 255, 255); // Turn on the first LED
    led_strip_refresh(led_strip.led_strip);
    vTaskDelay(pdMS_TO_TICKS(50));

    led_strip_manager_reset_led(led_strip);
    led_strip_manager_display(led_strip, customers);
}

void led_strip_manager_reset_led(led_strip_t led_strip)
{
    led_strip_clear(led_strip.led_strip);
}