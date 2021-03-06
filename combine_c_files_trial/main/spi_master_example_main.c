/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_system.h"
// #include "driver/spi_master.h"
// #include "soc/gpio_struct.h"
// #include "driver/gpio.h"

#include "nvs_blob.h"
#include "spi_master_example_main.h"

/*
 This code displays some fancy graphics on the 320x240 LCD on an ESP-WROVER_KIT board.
 This example demonstrates the use of both spi_device_transmit as well as
 spi_device_queue_trans/spi_device_get_trans_result and pre-transmit callbacks.

 Some info about the ILI9341/ST7789V: It has an C/D line, which is connected to a GPIO here. It expects this
 line to be low for a command and high for data. We use a pre-transmit callback here to control that
 line: every transaction has as the user-definable argument the needed state of the D/C line and just
 before the transaction is sent, the callback will set this line to the correct state.
*/

//To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
//but less overhead for setting up / finishing transfers. Make sure 240 is dividable by this.
#define PARALLEL_LINES 16
#define INV_MAX_SERIAL_READ 16


typedef struct {
    uint8_t accel_x[ARRAY_SIZE];
    uint8_t accel_y[ARRAY_SIZE];
    uint8_t accel_z[ARRAY_SIZE];
    uint8_t gyro_x[ARRAY_SIZE];
    uint8_t gyro_y[ARRAY_SIZE];
    uint8_t gyro_z[ARRAY_SIZE];
    uint8_t mag_x[ARRAY_SIZE];
    uint8_t mag_y[ARRAY_SIZE];
    uint8_t mag_z[ARRAY_SIZE];
}IMU_data_storage;

static IMU_data_storage IMU_data;

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

#define GPIO_OUTPUT_PIN_SEL (1ULL << PIN_NUM_CS)

void cs_gpio_setting()
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //SET AS OUTPUT MODE
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set, e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void cs_low()
{
    gpio_set_level(PIN_NUM_CS, 0);
}

void cs_high()
{
    gpio_set_level(PIN_NUM_CS, 1);
}

bool imu_read_trans(spi_device_handle_t spi, uint8_t reg, uint8_t *data)
{
    //esp_err_t ret;
    spi_transaction_t t;

    cs_low(); // CSN low, initialize SPI communication...

    reg |=0x80;
    memset(&t, 0, sizeof(t));   //Zero out the transaction
    t.length = 8;               //Command is 8 bits
    t.tx_buffer = &reg;         //The data is the cmd itself
    spi_device_transmit(spi, &t);

    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.flags = SPI_TRANS_USE_RXDATA;
    spi_device_transmit(spi, &t);

    cs_high();                  //CSN high, terminate SPI communication

    *data = t.rx_data[0];

    return true;
}

bool imu_write_trans(spi_device_handle_t spi, uint8_t reg, uint8_t data)
{
    //esp_err_t ret;
    spi_transaction_t t;
    uint8_t buf[2];

    cs_low(); // CSN low, initialize SPI communication...

    reg &= 0x7f;
    buf[0] = reg;
    buf[1] = data;
    memset(&t, 0, sizeof(t));   //Zero out the transaction
    t.length = 8 * 2;           //Command is 8 bits
    t.tx_buffer = buf;          //The data is the cmd itself
    
    spi_device_transmit(spi, &t);

    cs_high();                  //CSN high, terminate SPI communication

    return true;
}

//read lsm6ds3 chip id;
void imu_read(spi_device_handle_t spi, uint8_t reg1, uint8_t reg2, int user, uint8_t value[2])
{
    uint8_t id1;
    uint8_t id2;
    switch(user)
    {
        case 0:
            imu_write_trans(spi, REG_BANK_SEL, BANK0);
            break;
        case 1:
            imu_write_trans(spi, REG_BANK_SEL, BANK1);
            break;
        case 2:
            imu_write_trans(spi, REG_BANK_SEL, BANK2);
            break;
        case 3:
            imu_write_trans(spi, REG_BANK_SEL, BANK3);
            break;
        default:
            printf("ERROR! Enter a value between 0 and 3.\n");
    }
    if (reg2 == 0)
    {
        imu_read_trans(spi, reg1, &id1);
        *value = id1;
    }
    else
    {
        imu_read_trans(spi, reg1, &id1);
        imu_read_trans(spi, reg2, &id2);
        //*value = (id1 << 8) | id2;
        value[0] = id1;
        value[1] = id2;
    }
}

