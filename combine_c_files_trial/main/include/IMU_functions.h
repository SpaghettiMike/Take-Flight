
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"



void get_gyro(spi_device_handle_t spi, int x);
void get_accel(spi_device_handle_t spi, int x);
uint16_t max(uint16_t arr[50]);
uint16_t average(uint16_t array[50]);
void save_array(void);
void IMU_main(void);
void imu_initialize(spi_device_handle_t spi);
void imu_write(spi_device_handle_t spi, uint8_t write_reg, uint8_t write_data, int user);
void imu_read(spi_device_handle_t spi, uint8_t reg1, uint8_t reg2, int user, uint16_t *value);
bool imu_write_trans(spi_device_handle_t spi, uint8_t reg, uint8_t data);
bool imu_read_trans(spi_device_handle_t spi, uint8_t reg, uint8_t *data);
void cs_high();
void cs_low();
void cs_gpio_setting();
void lcd_spi_pre_transfer_callback(spi_transaction_t *t);