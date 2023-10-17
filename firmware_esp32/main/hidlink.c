#include "main.h"

#define TAG          "HIDLINK"

char *dev_name = HIDLINK_DEVICE_NAME;

typedef struct {
    hidlink_state_t state;
    hidlink_status_t status;
    QueueHandle_t command_queue;
    
    struct {
        uint32_t index;
        esp_bd_addr_t bd_addr[HIDLINK_DEVICE_BD_ADDR_LIST_LEN];
    } full_device_list;
    
    struct {
        uint32_t index;
        esp_bd_addr_t bd_addr[HIDLINK_DEVICES_NUMBER];
        char name[HIDLINK_DEVICES_NUMBER][ESP_BT_GAP_MAX_BDNAME_LEN];
    } hid_list;

    struct {
        hidlink_protocol_state_t state;
        uint8_t *data;
        uint32_t push;
        uint32_t data_count;
    } rx;

    struct {
        uint32_t len;
        uint32_t index;
        uint32_t task;
        uint8_t *data;
    } tx;

    struct {
        esp_gatt_if_t gatts_if;
        uint16_t conn_id;
        uint16_t chr_handle;
        esp_bd_addr_t remote_bda;
        uint16_t mtu_size;
        union {
            uint32_t val;
            struct {
                uint32_t notify_enable:1;
                uint32_t indicate_enable:1;
            } bits;
        } flags;
    } ble;
    
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

    hidlink.rx.state = HIDLINK_PROTOCOL_STATE_HEADER;
    hidlink.rx.data = NULL;
    hidlink.rx.push = 0;
    hidlink.rx.data = (uint8_t *) pvPortMalloc(HIDLINK_PROTOCOL_BUFFER_LEN * sizeof(uint8_t));
    if (hidlink.rx.data == NULL) {
        ESP_LOGE(TAG, "rx data malloc error!");
    }
    hidlink.tx.data = (uint8_t *) pvPortMalloc(HIDLINK_PROTOCOL_BUFFER_LEN * sizeof(uint8_t));
    if (hidlink.tx.data == NULL) {
        ESP_LOGE(TAG, "tx data malloc error!");
    }


    hidlink.status = HIDLINK_STATUS_IDLE;
    hidlink.state = HIDLINK_STATE_API_INIT;
}


static void hidlink_clear_full_device_list() {
    
    hidlink.full_device_list.index = 0;
    memset(hidlink.full_device_list.bd_addr, 0, sizeof(hidlink.full_device_list.bd_addr));
    ESP_LOGD(TAG, "clearing %d bytes", sizeof(hidlink.full_device_list.bd_addr));
}


static void hidlink_clear_hid_list() {

    hidlink.hid_list.index = 0;
    memset(hidlink.hid_list.bd_addr, 0, sizeof(hidlink.hid_list.bd_addr));
    memset(hidlink.hid_list.name, 0, sizeof(hidlink.hid_list.name));
    ESP_LOGD(TAG, "clearing %d + %d bytes", sizeof(hidlink.hid_list.bd_addr), sizeof(hidlink.hid_list.name));
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
                    // api init success
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
                        if (hidlink.status != HIDLINK_STATUS_SCANNING) {
                            ESP_LOGI(TAG, "%s, HIDLINK_COMMAND_SCAN_START", __func__);
                            hidlink_clear_full_device_list();
                            hidlink_clear_hid_list();
                            hidlink.status = HIDLINK_STATUS_SCANNING;
                            esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
                        }
                    }
                    else if (command == HIDLINK_COMMAND_SCAN_STOP) {
                        if (hidlink.status == HIDLINK_STATUS_SCANNING) {
                            ESP_LOGI(TAG, "%s, HIDLINK_COMMAND_SCAN_STOP", __func__);
                            esp_bt_gap_cancel_discovery();
                        }
                    }
                    else if (command == HIDLINK_COMMAND_SCAN_DONE) {
                        ESP_LOGI(TAG, "%s, HIDLINK_COMMAND_SCAN_DONE", __func__);
                        hidlink.status = HIDLINK_STATUS_IDLE;

                        ESP_LOGI(TAG, "hid devices found during scan: %lu", hidlink.hid_list.index);
                        
                        if (hidlink.hid_list.index != 0) {

                            uint32_t i;
                            char bda_str[18];

                            for(i = 0; i < hidlink.hid_list.index; i++) {
                                ESP_LOGI(TAG, "%02lu: %s [%s]", 
                                    i + 1,
                                    hidlink.hid_list.name[i],
                                    bda2str(hidlink.hid_list.bd_addr[i], bda_str, 18));
                            }
                        }
                    
                        // #TODO: send results to app
                    }
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