void imu_write(spi_device_handle_t spi, uint8_t write_reg, uint8_t write_data, int user)
{
    switch(user)
    {
        case 0:
            imu_write_trans(spi, REG_BANK_SEL, BANK0);
            break;
        case 1:
            imu_write_trans(spi, REG_BANK_SEL, BANK1);
            break;
        case 2:
            imu_write_trans(spi, REG_BANK_SEL, BANK2);
            break;
        case 3:
            imu_write_trans(spi, REG_BANK_SEL, BANK3);
            break;
        default:
            printf("ERROR! Enter a value between 0 and 3.\n");
    }
    imu_write_trans(spi, write_reg, write_data);
    //uint8_t read_data;
    //imu_read_trans(spi,write_reg, &read_data);
    //printf("read data from register 0x%02X: 0x%02X\n", write_reg, read_data);
}

void get_accel(spi_device_handle_t spi, int x)
{
    uint8_t value[2];
    //double decimal;
    //printf("X Accel bit value");
    imu_read(spi, ACCEL_XOUT_H, ACCEL_XOUT_L, 0, value);
    IMU_data.accel_x[x] = value[x];
    IMU_data.accel_x[x+1] = value[x+1];
//    decimal = value / 16384.0;
    //printf("X Accel decimal value %2.2f\n",decimal);
    //printf("Y Accel bit value:");
    imu_read(spi, ACCEL_YOUT_H, ACCEL_YOUT_L, 0, value);
    IMU_data.accel_y[x] = value[x];
    IMU_data.accel_y[x+1] = value[x+1];
    //decimal = value / 16384.0;
    //printf("Y Accel decimal value %2.2f\nZ Accel bit value",decimal);
    imu_read(spi, ACCEL_YOUT_H, ACCEL_YOUT_L, 0, value);
    IMU_data.accel_z[x] = value[x];
    IMU_data.accel_z[x+1] = value[x+1];
    //decimal = value / 16384.0;
    //printf("Z Accel decimal value %2.2f\n", decimal);
}

void get_gyro(spi_device_handle_t spi, int x)
{
    uint8_t value[2];
    //double decimal;
    //printf("X Gyro bit value");
    imu_read(spi, GYRO_XOUT_H, GYRO_XOUT_L, 0, value);
    IMU_data.gyro_x[x] = value[x];
    IMU_data.gyro_x[x+1] = value[x+1];
    //decimal = value / 131.0;
    //printf("X Gyro decimal value %2.2f\n",decimal);

    //printf("Y Gyro bit value");
    imu_read(spi, GYRO_YOUT_H, GYRO_YOUT_L, 0, value);
    IMU_data.gyro_y[x] = value[x];
    IMU_data.gyro_y[x+1] = value[x+1];
    //decimal = value / 131.0;
    //printf("Y Gyro decimal value %2.2f\n",decimal);

    //printf("Z Gyro bit value");
    imu_read(spi, GYRO_ZOUT_H, GYRO_ZOUT_L, 0, value);
    IMU_data.gyro_z[x] = value[x];
    IMU_data.gyro_z[x+1] = value[x+1];
    //decimal = value / 131.0;
    //printf("Z Gyro decimal value %2.2f\n",decimal);
}

void get_magnetometer(spi_device_handle_t spi, int x)
{
    uint8_t value[2];
    //double decimal;

    //printf("X Mag bit value");
    imu_read(spi, MAG_X_H, MAG_X_L, 3, value);
    IMU_data.mag_x[x] = value[x];
    IMU_data.mag_x[x+1] = value[x+1];
    //decimal = value / 0.15;
    //printf("X Mag decimal value %2.2f\n",decimal);

    //printf("Y Mag bit value");
    imu_read(spi, MAG_Y_H, MAG_Y_L, 3, value);
    IMU_data.mag_y[x] = value[x];
    IMU_data.mag_y[x+1] = value[x+1];
    //decimal = value / 0.15;
    //printf("Y Mag decimal value %2.2f\n",decimal);

    //printf("Z Mag bit value");
    imu_read(spi, MAG_Z_H, MAG_Z_L, 3, value);
    IMU_data.mag_z[x] = value[x];
    IMU_data.mag_z[x+1] = value[x+1];
    //decimal = value / 0.15;
    //printf("Z Gyro decimal value %2.2f\n",decimal);
}

