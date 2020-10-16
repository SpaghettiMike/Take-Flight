/* NMEA Parser example, that decode data stream from GPS receiver

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nmea_parser.h"

static const char *TAG = "gps_demo";

#define TIME_ZONE (+8)   //Beijing Time
#define YEAR_BASE (2000) //date in GPS starts from 2000

/**
 * @brief GPS Event Handler
 *
 * @param event_handler_arg handler specific arguments
 * @param event_base event base, here is fixed to ESP_NMEA_EVENT
 * @param event_id event id
 * @param event_data event specific arguments
 */
static void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    gps_t *gps = NULL;
    switch (event_id) {
    case GPS_UPDATE:
        gps = (gps_t *)event_data;
        /* print information parsed from GPS statements */
        ESP_LOGI(TAG, "%d/%d/%d %d:%d:%d => \r\n"
                 "\t\t\t\t\t\tlatitude   = %.05f°N\r\n"
                 "\t\t\t\t\t\tlongitude = %.05f°E\r\n"
                 "\t\t\t\t\t\taltitude   = %.02fm\r\n"
                 "\t\t\t\t\t\tspeed      = %fm/s",
                 gps->date.year + YEAR_BASE, gps->date.month, gps->date.day,
                 gps->tim.hour + TIME_ZONE, gps->tim.minute, gps->tim.second,
                 gps->latitude, gps->longitude, gps->altitude, gps->speed);
        break;
    case GPS_UNKNOWN:
        /* print unknown statements */
        ESP_LOGW(TAG, "Unknown statement:%s", (char *)event_data);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    // /* NMEA parser configuration */
    // printf("NMEA_PARTSER_CONFIG_DEFAULT");
    // nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
    // /* init NMEA parser library */
    // printf("nmea_parser_init(&config");
    // nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
    // /* register event handler for NMEA parser library */
    // printf("nmea_parser_add_handler(nmea_hdl, gps_event_handler, NULL)");
    // nmea_parser_add_handler(nmea_hdl, gps_event_handler, NULL);

    // vTaskDelay(10000 / portTICK_PERIOD_MS);

    // /* unregister event handler */
    // printf("nmea_parser_remove_handler(nmea_hdl, gps_event_handler)");
    // nmea_parser_remove_handler(nmea_hdl, gps_event_handler);
    // /* deinit NMEA parser library */
    // printf("nmea_parser_deinit(nmea_hdl)");
    // nmea_parser_deinit(nmea_hdl);

    const int uart_num = UART_NUM_2;
    uart_config_t uart_config = {
        .baud_rate = 38400,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // Set UART pins(TX: IO16 (UART2 default), RX: IO17 (UART2 default), RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, 18, 19));

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, \
                                            uart_buffer_size, 10, &uart_queue, 0));

    uint8_t data[200];
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
    
    printf("length = %i\n",length);
    
    length = uart_read_bytes(uart_num, data, length, 100);

    printf("length = %i\n", length);

    for(int a=0;a < 200;a++)
    {
        printf("0x%hhX ", data[a]);
    }
    
    printf("\n");
/*
    for(int a=0;a < 128;a++)
    {
        printf("%c ", data[a]);
    }
*/
}
