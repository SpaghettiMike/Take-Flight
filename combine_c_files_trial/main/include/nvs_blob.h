
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"



esp_err_t save_blob_nvs(uint16_t data[], char * location, size_t size);
esp_err_t print_blob(char* location);
esp_err_t save_value_nvs(uint32_t data, char * location);
esp_err_t print_value(char* location);
esp_err_t get_blob_values(uint16_t accel_x[10], uint16_t accel_y[10], uint16_t accel_z[10], uint16_t gyro_x[10], uint16_t gyro_y[10], uint16_t gyro_z[10]);
esp_err_t get_values(uint64_t* lat_init, uint64_t* lat_final, uint64_t* long_init, uint64_t* long_final, uint64_t* alt_init, uint64_t* alt_final, uint16_t* avg_rps, uint16_t* peak_rps);
void initialize_nvs_data(void);


