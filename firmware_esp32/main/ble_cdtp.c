#include "main.h"

/* CDTP - CUSTOM DATA TRANSFER PROFILE */

static const char *TAG = "BLE_CDTP";

// 59534241-ef18-4d1f-850e-b7a87878dfa0     custom data transfer profile service uuid
// 59534241-ef18-4d1f-850e-b7a87878dfa1     custom data transfer profile data characteristic uuid

uint8_t cdtp_service_uuid[16] =     {0xa0,0xdf,0x78,0x78,0xa8,0xb7,0x0e,0x85,0x1f,0x4d,0x18,0xef,0x41,0x42,0x53,0x59};
uint8_t cdtp_data_char_uuid[16] =   {0xa1,0xdf,0x78,0x78,0xa8,0xb7,0x0e,0x85,0x1f,0x4d,0x18,0xef,0x41,0x42,0x53,0x59};

static uint16_t cdtp_handle_table[BLE_CDTP_INDEX_MAX];

#define CHAR_DECLARATION_SIZE   (sizeof(uint8_t))

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

static const uint8_t char_prop_read_write_notify = 
    ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE_NR | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

static uint8_t  cdtp_data_buffer[BLE_CDTP_BUFFER_MAX_LEN];
static uint8_t  cdtp_data_ccc[2] = {0x00, 0x00};

static const esp_gatts_attr_db_t cdtp_gatt_db[BLE_CDTP_INDEX_MAX] = {

    // CDTP service declaration
    [BLE_CDTP_INDEX_SERVICE] = {
        {
            ESP_GATT_AUTO_RSP
        }, 
        {
            ESP_UUID_LEN_16,
            (uint8_t *)&primary_service_uuid,
            ESP_GATT_PERM_READ,
            sizeof(cdtp_service_uuid),
            sizeof(cdtp_service_uuid),
            cdtp_service_uuid
        }
    },

    // CDTP data characteristic declaration
    [BLE_CDTP_INDEX_DATA_CHAR] = {
        {
            ESP_GATT_AUTO_RSP
        },
        {
            ESP_UUID_LEN_16,
            (uint8_t *)&character_declaration_uuid,
            ESP_GATT_PERM_READ,
            CHAR_DECLARATION_SIZE,
            CHAR_DECLARATION_SIZE,
            (uint8_t *)&char_prop_read_write_notify
        }
    },
    
    // CDTP data characteristic value
    [BLE_CDTP_INDEX_DATA_VAL] = {
        {
            ESP_GATT_AUTO_RSP
        },
        {
            ESP_UUID_LEN_128,
            cdtp_data_char_uuid,
            ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
            sizeof(cdtp_data_buffer),
            sizeof(cdtp_data_buffer),
            (uint8_t *) cdtp_data_buffer
        }
    },

    // CDTP data characteristic cccd - Client Characteristic Configuration Descriptor
    [BLE_CDTP_INDEX_DATA_CFG] = {
        {
            ESP_GATT_AUTO_RSP
        },
        {
            ESP_UUID_LEN_16,
            (uint8_t *)&character_client_config_uuid,
            ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
            sizeof(uint16_t),
            sizeof(cdtp_data_ccc),
            (uint8_t *)cdtp_data_ccc
        }
    },
};


esp_ble_adv_params_t cdtp_adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};


static uint8_t find_char_and_desr_index(uint16_t handle) {
    uint8_t error = 0xff;
    for(int i = 0; i < BLE_CDTP_INDEX_MAX ; i++){
        if(handle == cdtp_handle_table[i]){
            return i;
        }
    }
    return error;
}


