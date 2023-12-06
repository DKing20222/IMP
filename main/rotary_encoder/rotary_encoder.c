#include "rotary_encoder.h"
#include "esp_log.h"

static const char *TAG = "ROTARY_ENCODER";

esp_err_t rotary_encoder_init(rotary_encoder_t *encoder, int step_size) {
    ESP_LOGI(TAG, "Initializing rotary encoder");

    encoder->step_size = step_size;

    // PCNT unit configuration
    pcnt_unit_config_t unit_config = {
        .high_limit = ROTARY_ENCODER_PCNT_HIGH_LIMIT,
        .low_limit = ROTARY_ENCODER_PCNT_LOW_LIMIT,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &(encoder->pcnt_unit)));

    // Set glitch filter
    pcnt_glitch_filter_config_t filter_config = {.max_glitch_ns = 1000};
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(encoder->pcnt_unit, &filter_config));

    // PCNT channel configurations for both GPIOs
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = encoder->gpio_a,
        .level_gpio_num = encoder->gpio_b,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(encoder->pcnt_unit, &chan_a_config, &pcnt_chan_a));

    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = encoder->gpio_b,
        .level_gpio_num = encoder->gpio_a,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(encoder->pcnt_unit, &chan_b_config, &pcnt_chan_b));

    // Set edge and level actions for channels
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    // Enable and start PCNT unit
    ESP_ERROR_CHECK(pcnt_unit_enable(encoder->pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(encoder->pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(encoder->pcnt_unit));

    return ESP_OK;
}

esp_err_t rotary_encoder_get_count(const rotary_encoder_t *encoder, int *count) {
    int raw_count;
    esp_err_t err = pcnt_unit_get_count(encoder->pcnt_unit, &raw_count);
    if (err != ESP_OK) {
        return err;
    }

    *count = raw_count / encoder->step_size;
    return ESP_OK;
}
