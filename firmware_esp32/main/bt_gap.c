#include "main.h"

static const char *TAG = "BT_GAP";


static char bda_str[18];


static char *bda2str(esp_bd_addr_t bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}


void bt_gap_event_handler(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {

    // app_gap_cb_t *p_dev = &m_dev_info;
    // char bda_str[18];
    // char uuid_str[37];

    switch (event) {

        case ESP_BT_GAP_DISC_RES_EVT: {
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_DISC_RES_EVT", __func__);
            //update_device_info(param);

            if (hidlink_check_device_already_discovered(&param->disc_res.bda) == false) {
                ESP_LOGI(TAG, "new device: %s", bda2str(param->disc_res.bda, bda_str, 18));
                hidlink_add_discovered_device(&param->disc_res.bda);
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
            }
            else {
                ESP_LOGD(TAG, "%s, unknown discovery state: %d", __func__, param->disc_st_chg.state);
            }
            
            //     if ( (p_dev->state == APP_GAP_STATE_DEVICE_DISCOVER_COMPLETE ||
            //             p_dev->state == APP_GAP_STATE_DEVICE_DISCOVERING)
            //             && p_dev->dev_found) {
            //         p_dev->state = APP_GAP_STATE_SERVICE_DISCOVERING;
            //         ESP_LOGI(TAG, "Discover services ...");
            //         esp_bt_gap_get_remote_services(p_dev->bda);
            //     }
            // } else if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STARTED) {
            //     ESP_LOGI(TAG, "Discovery started.");
            // }
            break;
        }

        case ESP_BT_GAP_RMT_SRVCS_EVT: {
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_RMT_SRVCS_EVT", __func__);
            // if (memcmp(param->rmt_srvcs.bda, p_dev->bda, ESP_BD_ADDR_LEN) == 0 &&
            //         p_dev->state == APP_GAP_STATE_SERVICE_DISCOVERING) {
            //     p_dev->state = APP_GAP_STATE_SERVICE_DISCOVER_COMPLETE;
            //     if (param->rmt_srvcs.stat == ESP_BT_STATUS_SUCCESS) {
            //         ESP_LOGI(TAG, "Services for device %s found",  bda2str(p_dev->bda, bda_str, 18));
            //         for (int i = 0; i < param->rmt_srvcs.num_uuids; i++) {
            //             esp_bt_uuid_t *u = param->rmt_srvcs.uuid_list + i;
            //             ESP_LOGI(TAG, "--%s", uuid2str(u, uuid_str, 37));
            //         }

            //         esp_bt_gap_read_remote_name(p_dev->bda);
            //     } else {
            //         ESP_LOGI(TAG, "Services for device %s not found",  bda2str(p_dev->bda, bda_str, 18));
            //     }
            // }
            break;
        }
    
        case ESP_BT_GAP_READ_REMOTE_NAME_EVT: {
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_READ_REMOTE_NAME_EVT", __func__);
            // ESP_LOGI(TAG, "%s, name: %s", __func__, param->read_rmt_name.rmt_name);
            // esp_bt_hid_host_connect(p_dev->bda);
            break;
        }

        case ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT: {
            ESP_LOGD(TAG, "%s, event ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT", __func__);
            
            // ESP_LOGI(TAG, "%s, bda %s, stat %d, handle %d", 
            //     __func__, 
            //     bda2str(param->acl_conn_cmpl_stat.bda, bda_str, 18),
            //     param->acl_conn_cmpl_stat.stat,
            //     param->acl_conn_cmpl_stat.handle);

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