void gatts_cdtp_cb(esp_gatts_cb_event_t event,esp_gatt_if_t gatts_if,esp_ble_gatts_cb_param_t *param) {

    esp_ble_gatts_cb_param_t *p_data = (esp_ble_gatts_cb_param_t *) param;
    uint8_t res = 0xff;

    switch (event) {

        case ESP_GATTS_REG_EVT: {
        	esp_ble_gatts_create_attr_tab(cdtp_gatt_db, gatts_if, BLE_CDTP_INDEX_MAX, BLE_CDTP_SERVICE_INSTANCE_ID);
       	    break;
        }
        

    	case ESP_GATTS_CREAT_ATTR_TAB_EVT: {

    	    if (param->add_attr_tab.status != ESP_GATT_OK) {
    	        ESP_LOGW(TAG, "ESP_GATTS_CREAT_ATTR_TAB_EVT failed, error 0x%x",
                    param->add_attr_tab.status
                );
    	    }
    	    else if (param->add_attr_tab.num_handle != BLE_CDTP_INDEX_MAX) {
    	        ESP_LOGE(TAG,
                    "ESP_GATTS_CREAT_ATTR_TAB_EVT Create attribute table abnormally, num_handle (%d) doesn't equal to HRS_IDX_NB(%d)",
                    param->add_attr_tab.num_handle, 
                    BLE_CDTP_INDEX_MAX);
    	    }
    	    else {
                ESP_LOGD(TAG, "ESP_GATTS_CREAT_ATTR_TAB_EVT ok");
    	        memcpy(cdtp_handle_table, param->add_attr_tab.handles, sizeof(cdtp_handle_table));
    	        esp_ble_gatts_start_service(cdtp_handle_table[BLE_CDTP_INDEX_SERVICE]);
    	    }
    	    break;
    	}
    	

        case ESP_GATTS_READ_EVT: {

            // ESP_LOGD(TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d", 
            //     p_data->read.conn_id, 
            //     p_data->read.trans_id, 
            //     p_data->read.handle);

            // take struct index according to handle
            res = find_char_and_desr_index(p_data->read.handle);
           
            if(res == BLE_CDTP_INDEX_DATA_VAL) {

                ESP_LOGD(TAG, "ESP_GATTS_READ_EVT");
                
                // int handle;

                // handle = dvis_get_handle_from_conn_id(p_data->connect.conn_id);

                // if(handle >= 0 && handle < DVIS_PARSER_INSTANCES) {

                //     ESP_LOGI(__func__, "read rx char");

                //     esp_gatt_rsp_t rsp;
                
                //     memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
                //     rsp.attr_value.handle = p_data->read.handle;
                //     rsp.attr_value.len = 4;
                //     rsp.attr_value.value[0] = 0x12;
                //     rsp.attr_value.value[1] = 0x34;
                //     rsp.attr_value.value[2] = 0x56;
                //     rsp.attr_value.value[3] = 0x78;

                //     esp_ble_gatts_send_response(
                //         dvis[handle].ble.gatts_if,
                //         p_data->read.conn_id,
                //         p_data->read.trans_id,
                //         ESP_GATT_OK,
                //         &rsp
                //         );
                // }
                
            }

            break;
        }
    	/* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        case ESP_GATTS_WRITE_EVT: {
    	    
            ESP_LOGD(TAG, "ESP_GATTS_WRITE_EVT");
            ESP_LOG_BUFFER_HEX_LEVEL(TAG, p_data->write.value, p_data->write.len, ESP_LOG_DEBUG);


            if (p_data->write.len >= 2) {

                if (p_data->write.value[0] == 0xaa) {

                    if (p_data->write.value[1] == 0x01) {

                        hidlink_set_command(HIDLINK_COMMAND_SCAN_START);
                    }
                    else {

                        hidlink_set_command(HIDLINK_COMMAND_SCAN_STOP);
                    }
                }
            }

            // res = find_char_and_desr_index(p_data->write.handle);

            // if(res == BLE_CDTP_INDEX_DATA_VAL) {

            //     char mock[] = "test response";

            //     esp_ble_gatts_send_indicate(
            //         gatts_if,
            //         p_data->write.conn_id,                          // uint16_t conn_id
            //         p_data->write.handle,                           // uint16_t attr_handle
            //         strlen(mock),                                   // uint16_t value_len
            //         (uint8_t *) mock,                               // uint8_t *value
            //         false);                                         // receive confirmation  

            // }




            // bool need_confirm (false = notify, true = indicate)


            // int handle = dvis_get_handle_from_conn_id(p_data->connect.conn_id);

            // if(handle >= 0 && handle < DVIS_PARSER_INSTANCES) {

            //     res = find_char_and_desr_index(p_data->write.handle);

            //     // ESP_LOGD(TAG, 
            //     //     "write, conn id %d, handle %d, res %d",
            //     //     p_data->connect.conn_id,
            //     //     handle,
            //     //     res
            //     // );

            //     //if(p_data->write.is_prep == false) {

            //         //ESP_LOGD(TAG, "write prep = false");

            //         if(res == SPP_IDX_SPP_TX_VAL) {

            //             // write to tx characteristic
            //             ESP_LOGD(TAG, "%s, ESP_GATTS_WRITE_EVT, %d bytes", 
            //                 __func__,
            //                 p_data->write.len);

            //             dvis_set_chr_handle(handle, spp_handle_table[SPP_IDX_SPP_RX_VAL]);
            //             dvis_parser_process(handle, p_data->write.value, p_data->write.len);
            //         }
            //         else if(res == SPP_IDX_SPP_RX_CFG) {

            //             // write to cccd of rx characteristic, enables notify or indication
            //             memcpy(spp_rx_ccc, p_data->write.value, 2);
            //             dvis_set_rx_cccd(handle, *((uint16_t *) spp_rx_ccc));
            //             ESP_LOGD(TAG, "%s, ESP_GATTS_WRITE_EVT, setting rx cccd value %d", 
            //                 __func__,
            //                 *((uint16_t *) spp_rx_ccc));
            //         }
            //     // }
            //     // else {

            //     //     if(res == SPP_IDX_SPP_TX_VAL) {
                        
            //     //         ESP_LOGD(TAG, "write prep = true");
            //     //         //store_wr_buffer(p_data); // #TODO: long write
            //     //         ESP_LOG_BUFFER_HEX_LEVEL(TAG, p_data->write.value, p_data->write.len, ESP_LOG_DEBUG);
            //     //     }
            //     // }
            // }
            // else {

            //     ESP_LOGW(TAG, "%s, ESP_GATTS_WRITE_EVT, write error, invalid handle %d", 
            //         __func__,
            //         handle);
            // }
      	 	break;
    	}
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
    	// case ESP_GATTS_EXEC_WRITE_EVT:{

        //     // int handle = dvis_get_handle_from_conn_id(p_data->connect.conn_id);

        //     // if(handle >= 0 && handle < DVIS_PARSER_INSTANCES) {

        //     //     res = find_char_and_desr_index(p_data->write.handle);

        //     //     ESP_LOGD(TAG, 
        //     //         "write exec, conn id %d, handle %d, res %d",
        //     //         p_data->connect.conn_id,
        //     //         handle,
        //     //         res
        //     //     );

        //     //     if(p_data->exec_write.exec_write_flag == true) {

        //     //         ESP_LOGD(TAG, "exec_write_flag 1");

        //     //         if(res == SPP_IDX_SPP_TX_VAL) {

        //     //             //print_write_buffer();
    	//     //             //free_write_buffer();
        //     //             ESP_LOGI(TAG, "write exec tx");
                        
        //     //         }
        //     //     }
        //     //     else {

        //     //         ESP_LOGD(TAG, "exec_write_flag 0");
        //     //     }
        //     // }
        //     // else {

        //     //     ESP_LOGW(TAG, "write error, invalid handle %d", handle);
        //     // }
    	//     break;
    	// }
    	/* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        case ESP_GATTS_MTU_EVT: {

            // uint16_t handle;

            // handle = dvis_get_handle_from_conn_id(p_data->connect.conn_id);

            // dvis_set_mtu(handle, p_data->mtu.mtu);

            ESP_LOGD(TAG, "ESP_GATTS_MTU_EVT, mtu: %d", 
                p_data->mtu.mtu);
            
    	    break;
        }
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        case ESP_GATTS_CONF_EVT: {
            ESP_LOGD(TAG, "ESP_GATTS_CONF_EVT");
            // #TODO: use this event when the total amount of data to send is greater than mut.
            // after every chunk of data sent, this event is called so the remaining data can be sent.
            break;
        }
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
    	case ESP_GATTS_CONNECT_EVT: {

            ESP_LOGD(TAG, "ESP_GATTS_CONNECT_EVT");

            // int handle;

            // handle = dvis_parser_create();

            // if(handle >= 0 && handle < DVIS_PARSER_INSTANCES) {

            //     ESP_LOGI(TAG, 
            //         "connect evt, dvis_parser_create ok, handle %d, conn_id %d", 
            //         handle, 
            //         p_data->connect.conn_id
            //     );

            //     dvis_set_ble_data(handle, gatts_if, p_data->connect.conn_id, &p_data->connect.remote_bda);
            // }
            // else {

            //     ESP_LOGE(TAG, "connect evt, dvis_parser_create error! ret code %d", handle);
            //     esp_ble_gap_disconnect(p_data->connect.remote_bda);
            // }

            // reinicia advertising para permitir novas conexões
            //esp_ble_gap_start_advertising(&cdtp_adv_params);
        	break;
        }
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
    	case ESP_GATTS_DISCONNECT_EVT: {

            ESP_LOGD(TAG, "ESP_GATTS_CONNECT_EVT");

            // int handle;

            // handle = dvis_get_handle_from_conn_id(p_data->connect.conn_id);

            // if(handle >= 0 && handle < DVIS_PARSER_INSTANCES) {

            //     ESP_LOGI(TAG, 
            //         "disconnect evt, conn_id %d, removing handle %d", 
            //         p_data->connect.conn_id,
            //         handle
            //     );

            //     dvis_parser_destroy(handle);

            // }
            // else {

            //     ESP_LOGE(TAG, 
            //         "disconnect evt, conn_id %d, invalid handle %d", 
            //         p_data->connect.conn_id,
            //         handle
            //     );
            // }

    	    esp_ble_gap_start_advertising(&cdtp_adv_params);
    	    break;
        }

        case ESP_GATTS_START_EVT: {
            ESP_LOGD(TAG, "ESP_GATTS_START_EVT");
            break;
        }
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        default: {
            ESP_LOGW(TAG, "UNEXPECTED GATTS EVENT: %d", event);
            break;
        }

        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
    }
}