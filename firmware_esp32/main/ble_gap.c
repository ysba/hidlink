#include "main.h"

static const char *TAG = "BLE_GAP";


// https://docs.silabs.com/bluetooth/latest/general/adv-and-scanning/bluetooth-adv-data-basics


static struct {
    uint8_t data[31];
    uint32_t len;
} scan;


static struct {
    uint8_t data[31];
    uint32_t len;
} advertising;


uint8_t *ble_gap_get_scan_data() {
    memset(scan.data, 0, sizeof(scan.data));
    scan.len = 0;
    scan.data[scan.len++] = 0x11;    // number of following bytes
    scan.data[scan.len++] = 0x07;    // packet type (7 = complete list of custom services (128 bits uuid))
    memcpy(&scan.data[scan.len], cdtp_service_uuid, sizeof(cdtp_service_uuid));
    scan.len += sizeof(cdtp_service_uuid);
    return (scan.data);
}


uint32_t ble_gap_get_scan_len() {

    return (scan.len);
}


uint8_t *ble_gap_get_advertising_data() {
    memset(advertising.data, 0, sizeof(advertising.data));
    advertising.len = 0;
    advertising.data[advertising.len++] = 0x02;
    advertising.data[advertising.len++] = 0x01;
    advertising.data[advertising.len++] = 0x06;
    advertising.data[advertising.len++] = strlen(HIDLINK_DEVICE_NAME) + 1;
    advertising.data[advertising.len++] = 0x09;
    strcpy((char *) &advertising.data[advertising.len], HIDLINK_DEVICE_NAME);
    advertising.len += strlen(HIDLINK_DEVICE_NAME);

    ESP_LOG_BUFFER_HEX_LEVEL(TAG, advertising.data, advertising.len, ESP_LOG_DEBUG);
    return (advertising.data);
}


uint32_t ble_gap_get_advertising_len() {

    return (advertising.len);
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

    switch (event) {

        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT: {
            ESP_LOGD(TAG, "%s, ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT", __func__);
            esp_ble_gap_start_advertising(&cdtp_adv_params);
            break;
        }
    
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT: {
            ESP_LOGD(TAG, "%s, ESP_GAP_BLE_ADV_START_COMPLETE_EVT", __func__);
            err = param->adv_start_cmpl.status;
            if(err != ESP_BT_STATUS_SUCCESS)
                ESP_LOGW(TAG, "%s, advertising start failed: %s", 
                    __func__,
                    esp_err_to_name(err));
            break;
        }

        case ESP_GAP_BLE_SEC_REQ_EVT: {
            ESP_LOGD(TAG, "%s, ESP_GAP_BLE_SEC_REQ_EVT", __func__);
            esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);            
            break;
        }

        case ESP_GAP_BLE_NC_REQ_EVT: {
            ESP_LOGD(TAG, "%s, ESP_GAP_BLE_NC_REQ_EVT, passkey notify number %ld", 
                __func__,
                param->ble_security.key_notif.passkey);
            esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
            break;
        }

        case ESP_GAP_BLE_AUTH_CMPL_EVT: {
            if(!param->ble_security.auth_cmpl.success) {
                ESP_LOGE(TAG, "%s, pair status = fail, reason = 0x%02x",
                    __func__,
                    param->ble_security.auth_cmpl.fail_reason);
                
                // #TODO: disconnect when auth fail
                //esp_ble_gap_disconnect(p_data->connect.remote_bda);
            }
            else {
                ESP_LOGI(TAG, "%s, pair status = success, auth mode = %s",
                    __func__,
                    esp_auth_req_to_str(param->ble_security.auth_cmpl.auth_mode));
            }
            break;
        }

        case ESP_GAP_BLE_KEY_EVT: {
            ESP_LOGD(TAG, "%s, ESP_GAP_BLE_KEY_EVT key type = %s",
                __func__,
                esp_key_type_to_str(param->ble_security.ble_key.key_type));
            break;
        }

        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT: {
            ESP_LOGD(TAG, "%s, ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT", __func__);
            break;
        }

        case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT: {
            ESP_LOGD(TAG, "%s, ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT", __func__);
            break;
        }

        default: {
            ESP_LOGD(TAG, "UNEXPECTED GAP EVENT: %d", event);
            break;
        }
    }

}
