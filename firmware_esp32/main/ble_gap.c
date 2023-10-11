#include "main.h"

static const char *TAG = "BLE_GAP";


// https://docs.silabs.com/bluetooth/latest/general/adv-and-scanning/bluetooth-adv-data-basics


static uint8_t spp_service_uuid[16] =       {0xad,0xae,0x02,0x5f,0x93,0xe6,0x4e,0xa7,0xa6,0xf4,0x97,0xad,0x00,0x59,0x00,0x00};
static uint8_t spp_data_notify_uuid[16] =   {0xad,0xae,0x02,0x5f,0x93,0xe6,0x4e,0xa7,0xa6,0xf4,0x97,0xad,0x01,0x59,0x00,0x00};
static uint8_t spp_rx_char_uuid[16] =       {0xad,0xae,0x02,0x5f,0x93,0xe6,0x4e,0xa7,0xa6,0xf4,0x97,0xad,0x02,0x59,0x00,0x00};
static uint8_t spp_tx_char_uuid[16] =       {0xad,0xae,0x02,0x5f,0x93,0xe6,0x4e,0xa7,0xa6,0xf4,0x97,0xad,0x03,0x59,0x00,0x00};

static struct {
    uint8_t data[64];
    uint32_t len;
} scan;


uint8_t *ble_gap_get_scan_data() {
    memset(scan.data, 0, sizeof(scan.data));
    scan.len = 0;
    scan.data[scan.len++] = 0x11;    // number of following bytes
    scan.data[scan.len++] = 0x07;    // packet type (7 = complete list of custom services (128 bits uuid))
    memcpy(&scan.data[scan.len], spp_service_uuid, sizeof(spp_service_uuid));
    scan.len += sizeof(spp_service_uuid);
    return (scan.data);
}


uint32_t ble_gap_get_scan_data_len() {

    return (scan.len);
}


static char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req) {
    char *auth_str = NULL;
    switch(auth_req) {
        case ESP_LE_AUTH_NO_BOND: auth_str = "ESP_LE_AUTH_NO_BOND"; break;
        case ESP_LE_AUTH_BOND: auth_str = "ESP_LE_AUTH_BOND"; break;
        case ESP_LE_AUTH_REQ_MITM: auth_str = "ESP_LE_AUTH_REQ_MITM"; break;
        case ESP_LE_AUTH_REQ_BOND_MITM: auth_str = "ESP_LE_AUTH_REQ_BOND_MITM"; break;
        case ESP_LE_AUTH_REQ_SC_ONLY: auth_str = "ESP_LE_AUTH_REQ_SC_ONLY"; break;
        case ESP_LE_AUTH_REQ_SC_BOND: auth_str = "ESP_LE_AUTH_REQ_SC_BOND"; break;
        case ESP_LE_AUTH_REQ_SC_MITM: auth_str = "ESP_LE_AUTH_REQ_SC_MITM"; break;
        case ESP_LE_AUTH_REQ_SC_MITM_BOND: auth_str = "ESP_LE_AUTH_REQ_SC_MITM_BOND"; break;
        default: auth_str = "INVALID BLE AUTH REQ"; break;
   }

   return auth_str;
}


static char *esp_key_type_to_str(esp_ble_key_type_t key_type) {
    char *key_str = NULL;
    switch(key_type) {
        case ESP_LE_KEY_NONE: key_str = "ESP_LE_KEY_NONE"; break;
        case ESP_LE_KEY_PENC: key_str = "ESP_LE_KEY_PENC"; break;
        case ESP_LE_KEY_PID: key_str = "ESP_LE_KEY_PID"; break;
        case ESP_LE_KEY_PCSRK: key_str = "ESP_LE_KEY_PCSRK"; break;
        case ESP_LE_KEY_PLK: key_str = "ESP_LE_KEY_PLK"; break;
        case ESP_LE_KEY_LLK: key_str = "ESP_LE_KEY_LLK"; break;
        case ESP_LE_KEY_LENC: key_str = "ESP_LE_KEY_LENC"; break;
        case ESP_LE_KEY_LID: key_str = "ESP_LE_KEY_LID"; break;
        case ESP_LE_KEY_LCSRK: key_str = "ESP_LE_KEY_LCSRK"; break;
        default: key_str = "INVALID BLE KEY TYPE"; break;
   }

   return key_str;
}


void ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {

    esp_err_t err;
    //esp_ble_gatts_cb_param_t *p_data = (esp_ble_gatts_cb_param_t *) param;

    switch (event) {

        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            ESP_LOGD(TAG, "ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT");
            //esp_ble_gap_start_advertising(&spp_adv_params);
            break;
    
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            ESP_LOGD(TAG, "ESP_GAP_BLE_ADV_START_COMPLETE_EVT");
            // err = param->adv_start_cmpl.status;
            // if(err != ESP_BT_STATUS_SUCCESS)
            //     ESP_LOGE(TAG, "Advertising start failed: %s", esp_err_to_name(err));
            break;

        case ESP_GAP_BLE_SEC_REQ_EVT:            
            ESP_LOGD(TAG, "ESP_GAP_BLE_SEC_REQ_EVT");
            //esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);            
            break;

        case ESP_GAP_BLE_NC_REQ_EVT:
            //esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
            ESP_LOGD(   TAG, 
                        "ESP_GAP_BLE_NC_REQ_EVT, passkey notify number %ld",
                        param->ble_security.key_notif.passkey
                    );
            break;

        case ESP_GAP_BLE_AUTH_CMPL_EVT:

            if(!param->ble_security.auth_cmpl.success) {
                ESP_LOGE(   TAG,
                            "pair status = fail, reason = 0x%02x",
                            param->ble_security.auth_cmpl.fail_reason
                        );
                
                // #TODO: disconnect when auth fail
                //esp_ble_gap_disconnect(p_data->connect.remote_bda);
            }
            else {
                ESP_LOGI(   TAG, 
                            "pair status = success, auth mode = %s",
                            esp_auth_req_to_str(param->ble_security.auth_cmpl.auth_mode)
                        );
            }
            break;

        case ESP_GAP_BLE_KEY_EVT:
            ESP_LOGD(   TAG, 
                        "ESP_GAP_BLE_KEY_EVT key type = %s", 
                        esp_key_type_to_str(param->ble_security.ble_key.key_type));
            break;

        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGD(TAG, "ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT");
            break;

        default:
            ESP_LOGD(TAG, "gap event ??? %d", event);
            break;
    }

}


