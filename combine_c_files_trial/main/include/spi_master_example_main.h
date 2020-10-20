#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"

#define STORAGE_NAMESPACE "storage"
#define ARRAY_SIZE 200

//User Bank 0
#define WHO_AM_I        0x00
#define USER_CTRL       0x03
#define PWR_MGMT_1      0x06
#define PWR_MGMT_2      0x07
#define INT_STATUS_1    0x1a
#define MEAS_ON         0x01
#define ACCEL_XOUT_H    0x2d
#define ACCEL_XOUT_L   0x2e
#define ACCEL_YOUT_H    0x2f
#define ACCEL_YOUT_L   0x30
#define ACCEL_ZOUT_H    0x31
#define ACCEL_ZOUT_L   0x32
#define GYRO_XOUT_H   0x33
#define GYRO_XOUT_L  0x34
#define GYRO_YOUT_H   0x35
#define GYRO_YOUT_L  0x36
#define GYRO_ZOUT_H   0x37
#define GYRO_ZOUT_L  0x38


//Bank 3
#define MAG_X_H    0x12
#define MAG_X_L   0x11
#define MAG_Y_H    0x14
#define MAG_Y_L   0x13
#define MAG_Z_H    0x16
#define MAG_Z_L   0x15
#define MAG_CNTL2 0x31
#define MAG_SINGLE 0x01
#define MAG_CONT1 0x02

#define REG_BANK_SEL    0x7f
#define BANK0           0x00
#define BANK1           0x10
#define BANK2           0x20
#define BANK3           0x30

#define MAG_CONT_1      0x03 
#define MAG_CNTL3      0x32   

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL 5

void lcd_spi_pre_transfer_callback(spi_transaction_t *t);
void cs_gpio_setting();
void cs_low();
void cs_high();
bool imu_read_trans(spi_device_handle_t spi, uint8_t reg, uint8_t *data);
bool imu_write_trans(spi_device_handle_t spi, uint8_t reg, uint8_t data);
void imu_read(spi_device_handle_t spi, uint8_t reg1, uint8_t reg2, int user, uint8_t value[2]);
void imu_write(spi_device_handle_t spi, uint8_t write_reg, uint8_t write_data, int user);
void get_accel(spi_device_handle_t spi, int x);
void get_gyro(spi_device_handle_t spi, int x);
void get_magnetometer(spi_device_handle_t spi, int x);
void imu_initialize(spi_device_handle_t spi);
uint16_t average(uint16_t array[ARRAY_SIZE]);
uint16_t max(uint16_t arr[ARRAY_SIZE]);
esp_err_t IMU_blob_nvs(int user, char * location);
esp_err_t save_array(void);
void spi_main(void);