void imu_initialize(spi_device_handle_t spi)
{
    imu_write(spi, PWR_MGMT_1, 0x01, 0);    //initialize the 
    imu_write(spi, PWR_MGMT_2, 0x00, 0);
    imu_write(spi, USER_CTRL, 0xF0, 0);
    //imu_write(spi, MAG_CNTL3, 0x01, 3);
    //imu_write(spi, MAG_CNTL2, 0x08, 3);
}

uint16_t average(uint16_t array[ARRAY_SIZE]) 
{
   int sum, loop;
   uint16_t avg;

   sum = avg = 0;
   
   for(loop = 0; loop < (ARRAY_SIZE - 1); loop++) {
      sum = sum + array[loop];
   }
   
   avg = sum / loop; 
   
   return avg;
}

uint16_t max(uint16_t arr[ARRAY_SIZE]) 
{ 
    int i; 
        
    // Initialize maximum element 
    int max = arr[0]; 
    
    // Traverse array elements from second and 
    // compare every element with current max  
    for (i = 1; i < (ARRAY_SIZE - 1); i++) 
        if (arr[i] > max) 
            max = arr[i]; 
    return max; 
}

esp_err_t IMU_blob_nvs(int user, char * location)
{   
    uint8_t data[ARRAY_SIZE];
    esp_err_t err;

    switch(user)
    {
        case 0:
            for(int x = 0; x < (ARRAY_SIZE - 1); x++){
                data[x] = IMU_data.accel_x[x];
            }
            break;
        case 1:
            for(int x = 0; x < (ARRAY_SIZE - 1); x++){
                data[x] = IMU_data.accel_y[x];
            }
            break;
        case 2:
            for(int x = 0; x < (ARRAY_SIZE - 1); x++){
                data[x] = IMU_data.accel_z[x];
            }
            break;
        case 3:
            for(int x = 0; x < (ARRAY_SIZE - 1); x++){
                data[x] = IMU_data.gyro_x[x];
            }
            break;
        case 4:
            for(int x = 0; x < (ARRAY_SIZE - 1); x++){
                data[x] = IMU_data.gyro_y[x];
            }
            break;
        case 5:
            for(int x = 0; x < (ARRAY_SIZE - 1); x++){
                data[x] = IMU_data.gyro_z[x];
            }
            break;
        case 6:
            for(int x = 0; x < (ARRAY_SIZE - 1); x++){
                data[x] = IMU_data.mag_x[x];
            }
            break;
        case 7:
            for(int x = 0; x < (ARRAY_SIZE - 1); x++){
                data[x] = IMU_data.mag_y[x];
            }
            break;
        case 8:
            for(int x = 0; x < (ARRAY_SIZE - 1); x++){
                data[x] = IMU_data.mag_z[x];
            }
            break;
        default:
            printf("ERROR! Enter a value between 0 and 8.\n");
    }
    size_t size = sizeof(data);
    // Open
    err = save_blob_nvs(data, location, size);
    if (err != ESP_OK) return err;
    return ESP_OK;
}


/* Read from NVS and print restart counter
   and the table with run times.
   Return an error if anything goes wrong
   during this process.
 */

esp_err_t save_array(void)
{
    esp_err_t err;
    uint32_t init = 1;
    uint32_t final = 2;
    
    err = IMU_blob_nvs(0, "accel_x");
    if (err != ESP_OK) 
    {
        printf("error on save_blob_nvs");
        return err;
    }
    err = IMU_blob_nvs(1, "accel_y");
    if (err != ESP_OK) return err;
    err = IMU_blob_nvs(2, "accel_z");
    if (err != ESP_OK) return err;
    err = IMU_blob_nvs(3, "gyro_x");
    if (err != ESP_OK) return err;
    err = IMU_blob_nvs(4, "gyro_y");
    if (err != ESP_OK) return err;
    err = IMU_blob_nvs(5, "gyro_z");
    if (err != ESP_OK) return err;

    //err = save_value_nvs16(average(IMU_data->gyro_x), "avg_rps");
    //err = save_value_nvs16(max(IMU_data->gyro_x), "peak_rps");
    err = save_value_nvs32(init, "lat_init");
    if (err != ESP_OK) return err;
    err = save_value_nvs32(final, "lat_final");
    if (err != ESP_OK) return err;
    err = save_value_nvs32(init, "long_init");
    if (err != ESP_OK) return err;
    err = save_value_nvs32(final, "long_final");
    if (err != ESP_OK) return err;
    err = save_value_nvs32(init, "alt_init");
    if (err != ESP_OK) return err;
    err = save_value_nvs32(final, "alt_final");
    if (err != ESP_OK) return err;

    return ESP_OK;
}