bool hidlink_check_device_already_discovered(esp_bd_addr_t *device_bd_addr) {

    bool ret_val = false;
    uint32_t i;

    for (i = 0; i < hidlink.full_device_list.index; i++) {

        if (memcmp(&hidlink.full_device_list.bd_addr[i], device_bd_addr, sizeof(esp_bd_addr_t)) == 0) {
            ret_val = true;
            break;
        }
    }

    return (ret_val);
}


void hidlink_add_discovered_device(esp_bd_addr_t *device_bd_addr) {

    if (hidlink.full_device_list.index < HIDLINK_DEVICE_BD_ADDR_LIST_LEN) {
        memcpy(&hidlink.full_device_list.bd_addr[hidlink.full_device_list.index++], device_bd_addr, sizeof(esp_bd_addr_t));
    }
}


void hidlink_add_hid_device(esp_bd_addr_t *bd_addr, char *name) {

    uint32_t i;
    uint32_t name_len;

    i = hidlink.hid_list.index;

    name_len = strlen(name);

    if (name_len > (ESP_BT_GAP_MAX_BDNAME_LEN - 1))
        name_len = ESP_BT_GAP_MAX_BDNAME_LEN - 1;

    if (i < HIDLINK_DEVICES_NUMBER) {

        memcpy(&hidlink.hid_list.bd_addr[i], bd_addr, sizeof(esp_bd_addr_t));
        memset(&hidlink.hid_list.name[i], 0, ESP_BT_GAP_MAX_BDNAME_LEN);
        memcpy(&hidlink.hid_list.name[i], name, name_len);
        hidlink.hid_list.index++;
    }
}



void hidlink_ble_indicate() {

    uint32_t byte_to_send;

    if(hidlink.tx.len != 0) {

        // still got data to send

        if(hidlink.tx.len <= (hidlink.ble.mtu_size - 8)) {

            // if amount of data to send is less than mtu, send everything

            byte_to_send = hidlink.tx.len;
        }
        else {

            // if amout of data to send is greater than mtu, send mtu

            byte_to_send = (hidlink.ble.mtu_size - 8);
        }

        hidlink.tx.len -= byte_to_send;

        if(hidlink.ble.flags.bits.notify_enable || hidlink.ble.flags.bits.indicate_enable) {
            
            esp_ble_gatts_send_indicate(
                hidlink.ble.gatts_if,
                hidlink.ble.conn_id,
                hidlink.ble.chr_handle,
                byte_to_send,
                &hidlink.tx.data[hidlink.tx.index],
                true);

            ESP_LOG_BUFFER_HEX_LEVEL(TAG, &hidlink.tx.data[hidlink.tx.index], byte_to_send, ESP_LOG_DEBUG);

            hidlink.tx.index += byte_to_send;

            ESP_LOGV(TAG, "%s, %lu sent, %lu remaining", 
                __func__, 
                byte_to_send, 
                hidlink.tx.len);
        }
        // #TODO: enable write without indication or notify
        else {

        //     esp_ble_gatts_send_response();

            ESP_LOGD(TAG, "%s, indicate and notify off", __func__);

            hidlink.tx.len = 0;
        }
    }
}
            


static void hidlink_ble_ack_response(uint8_t ack_val) {

    uint8_t checksum = 0;
    uint32_t i;

    hidlink.tx.data[0] = 0x3c;
    hidlink.tx.data[1] = hidlink.rx.data[1];
    hidlink.tx.data[2] = 1;
    hidlink.tx.data[3] = ack_val;

    for (i = 0; i < 4; i++) {
        checksum += hidlink.tx.data[i];
    }

    hidlink.tx.data[4] = (checksum ^ ((uint8_t) 0xff)) + 1;

    hidlink.tx.len = 5;
    hidlink.tx.index = 0;

    hidlink_ble_indicate();
}


static void hidlink_ble_command_get_status() {

    ESP_LOGI(TAG, "%s", __func__);
    hidlink_ble_ack_response(hidlink.status);
}


static void hidlink_ble_command_start_scan() {

    ESP_LOGI(TAG, "%s", __func__);
    hidlink_set_command(HIDLINK_COMMAND_SCAN_START);
    hidlink_ble_ack_response(0x06);
}


static void hidlink_ble_command_stop_scan() {

    ESP_LOGI(TAG, "%s", __func__);
    hidlink_set_command(HIDLINK_COMMAND_SCAN_STOP);
    hidlink_ble_ack_response(0x06);
}


static void hidlink_ble_command_attach_to_peripheral() {

    ESP_LOGI(TAG, "%s", __func__);
    // #TODO: attach to peripheral
    hidlink_ble_ack_response(0x06);
}


