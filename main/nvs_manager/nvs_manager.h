#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include <esp_err.h>

#include <nvs.h>
#include <nvs_flash.h>

esp_err_t nvs_manager_init();
esp_err_t nvs_manager_write_int(const char* key, int value);
esp_err_t nvs_manager_read_int(const char* key, int* value);

#endif // NVS_MANAGER_H
