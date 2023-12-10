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

// Effect helpers

void breath_in(led_strip_t led_strip, hsv_color_t color)
{
    for (int brightness = 0; brightness <= color.v; brightness++)
    {
        for (int i = 0; i < led_strip.led_num; i++)
        {
            led_strip_set_pixel_hsv(led_strip.led_strip, i, color.h, color.s, brightness);
        }
        led_strip_refresh(led_strip.led_strip);
        vTaskDelay(pdMS_TO_TICKS(25));
    }
}

void breath_out(led_strip_t led_strip, hsv_color_t color)
{
    for (int brightness = color.v; brightness >= 0; brightness--)
    {
        for (int i = 0; i < led_strip.led_num; i++)
        {
            led_strip_set_pixel_hsv(led_strip.led_strip, i, color.h, color.s, brightness);
        }
        led_strip_refresh(led_strip.led_strip);
        vTaskDelay(pdMS_TO_TICKS(25));
    }
}

void chase(led_strip_t led_strip, hsv_color_t color, hsv_color_t color2, int length)
{
    const int total_cycles = led_strip.led_num + length;

    for (int cycle = 0; cycle < total_cycles; cycle++) {
        for (int i = 0; i < led_strip.led_num; i++) {
            if (i >= cycle - length && i < cycle) {
                led_strip_set_pixel_hsv(led_strip.led_strip, i, color.h, color.s, color.v);
            } else {
                led_strip_set_pixel_hsv(led_strip.led_strip, i, color2.h, color2.s, color2.v);
            }
        }
        led_strip_refresh(led_strip.led_strip);
        vTaskDelay(pdMS_TO_TICKS(35));
    }
}
// Effects
void effect_basic(led_strip_t led_strip)
{
    for (int i = 0; i < led_strip.led_num; i++)
    {
        led_strip_set_pixel(led_strip.led_strip, i, 225, 225, 225);
    }
    led_strip_refresh(led_strip.led_strip);
    vTaskDelay(pdMS_TO_TICKS(3000));
}

void effect_rainbow(led_strip_t led_strip)
{
    for (int i = 0; i < led_strip.led_num; i++)
    {
        led_strip_set_pixel_hsv(led_strip.led_strip, i, i * 255 / led_strip.led_num, 255, 255);
        led_strip_refresh(led_strip.led_strip);
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    for (int i = 0; i < led_strip.led_num; i++)
    {
        led_strip_set_pixel_hsv(led_strip.led_strip, i, 0,0,0);
        led_strip_refresh(led_strip.led_strip);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// Portal effects
void effect_portal_blue(led_strip_t led_strip)
{
    hsv_color_t color = (hsv_color_t){HSV_BLUE.h, HSV_BLUE.s, 50};
    breath_in(led_strip, color);
    chase(led_strip, HSV_BLUE, color, 10);
    breath_out(led_strip, color);
}

void effect_portal_orange(led_strip_t led_strip)
{
    hsv_color_t color = (hsv_color_t){HSV_ORANGE.h, HSV_ORANGE.s, 50};
    breath_in(led_strip, color);
    chase(led_strip, HSV_ORANGE, color, 10);
    breath_out(led_strip, color);
}

// Pulse effects
void effect_chase_red(led_strip_t led_strip, int length) {
    chase(led_strip, HSV_RED, HSV_NULL, length);
}

void effect_chase_green(led_strip_t led_strip, int length) {
    chase(led_strip, HSV_GREEN, HSV_NULL, length);
}

void effect_chase_blue(led_strip_t led_strip, int length) {
    chase(led_strip, HSV_BLUE, HSV_NULL, length);
}

// Breath effects
void effect_breath_red(led_strip_t led_strip)
{
    breath_in(led_strip, HSV_RED);
    breath_out(led_strip, HSV_RED);
}

void effect_breath_green(led_strip_t led_strip)
{
    breath_in(led_strip, HSV_GREEN);
    breath_out(led_strip, HSV_GREEN);
}

void effect_breath_blue(led_strip_t led_strip)
{
    breath_in(led_strip, HSV_BLUE);
    breath_out(led_strip, HSV_BLUE);
}

void led_strip_manager_display_effect(led_strip_t led_strip, int effect)
{
    switch (effect)
    {
    case 0:
        effect_basic(led_strip);
        break;

    case 1:
        effect_rainbow(led_strip);
        break;

    case 2:
        effect_portal_blue(led_strip);
        break;

    case 3:
        effect_portal_orange(led_strip);
        break;

    case 4:
        effect_chase_red(led_strip, 10);
        break;

    case 5:
        effect_chase_green(led_strip, 10);
        break;

    case 6:
        effect_chase_blue(led_strip, 10);
        break;

    case 7:
        effect_breath_red(led_strip);
        break;

    case 8:
        effect_breath_green(led_strip);
        break;

    case 9:
        effect_breath_blue(led_strip);
        break;

    default:
        break;
    }
    led_strip_refresh(led_strip.led_strip);
    vTaskDelay(pdMS_TO_TICKS(10));
}

void led_strip_manager_reset_led(led_strip_t led_strip)
{
    led_strip_clear(led_strip.led_strip);
}