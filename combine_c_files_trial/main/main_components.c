/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_blob.h"
#include "gatts_table_creat_demo.h"
#include "spi_master_example_main.h"

void app_main(void)
{
    TaskHandle_t xHandle1;
    TaskHandle_t xHandle2;
    xTaskCreate(spi_main,"spi_main", 20000, NULL, 1, &xHandle1);
//    initialize_nvs_data();
    xTaskCreate(gatt_main,"gatt_main", 20000, NULL, 2, &xHandle2);
}
