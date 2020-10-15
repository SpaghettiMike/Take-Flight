/* Non-Volatile Storage (NVS) Read and Write a Blob - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"

#define STORAGE_NAMESPACE "storage"

/* Save the number of module restarts in NVS
   by first reading and then incrementing
   the number that has been saved previously.
   Return an error if anything goes wrong
   during this process.
 */

/* Save new run time value in NVS
   by first reading a table of previously saved values
   and then adding the new value at the end of the table.
   Return an error if anything goes wrong
   during this process.
 */
esp_err_t save_blob_nvs(uint16_t data[], char * location, size_t size)
{
    esp_err_t err;
    nvs_handle_t my_handle;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Read previously saved blob if available
    err = nvs_set_blob(my_handle, location, data, size);
    if (err != ESP_OK) return err;

    // Commit*/
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close*/
    nvs_close(my_handle);
    return ESP_OK;
}

/* Read from NVS and print restart counter
   and the table with run times.
   Return an error if anything goes wrong
   during this process.
 */
esp_err_t print_blob(char* location)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;
    size_t size;

    err = nvs_get_blob(my_handle, location, NULL, &size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;


    uint16_t data[10];

    err = nvs_get_blob(my_handle, location, data, &size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    for(int x = 0; x < 10; x++){
        printf("%s = %d\n", location, data[x]);
    }

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t save_value_nvs(uint32_t data, char * location)
{
    esp_err_t err;
    nvs_handle_t my_handle;
    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_set_u32(my_handle, location, data);
    if (err != ESP_OK) return err;

    // Commit*/
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close*/
    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t print_value(char* location)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;
    uint32_t data = 0;

    err = nvs_get_u32(my_handle, location, &data);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    printf("%s = %d\n", location, data);

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}



void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    uint16_t data[10] = {0,1,2,3,4,5,6,7,8,9};
    uint32_t test_value = 1;

    char* accel_x = "accel_x";
    char* accel_y = "accel_y";
    char* accel_z = "accel_z";

    char* gyro_x = "gyro_x";
    char* gyro_y = "gyro_y";
    char* gyro_z = "gyro_z";

    char* lat_init = "lat_init";
    char* lat_final = "lat_final";
    char* long_init = "long_final";
    char* long_final = "long_final";

    char* alt_init = "alt_init";
    char* alt_final = "alt_final";

    err = save_blob_nvs(data, accel_x,sizeof(data));
    err = print_blob(accel_x);

    err = save_blob_nvs(data, accel_y,sizeof(data));
    err = print_blob(accel_y);

    err = save_blob_nvs(data, accel_z,sizeof(data));
    err = print_blob(accel_z);

    err = save_blob_nvs(data, gyro_x,sizeof(data));
    err = print_blob(gyro_x);

    err = save_blob_nvs(data, gyro_y,sizeof(data));
    err = print_blob(gyro_y);

    err = save_blob_nvs(data, gyro_z,sizeof(data));
    err = print_blob(gyro_z);

    err = save_value_nvs(test_value, lat_init);
    err = save_value_nvs(test_value, long_init);
    err = save_value_nvs(test_value, lat_final);
    err = save_value_nvs(test_value, long_final);
    err = save_value_nvs(test_value, alt_init);
    err = save_value_nvs(test_value, alt_final); 

    err = print_value(lat_init);
    err = print_value(long_init);
    err = print_value(lat_final);
    err = print_value(long_final);
    err = print_value(alt_init);
    err = print_value(alt_final);

}
