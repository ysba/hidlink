#ifndef __BLE_CDTP__
#define __BLE_CDTP__


#define BLE_CDTP_BUFFER_MAX_LEN         512
#define BLE_CDTP_SERVICE_INSTANCE_ID    0

//Attributes State Machine
enum{
    BLE_CDTP_INDEX_SERVICE,
    BLE_CDTP_INDEX_DATA_CHAR,
    BLE_CDTP_INDEX_DATA_VAL,
    BLE_CDTP_INDEX_DATA_CFG,
    BLE_CDTP_INDEX_MAX,
};


void gatts_cdtp_cb(esp_gatts_cb_event_t event,esp_gatt_if_t gatts_if,esp_ble_gatts_cb_param_t *param);

extern uint8_t cdtp_service_uuid[16];
extern uint8_t cdtp_data_char_uuid[16];
extern esp_ble_adv_params_t cdtp_adv_params;

#endif
