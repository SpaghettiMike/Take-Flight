
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"

#define ARRAY_SIZE 200
#define STORAGE_NAMESPACE "storage"


esp_err_t save_blob_nvs(uint8_t data[], char * location, size_t size);
esp_err_t print_blob(char* location);
esp_err_t save_value_nvs64(uint64_t data, char * location);
esp_err_t save_value_nvs16(uint16_t data, char * location);
esp_err_t save_value_nvs32(uint32_t data, char * location);
esp_err_t print_value(char* location);
esp_err_t get_blob_values(uint8_t accel_x[ARRAY_SIZE], uint8_t accel_y[ARRAY_SIZE], uint8_t accel_z[ARRAY_SIZE], uint8_t gyro_x[ARRAY_SIZE], uint8_t gyro_y[ARRAY_SIZE], uint8_t gyro_z[ARRAY_SIZE]);
esp_err_t get_values(uint32_t* lat_init, uint32_t* lat_final, uint32_t* long_init, uint32_t* long_final, uint32_t* alt_init, uint32_t* alt_final, uint16_t* avg_rps, uint16_t* peak_rps);
void initialize_nvs_data(void);
esp_err_t initialize_nvs_flash(void);