/*int inv_icm20948_firmware_load(struct inv_icm20948 * s, const unsigned char *data_start, unsigned short size_start, unsigned short load_addr)
{ 
    int write_size;
    int result;
    unsigned short memaddr;
    const unsigned char *data;
    unsigned short size;
    unsigned char data_cmp[16];
    int flag = 0;
		
    // Write DMP memory
    data = data_start;
    size = size_start;
    memaddr = load_addr;
    while (size > 0) {
        write_size = min(size, 16);
        if ((memaddr & 0xff) + write_size > 0x100) {
            // Moved across a bank
            write_size = (memaddr & 0xff) + write_size - 0x100;
        }
        result = inv_icm20948_write_mems(s, memaddr, write_size, (unsigned char *)data);
        if (result)  
            return result;
        data += write_size;
        size -= write_size;
        memaddr += write_size;
    }

    // Verify DMP memory

    data = data_start;
    size = size_start;
    memaddr = load_addr;
    while (size > 0) {
        write_size = min(size, 16);
        if ((memaddr & 0xff) + write_size > 0x100) {
            // Moved across a bank
            write_size = (memaddr & 0xff) + write_size - 0x100;
        }
        result = inv_icm20948_read_mems(s, memaddr, write_size, data_cmp);
        if (result)
            flag++; // Error, DMP not written correctly
        if (memcmp(data_cmp, data, write_size))
            return -1;
        data += write_size;
        size -= write_size;
        memaddr += write_size;
    }

#if defined(WIN32)   
    //if(!flag)
      // inv_log("DMP Firmware was updated successfully..\r\n");
#endif

    return 0;
}
*/

/*int inv_icm20948_write_mems(struct inv_icm20948 * s, unsigned short reg, unsigned int length, const unsigned char *data)
{
    int result=0;
    unsigned int bytesWritten = 0;
    unsigned int thisLen;
    unsigned char lBankSelected;
    unsigned char lStartAddrSelected;
    
    unsigned char power_state = inv_icm20948_get_chip_power_state(s);

    if(!data)
        return -1;
    
    if((power_state & CHIP_AWAKE) == 0)   // Wake up chip since it is asleep
        result = inv_icm20948_set_chip_power_state(s, CHIP_AWAKE, 1);

    result |= inv_icm20948_set_chip_power_state(s, CHIP_LP_ENABLE, 0);
            
	result |= inv_set_bank(s, 0);
    
    lBankSelected = (reg >> 8);
	if (lBankSelected != s->lLastBankSelected)
	{
		result |= inv_icm20948_write_reg(s, REG_MEM_BANK_SEL, &lBankSelected, 1);
		if (result)
			return result;
		s->lLastBankSelected = lBankSelected;
	}
    
    while (bytesWritten < length) 
    {
        lStartAddrSelected = (reg & 0xff);
        // Sets the starting read or write address for the selected memory, inside of the selected page (see MEM_SEL Register).
        //   Contents are changed after read or write of the selected memory.
        //   This register must be written prior to each access to initialize the register to the proper starting address.
        //   The address will auto increment during burst transactions.  Two consecutive bursts without re-initializing the start address would skip one address.
        result |= inv_icm20948_write_reg(s, REG_MEM_START_ADDR, &lStartAddrSelected, 1);
        if (result)
            return result;
        
        thisLen = min(INV_MAX_SERIAL_WRITE, length-bytesWritten);
        
        // Write data 
        result |= inv_icm20948_write_reg(s, REG_MEM_R_W, &data[bytesWritten], thisLen);
        if (result)
            return result;
        
        bytesWritten += thisLen;
        reg += thisLen;
    }

    //Enable LP_EN since we disabled it at begining of this function.
    result |= inv_icm20948_set_chip_power_state(s, CHIP_LP_ENABLE, 1);

    return result;
}
*/
/*int inv_icm20948_read_mems(struct inv_icm20948 * s, unsigned short reg, unsigned int length, unsigned char *data)
{
	int result=0;
	unsigned int bytesWritten = 0;
	unsigned int thisLen;
	unsigned char i, dat[INV_MAX_SERIAL_READ] = {0};
	unsigned char power_state = inv_icm20948_get_chip_power_state(s);
	unsigned char lBankSelected;
	unsigned char lStartAddrSelected;

	if(!data)
		return -1;

	if((power_state & CHIP_AWAKE) == 0)   // Wake up chip since it is asleep
		result = inv_icm20948_set_chip_power_state(s, CHIP_AWAKE, 1);

	if(check_reg_access_lp_disable(s, reg))
		result |= inv_icm20948_set_chip_power_state(s, CHIP_LP_ENABLE, 0);

	result |= inv_set_bank(s, 0);

	lBankSelected = (reg >> 8);
	if (lBankSelected != s->lLastBankSelected)
	{
		result |= inv_icm20948_write_reg(s, REG_MEM_BANK_SEL, &lBankSelected, 1);
		if (result)
			return result;
		s->lLastBankSelected = lBankSelected;
	}

	while (bytesWritten < length) 
	{
		lStartAddrSelected = (reg & 0xff);
		// Sets the starting read or write address for the selected memory, inside of the selected page (see MEM_SEL Register).
		//   Contents are changed after read or write of the selected memory.
		//   This register must be written prior to each access to initialize the register to the proper starting address.
		//   The address will auto increment during burst transactions.  Two consecutive bursts without re-initializing the start address would skip one address.
		result |= inv_icm20948_write_reg(s, REG_MEM_START_ADDR, &lStartAddrSelected, 1);
		if (result)
			return result;
		
		thisLen = min(INV_MAX_SERIAL_READ, length-bytesWritten);
		// Write data 
		if(s->base_state.serial_interface == SERIAL_INTERFACE_SPI) {
			result |= inv_icm20948_read_reg(s, REG_MEM_R_W, &dat[bytesWritten], thisLen);
		} else {
			result |= inv_icm20948_read_reg(s, REG_MEM_R_W, &data[bytesWritten], thisLen);
		}
		if (result)
			return result;
		
		bytesWritten += thisLen;
		reg += thisLen;
	}

	if(s->base_state.serial_interface == SERIAL_INTERFACE_SPI) {
		for (i=0; i< length; i++) {
			*data= dat[i];
			 data++;
		}
	}

	//Enable LP_EN if we disabled it at begining of this function.
	if(check_reg_access_lp_disable(s, reg))
		result |= inv_icm20948_set_chip_power_state(s, CHIP_LP_ENABLE, 1);

	return result;
}
*/

