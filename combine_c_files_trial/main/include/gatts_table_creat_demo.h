/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "gatts_table_creat_demo.h"
#include "esp_gatt_common_api.h"
*/

/* Attributes State Machine */

enum
{
    IDX_SVC,
    IDX_CHAR_ilong,
    IDX_CHAR_VAL_ilong,
    IDX_CHAR_CFG_ilong,

    IDX_CHAR_ilat,
    IDX_CHAR_VAL_ilat,

    IDX_CHAR_flong,
    IDX_CHAR_VAL_flong,

    IDX_CHAR_flat,
    IDX_CHAR_VAL_flat,

    IDX_CHAR_xAccel,
    IDX_CHAR_VAL_xAccel,

    IDX_CHAR_yAccel,
    IDX_CHAR_VAL_yAccel,

    IDX_CHAR_zAccel,
    IDX_CHAR_VAL_zAccel,

    IDX_CHAR_xGyro,
    IDX_CHAR_VAL_xGyro,

    IDX_CHAR_yGyro,
    IDX_CHAR_VAL_yGyro,

    IDX_CHAR_zGyro,
    IDX_CHAR_VAL_zGyro,

//    IDX_CHAR_avgRPS,
//     IDX_CHAR_VAL_avgRPS,
    
//     IDX_CHAR_peakRPS,
//     IDX_CHAR_VAL_peakRPS,

    IDX_CHAR_ialt,
    IDX_CHAR_VAL_ialt,

    IDX_CHAR_falt,
    IDX_CHAR_VAL_falt,

    HRS_IDX_NB,
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void example_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
void gatt_main(void);

