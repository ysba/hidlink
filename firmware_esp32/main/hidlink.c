#include "main.h"

#define TAG          "HIDLINK"

char *dev_name = HIDLINK_DEVICE_NAME;

typedef struct {
    hidlink_state_t state;
    QueueHandle_t command_queue;
    union {
        uint32_t val;
        struct {
            bool scan_start:1;
        } bits;
    } flags;
} hidlink_t;


static hidlink_t hidlink;


static void hidlink_init() {
    memset(&hidlink, 0, sizeof(typeof(hidlink)));
    
    hidlink.command_queue = xQueueCreate(10, sizeof(hidlink_command_t));

    if (hidlink.command_queue == NULL) {
        ESP_LOGE(TAG, "%s, queue creation failed", __func__);
    }

    hidlink.state = HIDLINK_STATE_API_INIT;
}


void hidlink_main_task() {

    esp_err_t err;

    hidlink_init();

    while (1) {

        switch(hidlink.state) {   
            
            case HIDLINK_STATE_API_INIT: {

                ESP_LOGD(TAG, "%s, HIDLINK_STATE_API_INIT, start", __func__);

                esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
                esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();

                err = nvs_flash_init();
                if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                    ESP_LOGW(TAG, "%s, nvs_flash_init failed, err %s", __func__, esp_err_to_name(err));
                    nvs_flash_erase();
                    err = nvs_flash_init();    
                }
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "nvs_flash_init failed:  %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }

                if ((err = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_controller_init failed:  %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_bt_controller_enable(ESP_BT_MODE_BTDM)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_controller_enable failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_bluedroid_init_with_cfg(&bluedroid_cfg)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bluedroid_init_with_cfg failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_bluedroid_enable()) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bluedroid_enable failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_bt_gap_register_callback(bt_gap_event_handler)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_gap_register_callback failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_bt_hid_host_register_callback(bt_hid_host_event_handler)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_hid_host_register_callback failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_ble_gatts_register_callback(ble_gatts_event_handler)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_ble_gatts_register_callback failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_ble_gap_register_callback(ble_gap_event_handler)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_ble_gap_register_callback failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_bt_dev_set_device_name(dev_name)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_dev_set_device_name failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_ble_gap_set_device_name(dev_name)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_ble_gap_set_device_name failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_NON_DISCOVERABLE)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_gap_set_scan_mode failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_bt_hid_host_init()) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_hid_host_init failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_ble_gatts_app_register(ESP_SPP_APP_ID)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_ble_gatts_app_register failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_ble_gap_config_scan_rsp_data_raw(
                        ble_gap_get_scan_data(),
                        ble_gap_get_scan_len())) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_ble_gap_config_scan_rsp_data_raw failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                }
                else if ((err = esp_ble_gap_config_adv_data_raw(
                        ble_gap_get_advertising_data(),
                        ble_gap_get_advertising_len())) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_ble_gap_config_scan_rsp_data_raw failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_API_DEINIT;
                } 
                else {
                    // success
                    ESP_LOGD(TAG, "%s, HIDLINK_STATE_API_INIT, ok", __func__);
                    hidlink.state = HIDLINK_STATE_IDLE;
                }
                break;
            }        

            case HIDLINK_STATE_API_DEINIT: {

                ESP_LOGD(TAG, "%s, HIDLINK_STATE_API_DEINIT, start", __func__);
                
                if ((err = esp_bluedroid_disable()) != ESP_OK) {
                    ESP_LOGW(TAG, "%s, esp_bluedroid_disable failed: %s", __func__, esp_err_to_name(err));
                }

                if ((err = esp_bluedroid_deinit()) != ESP_OK) {
                    ESP_LOGW(TAG, "%s, esp_bluedroid_deinit failed: %s", __func__, esp_err_to_name(err));
                }

                if ((err = esp_bt_controller_disable()) != ESP_OK) {
                    ESP_LOGW(TAG, "%s, esp_bt_controller_disable failed: %s", __func__, esp_err_to_name(err));
                }

                if ((err = esp_bt_controller_deinit()) != ESP_OK) {
                    ESP_LOGW(TAG, "%s, esp_bt_controller_deinit failed: %s", __func__, esp_err_to_name(err));
                }

                if ((err = nvs_flash_deinit()) != ESP_OK) {
                    ESP_LOGW(TAG, "%s, nvs_flash_deinit failed: %s", __func__, esp_err_to_name(err));
                }

                ESP_LOGD(TAG, "%s, HIDLINK_STATE_API_DEINIT, 5s delay then restart", __func__);
                vTaskDelay(pdMS_TO_TICKS(5000));
                hidlink_init();
                break;
            }
            
            case HIDLINK_STATE_IDLE: {

                hidlink_command_t command;

                if (xQueueReceive(hidlink.command_queue, &command, pdMS_TO_TICKS(5000)) == pdTRUE) {

                    if (command == HIDLINK_COMMAND_SCAN_START) {
                        ESP_LOGI(TAG, "%s, HIDLINK_COMMAND_SCAN_START", __func__);
                        esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
                    }
                    else if (command == HIDLINK_COMMAND_SCAN_STOP) {
                        ESP_LOGI(TAG, "%s, HIDLINK_COMMAND_SCAN_STOP", __func__);
                        esp_bt_gap_cancel_discovery();
                        // send results to app
                    }
                }
                else {

                    //ESP_LOGD(TAG, "%s, idle loop", __func__);
                }

                break;
            }        
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void hidlink_set_command(hidlink_command_t command) {
    
    xQueueSend(hidlink.command_queue, &command, portMAX_DELAY);
}