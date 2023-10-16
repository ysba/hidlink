#include "main.h"

static const char *TAG = "BT_GAP";


static char bda_str[18];


char *bda2str(esp_bd_addr_t bda, char *str, size_t size) {
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}


void bt_gap_event_handler(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {

    switch (event) {

        case ESP_BT_GAP_DISC_RES_EVT: {
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_DISC_RES_EVT", __func__);

            if (hidlink_check_device_already_discovered(&param->disc_res.bda) == false) {

                uint32_t i;
                esp_bt_gap_dev_prop_t *property;
                uint32_t cod = 0;
                int32_t rssi = -129; /* invalid value */
                uint8_t *bdname = NULL;
                uint32_t bdname_len = 0;
                uint8_t *eir = NULL;
                uint8_t eir_len = 0;
                
                hidlink_add_discovered_device(&param->disc_res.bda);

                ESP_LOGI(TAG, "device found: %s", bda2str(param->disc_res.bda, bda_str, 18));
                ESP_LOGD(TAG, "number of properties: %d", param->disc_res.num_prop);
                
                for (i = 0; i < param->disc_res.num_prop; i++) {
                    
                    property = param->disc_res.prop + i;

                    switch (property->type) {

                        case ESP_BT_GAP_DEV_PROP_COD: {
                            cod = *(uint32_t *)(property->val);
                            ESP_LOGD(TAG, "--class of device: 0x%"PRIx32, cod);
                            break;
                        }

                        case ESP_BT_GAP_DEV_PROP_RSSI: {
                            rssi = *(int8_t *)(property->val);
                            ESP_LOGD(TAG, "--rssi: %"PRId32, rssi);
                            break;
                        }

                        case ESP_BT_GAP_DEV_PROP_BDNAME: {
                            if (property->len < ESP_BT_GAP_MAX_BDNAME_LEN)
                                bdname_len = property->len;
                            else
                                bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
                            bdname = (uint8_t *)(property->val);
                            ESP_LOGD(TAG, "--name: %s", bdname);
                            break;
                        }

                        case ESP_BT_GAP_DEV_PROP_EIR: {
                            eir_len = property->len;
                            eir = (uint8_t *)(property->val);
                            ESP_LOGD(TAG, "--extended inquiry response:");
                            ESP_LOG_BUFFER_HEX_LEVEL(TAG, eir, eir_len, ESP_LOG_DEBUG);
                            break;
                        }

                        default: {
                            ESP_LOGD(TAG, "--unexpected property: %d", property->type);
                            break;
                        }
                    }
                }

                if (!esp_bt_gap_is_valid_cod(cod)) {

                    ESP_LOGW(TAG, "invalid class of device: 0x%04lx", cod);
                    break;
                }

                if (esp_bt_gap_get_cod_major_dev(cod) != ESP_BT_COD_MAJOR_DEV_PERIPHERAL) {
                    
                    ESP_LOGW(TAG, "device is not PERIPHERAL");
                    break;
                }

                
                if (bdname_len == 0) {
                    ESP_LOGD(TAG, "name not available, requesting remote device name");
                    esp_bt_gap_read_remote_name(param->disc_res.bda);
                }
                else {
                    hidlink_add_hid_device(&param->disc_res.bda, (char *) bdname);
                    ESP_LOGI(TAG, "device added %s to hidlink list!", bda2str(param->disc_res.bda, bda_str, 18));
                }
            }
            else {
                ESP_LOGD(TAG, "device already discovered: %s", bda2str(param->disc_res.bda, bda_str, 18));
            }

            break;
        }

        case ESP_BT_GAP_DISC_STATE_CHANGED_EVT: {
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_DISC_STATE_CHANGED_EVT", __func__);

            if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STARTED) {
                ESP_LOGD(TAG, "%s, device discovery started", __func__);
            }
            else if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
                ESP_LOGD(TAG, "%s, device discovery stopped", __func__);
                hidlink_set_command(HIDLINK_COMMAND_SCAN_DONE);
            }
            else {
                ESP_LOGD(TAG, "%s, unknown discovery state: %d", __func__, param->disc_st_chg.state);
            }
            break;
        }

        case ESP_BT_GAP_RMT_SRVCS_EVT: {
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_RMT_SRVCS_EVT", __func__);    
            break;
        }
    
        case ESP_BT_GAP_READ_REMOTE_NAME_EVT: {
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_READ_REMOTE_NAME_EVT", __func__);
            ESP_LOGI(TAG, "--name: %s", param->read_rmt_name.rmt_name);
            hidlink_add_hid_device(&param->read_rmt_name.bda, (char *) param->read_rmt_name.rmt_name);
            ESP_LOGI(TAG, "device added %s to hidlink list!", bda2str(param->read_rmt_name.bda, bda_str, 18));
            break;
        }

        case ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT: {
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT", __func__);
            break;
        }

        case ESP_BT_GAP_MODE_CHG_EVT:
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_MODE_CHG_EVT", __func__);
            break;

        case ESP_BT_GAP_RMT_SRVC_REC_EVT:
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_RMT_SRVC_REC_EVT", __func__);
            break;

        case ESP_BT_GAP_AUTH_CMPL_EVT:
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_AUTH_CMPL_EVT", __func__);
            break;

        case ESP_BT_GAP_CONFIG_EIR_DATA_EVT:
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_CONFIG_EIR_DATA_EVT", __func__);
            break;

        default: {
            ESP_LOGW(TAG, "event: %d", event);
            break;
        }
    }

}
