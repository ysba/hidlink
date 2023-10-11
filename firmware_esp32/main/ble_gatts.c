#include "main.h"

static const char *TAG = "BLE_GATTS";


void ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {

    switch(event) {

        case ESP_GATTS_REG_EVT:

            if (param->reg.status == ESP_GATT_OK) {
                ESP_LOGD(TAG, "ESP_GATTS_REG_EVT Reg app ESP_GATT_OK");
                //spp_profile_tab[SPP_PROFILE_APP_IDX].gatts_if = gatts_if;
            }
            else {
                ESP_LOGD(   TAG, 
                    "ESP_GATTS_REG_EVT Reg app failed, app_id %04x, status %d",
                    param->reg.app_id, 
                    param->reg.status
                    );
                return;
            }           
            break;
        
        // case ESP_GATTS_READ_EVT:
        //     ESP_LOGD(TAG, "ESP_GATTS_READ_EVT"); 
        //     break;

        // case ESP_GATTS_WRITE_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_WRITE_EVT"); 
        //     break;

        // case ESP_GATTS_EXEC_WRITE_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_EXEC_WRITE_EVT"); 
        //     break;

        // case ESP_GATTS_MTU_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_MTU_EVT"); 
        //     break;

        // case ESP_GATTS_CONF_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_CONF_EVT"); 
        //     break;

        // case ESP_GATTS_UNREG_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_UNREG_EVT"); 
        //     break;

        // case ESP_GATTS_CREATE_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_CREATE_EVT"); 
        //     break;

        // case ESP_GATTS_ADD_INCL_SRVC_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_ADD_INCL_SRVC_EVT"); 
        //     break;

        // case ESP_GATTS_ADD_CHAR_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_ADD_CHAR_EVT"); 
        //     break;

        // case ESP_GATTS_ADD_CHAR_DESCR_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_ADD_CHAR_DESCR_EVT"); 
        //     break;

        // case ESP_GATTS_DELETE_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_DELETE_EVT"); 
        //     break;

        // case ESP_GATTS_START_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_START_EVT"); 
        //     break;

        // case ESP_GATTS_STOP_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_STOP_EVT"); 
        //     break;

        // case ESP_GATTS_CONNECT_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_CONNECT_EVT"); 
        //     break;

        // case ESP_GATTS_DISCONNECT_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_DISCONNECT_EVT"); 
        //     break;

        // case ESP_GATTS_OPEN_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_OPEN_EVT"); 
        //     break;

        // case ESP_GATTS_CANCEL_OPEN_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_CANCEL_OPEN_EVT"); 
        //     break;

        // case ESP_GATTS_CLOSE_EVT:
        //     ESP_LOGD(TAG, "ESP_GATTS_CLOSE_EVT"); 
        //     break;

        // case ESP_GATTS_LISTEN_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_LISTEN_EVT"); 
        //     break;

        // case ESP_GATTS_CONGEST_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_CONGEST_EVT"); 
        //     break;

        // case ESP_GATTS_RESPONSE_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_RESPONSE_EVT"); 
        //     break;

        // case ESP_GATTS_CREAT_ATTR_TAB_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_CREAT_ATTR_TAB_EVT"); 
        //     break;

        // case ESP_GATTS_SET_ATTR_VAL_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_SET_ATTR_VAL_EVT"); 
        //     break;

        // case ESP_GATTS_SEND_SERVICE_CHANGE_EVT: 
        //     ESP_LOGD(TAG, "ESP_GATTS_SEND_SERVICE_CHANGE_EVT"); 
        //     break;        

    	default: 
            ESP_LOGD(TAG, "unexpected gatts event: %d", event); 
            break;    	    
    }

    // do {
    //     int idx;

    //     // loop percorre todos os profiles declarados em "spp_profile_tab"

    //     for (idx = 0; idx < GATT_PROFILE_NUM; idx++) {
            
    //         /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
    //         if (gatts_if == ESP_GATT_IF_NONE || gatts_if == spp_profile_tab[idx].gatts_if) {
                
    //             if (spp_profile_tab[idx].gatts_cb) {

    //                 spp_profile_tab[idx].gatts_cb(event, gatts_if, param);
    //             }
    //         }
    //     }
    // } while (0);
    
}