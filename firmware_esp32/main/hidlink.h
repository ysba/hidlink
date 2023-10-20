#ifndef __HIDLINK__
#define __HIDLINK__

#define HIDLINK_DEVICE_NAME "hidlink"

#define HIDLINK_DEVICE_BD_ADDR_LIST_LEN     64
#define HIDLINK_DEVICES_NUMBER              16

#define HIDLINK_PROTOCOL_BUFFER_LEN         64

#define HIDLINK_PERIPHERAL_MAX_NAME_LEN     32

#define HIDLINK_UART_PORT_NUM       2
#define HIDLINK_UART_BUF_SIZE       256

typedef enum {
    HIDLINK_STATE_API_INIT = 0,
    HIDLINK_STATE_API_DEINIT,
	HIDLINK_STATE_IDLE,
} hidlink_state_t;


typedef enum {
    HIDLINK_COMMAND_NONE = 0,
    HIDLINK_COMMAND_SCAN_START,
    HIDLINK_COMMAND_SCAN_STOP,
    HIDLINK_COMMAND_SCAN_DONE,
    HIDLINK_COMMAND_SET_STATUS_CONNECTED,
    HIDLINK_COMMAND_SET_STATUS_IDLE,
} hidlink_command_t;


typedef enum {
    HIDLINK_STATUS_NONE = 0,
    HIDLINK_STATUS_IDLE,
    HIDLINK_STATUS_SCANNING,
    HIDLINK_STATUS_CONNECTED,
} hidlink_status_t; 


typedef enum {
    HIDLINK_PROTOCOL_STATE_HEADER = 0,
    HIDLINK_PROTOCOL_STATE_COMMAND,
    HIDLINK_PROTOCOL_STATE_LEN,
    HIDLINK_PROTOCOL_STATE_DATA,
    HIDLINK_PROTOCOL_STATE_CHECKSUM,
} hidlink_protocol_state_t;


typedef enum {
    HIDLINK_PROTOCOL_COMMAND_NONE = 0,
    HIDLINK_PROTOCOL_COMMAND_GET_STATUS,
    HIDLINK_PROTOCOL_COMMAND_START_SCAN,
    HIDLINK_PROTOCOL_COMMAND_STOP_SCAN,
    HIDLINK_PROTOCOL_COMMAND_ATTACH_TO_PERIPHERAL,
    HIDLINK_PROTOCOL_COMMAND_PERIPHERAL_SCAN_DATA,
    HIDLINK_PROTOCOL_COMMAND_MAX,
} hidlink_protocol_command_t;

void hidlink_main_task();
void hidlink_set_command(hidlink_command_t command);
void hidlink_add_discovered_device(esp_bd_addr_t *device_bd_addr);
bool hidlink_check_device_already_discovered(esp_bd_addr_t *device_bd_addr);
void hidlink_add_hid_device(esp_bd_addr_t *bd_addr, char *name);
void hidlink_ble_protocol_parser(uint8_t *data, uint32_t len);
void hidlink_ble_set_char_handle(uint16_t char_handle);
void hidlink_ble_set_mtu(uint16_t mtu);
void hidlink_set_ble_data(esp_gatt_if_t gatts_if, uint16_t conn_id, esp_bd_addr_t *bda);
void hidlink_set_rx_cccd(uint16_t val);
void hidlink_send_hid_peripheral_data(uint8_t peripheral_index, esp_bd_addr_t *bd_addr, char *name);
void hidlink_send_hid_report_to_uart(uint8_t *data, uint32_t len);

#endif