void hidlink_ble_protocol_parser(uint8_t *data, uint32_t len) {

    uint8_t rx_data;

    if (hidlink.rx.data == NULL) {
        ESP_LOGE(TAG, "rx buffer invalid pointer!");
    }
    else {

        while (len--) {

            rx_data = *data++;

            if (hidlink.rx.push < HIDLINK_PROTOCOL_BUFFER_LEN) {
                hidlink.rx.data[hidlink.rx.push++] = rx_data;
            }

            switch (hidlink.rx.state) {

                case HIDLINK_PROTOCOL_STATE_HEADER: {
                    if (rx_data == 0x3e) {
                        hidlink.rx.data[0] = rx_data;
                        hidlink.rx.push = 1;
                        hidlink.rx.state = HIDLINK_PROTOCOL_STATE_COMMAND;
                    }
                    break;
                }

                case HIDLINK_PROTOCOL_STATE_COMMAND: {
                    if ((hidlink_protocol_command_t) rx_data < HIDLINK_PROTOCOL_COMMAND_MAX) {
                        hidlink.rx.state = HIDLINK_PROTOCOL_STATE_LEN;
                    }
                    else {
                        hidlink.rx.state = HIDLINK_PROTOCOL_STATE_HEADER;
                    }
                    break;
                }

                case HIDLINK_PROTOCOL_STATE_LEN: {
                    if (rx_data == 0) {
                        hidlink.rx.state = HIDLINK_PROTOCOL_STATE_CHECKSUM;
                    }
                    else if (rx_data < (HIDLINK_PROTOCOL_BUFFER_LEN - 3)) {
                        hidlink.rx.data_count = rx_data;
                        hidlink.rx.state = HIDLINK_PROTOCOL_STATE_DATA;
                    }
                    else {
                        hidlink.rx.state = HIDLINK_PROTOCOL_STATE_HEADER;
                    }

                    break;
                }

                case HIDLINK_PROTOCOL_STATE_DATA: {
                    if (--hidlink.rx.data_count == 0) {
                        hidlink.rx.state = HIDLINK_PROTOCOL_STATE_CHECKSUM;
                    }
                    break;
                }

                case HIDLINK_PROTOCOL_STATE_CHECKSUM: {

                    uint32_t i;
                    uint8_t checksum = 0;

                    for (i = 0; i < hidlink.rx.push; i++) {
                        checksum += hidlink.rx.data[i];
                    }

                    if (checksum == 0) {

                        switch ((hidlink_protocol_command_t) hidlink.rx.data[1]) {

                            case HIDLINK_PROTOCOL_COMMAND_GET_STATUS: {
                                hidlink_ble_command_get_status();
                                break;
                            }

                            case HIDLINK_PROTOCOL_COMMAND_START_SCAN: {
                                hidlink_ble_command_start_scan();
                                break;
                            }

                            case HIDLINK_PROTOCOL_COMMAND_STOP_SCAN: {
                                hidlink_ble_command_stop_scan();
                                break;
                            }

                            case HIDLINK_PROTOCOL_COMMAND_ATTACH_TO_PERIPHERAL: {
                                hidlink_ble_command_attach_to_peripheral();
                                break;
                            }

                            default: {
                                ESP_LOGW(TAG, "invalid command!");
                            }
                        }
                    }

                    hidlink.rx.state = HIDLINK_PROTOCOL_STATE_HEADER;

                    break;
                }
            }
        }
    }    
}


void hidlink_ble_set_char_handle(uint16_t char_handle) {

    hidlink.ble.chr_handle = char_handle;
}


void hidlink_ble_set_mtu(uint16_t mtu) {

    hidlink.ble.mtu_size = mtu;
}


void hidlink_set_ble_data(esp_gatt_if_t gatts_if, uint16_t conn_id, esp_bd_addr_t *bda) {

    hidlink.ble.conn_id = conn_id;
    hidlink.ble.gatts_if = gatts_if;
    memcpy(&hidlink.ble.remote_bda, bda, sizeof(esp_bd_addr_t));
}


void hidlink_set_rx_cccd(uint16_t val) {

    if(val == 1) {
        ESP_LOGW(TAG, "%s, notify enabled", __func__);
        hidlink.ble.flags.bits.notify_enable = 1;
        hidlink.ble.flags.bits.indicate_enable = 0;
    }
    else if(val == 2) {
        ESP_LOGW(TAG, "%s, indicate enabled", __func__);
        hidlink.ble.flags.bits.notify_enable = 0;
        hidlink.ble.flags.bits.indicate_enable = 1;
    }
    else {
        ESP_LOGW(TAG, "%s, notify and indicate disabled", __func__);
        hidlink.ble.flags.bits.notify_enable = 0;
        hidlink.ble.flags.bits.indicate_enable = 0;
    }
}