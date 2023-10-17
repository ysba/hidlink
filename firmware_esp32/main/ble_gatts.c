#include "main.h"


static const char *TAG = "BLE_GATTS";


struct gatts_profile_inst{
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};


static struct gatts_profile_inst gatt_profile_table[GATT_PROFILE_INDEX_MAX] = {
    [GATT_PROFILE_INDEX_SPP] = {
        .gatts_cb = gatts_cdtp_cb,
        .gatts_if = ESP_GATT_IF_NONE,
    },
};


void ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {

    switch(event) {

        case ESP_GATTS_REG_EVT: {
            if (param->reg.status == ESP_GATT_OK) {
                ESP_LOGD(TAG, "ESP_GATTS_REG_EVT Reg app ESP_GATT_OK");
                gatt_profile_table[GATT_PROFILE_INDEX_SPP].gatts_if = gatts_if;
            }
            else {
                ESP_LOGW(TAG, "ESP_GATTS_REG_EVT Reg app failed, app_id %04x, status %d",
                    param->reg.app_id, 
                    param->reg.status);
                    return;
            }           
            break;
        }
        
        case ESP_GATTS_WRITE_EVT: 
            ESP_LOGD(TAG, "ESP_GATTS_WRITE_EVT"); 
            break;

        case ESP_GATTS_MTU_EVT: 
            ESP_LOGD(TAG, "ESP_GATTS_MTU_EVT"); 
            break;

        case ESP_GATTS_CONF_EVT: 
            ESP_LOGD(TAG, "ESP_GATTS_CONF_EVT"); 
            break;

        case ESP_GATTS_START_EVT: 
            ESP_LOGD(TAG, "ESP_GATTS_START_EVT"); 
            break;

        case ESP_GATTS_CONNECT_EVT: 
            ESP_LOGD(TAG, "ESP_GATTS_CONNECT_EVT"); 
            break;

        case ESP_GATTS_DISCONNECT_EVT: 
            ESP_LOGD(TAG, "ESP_GATTS_DISCONNECT_EVT"); 
            break;

        case ESP_GATTS_CREAT_ATTR_TAB_EVT: 
            ESP_LOGD(TAG, "ESP_GATTS_CREAT_ATTR_TAB_EVT"); 
            break;

    	default: 
            ESP_LOGW(TAG, "UNEXPECTED GATTS EVENT: %d", event); 
            break;    	    
    }

    
    // iterate through all profiles declared in gatt_profile_table
    int idx;
    for (idx = 0; idx < GATT_PROFILE_INDEX_MAX; idx++) {    
        if (gatts_if == ESP_GATT_IF_NONE || gatts_if == gatt_profile_table[idx].gatts_if) {
            if (gatt_profile_table[idx].gatts_cb) {
                gatt_profile_table[idx].gatts_cb(event, gatts_if, param);
            }
            else {
                ESP_LOGW(TAG, "skip 2");
            }
        }
        else {
            ESP_LOGW(TAG, "skip 1");
        }
    }
}
