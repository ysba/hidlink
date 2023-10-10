#include "main.h"

#define TAG          "hidlink"

char *dev_name = "hidlink";

typedef struct {
    hidlink_state_t state;
    // union {
    //     uint16_t val;
    //     struct {
    //         bool timer_skip:1;
    //     } bits;
    // } flags;
} hidlink_t;


static hidlink_t hidlink;


static void hidlink_init() {
    memset(&hidlink, 0, sizeof(hidlink));
    /* place other initialization code here */
}

void mock() {
    ESP_LOGW(TAG, "test");
}


void hidlink_main_task() {

    esp_err_t err;

    hidlink_init();

    while (1) {

        switch(hidlink.state) {   

            case HIDLINK_STATE_INIT_ESP_API: {
                err = nvs_flash_init();
                if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                    nvs_flash_erase();
                    err = nvs_flash_init();
                }
                
                if (err == ESP_OK) {
                    hidlink.state = HIDLINK_STATE_INIT_BT_API;
                }
                else {
                    hidlink.state = HIDLINK_STATE_DEINIT_BT_API;
                }
                break;
            }     
            
            case HIDLINK_STATE_INIT_BT_API: {

                esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
                esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();

                esp_bt_controller_mem_release(ESP_BT_MODE_BLE);

                if ((err = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_controller_init failed:  %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_DEINIT_BT_API;
                }
                else if ((err = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_controller_enable failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_DEINIT_BT_API;
                }
                else if ((err = esp_bluedroid_init_with_cfg(&bluedroid_cfg)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bluedroid_init_with_cfg failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_DEINIT_BT_API;
                }
                else if ((err = esp_bluedroid_enable()) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bluedroid_enable failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_DEINIT_BT_API;
                }
                else if ((err = esp_bt_gap_register_callback(mock)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_gap_register_callback failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_DEINIT_BT_API;
                }
                else if ((err = esp_bt_hid_host_register_callback(mock)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_hid_host_register_callback failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_DEINIT_BT_API;
                }
                else if ((err = esp_bt_dev_set_device_name(dev_name)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_dev_set_device_name failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_DEINIT_BT_API;
                }
                else if ((err = esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE)) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_gap_set_scan_mode failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_DEINIT_BT_API;
                }
                else if ((err = esp_bt_hid_host_init()) != ESP_OK) {
                    ESP_LOGE(TAG, "esp_bt_hid_host_init failed: %s", esp_err_to_name(err));
                    hidlink.state = HIDLINK_STATE_DEINIT_BT_API;
                }


                // #TODO: init ble api

                else {

                    hidlink.state = HIDLINK_STATE_INIT_BT_API;
                }
                break;
            }        
            
            case HIDLINK_STATE_IDLE: {

                // TODO: check scan command

                // TODO: try to connect to paired device
                
                break;
            }        
            
            case HIDLINK_STATE_SCAN_START: {

                esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
                hidlink.state = HIDLINK_STATE_SCAN_WAIT;
                break;
            }        
            
            case HIDLINK_STATE_SCAN_WAIT: {
                
                hidlink.state = HIDLINK_STATE_SCAN_RESULTS;
                break;
            }        
            
            case HIDLINK_STATE_SCAN_RESULTS: {
                
                hidlink.state = HIDLINK_STATE_IDLE;
                break;
            }        
            
            case HIDLINK_STATE_DEINIT_BT_API: {
                
                break;
            }
        }

    }
}