void spi_main(void)
{
    esp_err_t ret;
    spi_device_handle_t spi;
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=PARALLEL_LINES*320*2+8
    };
    spi_device_interface_config_t devcfg={

        .clock_speed_hz=6*1000*1000,           //Clock out at 10 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num=22,               //CS pin
        .queue_size=1,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };

     //Initialize the SPI bus
    ret=spi_bus_initialize(1, &buscfg, 1);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(1, &devcfg, &spi);
    //inv_icm20948_firmware_load(s, image, size, 0x90);
    imu_initialize(spi);
    cs_gpio_setting();
    gpio_pad_select_gpio(GPIO_NUM_0);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_DEF_INPUT);

    ESP_ERROR_CHECK(ret);

    while(1)
    {
        ret = delete_nvs_data();
        ret = initialize_nvs_flash();        
        
        ESP_ERROR_CHECK(ret);

        /* Read the status of GPIO0. If GPIO0 is LOW for longer than 1000 ms,
        then save module's run time and restart it
        */

        while (1) {
            if (gpio_get_level(GPIO_NUM_0) == 0) {
                vTaskDelay(500/portTICK_PERIOD_MS);
                if (gpio_get_level(GPIO_NUM_0) != 0) {
                    break;
                }
                else{
                    vTaskDelay(2000/portTICK_PERIOD_MS);
                }
            }
            vTaskDelay(10/portTICK_PERIOD_MS);
        }
        
        nvs_erase();
        ret = initialize_nvs_flash();
        
        printf("Start recording IMU\n");
        //Start reading and writing using the SPI connection
        printf("read and write the SPI connection\n");
        for(int x = 0; x < (ARRAY_SIZE - 1); x = x + 2)
        {
            get_accel(spi,x);
            get_gyro(spi,x);
            //get_magnetometer(spi,x);
            vTaskDelay(1);
        }
        
        ret = save_array();
        if (ret != ESP_OK){
            printf("error! save array\n");
        }
        
        ret = print_blob("accel_x");
        printf("\n");
        ret = print_blob("accel_y");
        printf("\n");
        ret = print_blob("accel_z");
        printf("\n");
        ret = print_blob("gyro_x");
        printf("\n");
        ret = print_blob("gyro_y");
        printf("\n");
        ret = print_blob("gyro_z");
        printf("\n");

        if (ret != ESP_OK){
            printf("error! print_blob\n");
        }
    }
}
