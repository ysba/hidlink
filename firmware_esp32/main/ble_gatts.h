#ifndef __BLE_GATTS__
#define __BLE_GATTS__

enum {
    GATT_PROFILE_INDEX_SPP = 0,
    GATT_PROFILE_INDEX_MAX,
};

#define ESP_SPP_APP_ID              0x56

void ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

#endif

