#include "main.h"

static const char *TAG = "gap";


void bt_hid_host_callback(esp_hidh_cb_event_t event, esp_hidh_cb_param_t *param) {

    switch (event) {

        case ESP_HIDH_INIT_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_INIT_EVT", __func__);
            break;
        }

        case ESP_HIDH_DEINIT_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_DEINIT_EVT", __func__);
            break;
        }

        case ESP_HIDH_OPEN_EVT: {
            // 01, 02
            ESP_LOGD(TAG, "%s, ESP_HIDH_OPEN_EVT", __func__);
            break;
        }

        case ESP_HIDH_CLOSE_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_CLOSE_EVT", __func__);
            break;
        }

        case ESP_HIDH_GET_RPT_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_GET_RPT_EVT", __func__);
            break;
        }

        case ESP_HIDH_SET_RPT_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_SET_RPT_EVT", __func__);
            break;
        }

        case ESP_HIDH_GET_PROTO_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_GET_PROTO_EVT", __func__);
            break;
        }

        case ESP_HIDH_SET_PROTO_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_SET_PROTO_EVT", __func__);
            break;
        }

        case ESP_HIDH_GET_IDLE_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_GET_IDLE_EVT", __func__);
            break;
        }

        case ESP_HIDH_SET_IDLE_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_SET_IDLE_EVT", __func__);
            break;
        }

        case ESP_HIDH_GET_DSCP_EVT: {
            // 03
            ESP_LOGD(TAG, "%s, ESP_HIDH_GET_DSCP_EVT", __func__);
            break;
        }

        case ESP_HIDH_ADD_DEV_EVT: {
            // 04
            ESP_LOGD(TAG, "%s, ESP_HIDH_ADD_DEV_EVT", __func__);
            break;
        }

        case ESP_HIDH_RMV_DEV_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_RMV_DEV_EVT", __func__);
            break;
        }

        case ESP_HIDH_VC_UNPLUG_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_VC_UNPLUG_EVT", __func__);
            break;
        }

        case ESP_HIDH_DATA_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_DATA_EVT", __func__);
            break;
        }

        case ESP_HIDH_DATA_IND_EVT: {
            // 05, 06, 06
            ESP_LOGD(TAG, "%s, ESP_HIDH_DATA_IND_EVT", __func__);

            ESP_LOG_BUFFER_HEX_LEVEL(TAG, param->data_ind.data, param->data_ind.len, ESP_LOG_INFO);
            break;
        }

        case ESP_HIDH_SET_INFO_EVT: {
            ESP_LOGD(TAG, "%s, ESP_HIDH_SET_INFO_EVT", __func__);
            break;
        }

    }
}