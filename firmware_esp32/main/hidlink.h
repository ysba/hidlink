#ifndef __HIDLINK__
#define __HIDLINK__

#define HIDLINK_DEVICE_NAME "hidlink"

#define HIDLINK_DEVICE_BD_ADDR_LIST_LEN     64

typedef enum {
    HIDLINK_STATE_API_INIT = 0,
    HIDLINK_STATE_API_DEINIT,
	HIDLINK_STATE_IDLE,
} hidlink_state_t;


typedef enum {
    HIDLINK_COMMAND_NONE = 0,
    HIDLINK_COMMAND_SCAN_START,
    HIDLINK_COMMAND_SCAN_STOP,
} hidlink_command_t;


void hidlink_main_task();
void hidlink_set_command(hidlink_command_t command);
void hidlink_add_discovered_device(esp_bd_addr_t *device_bd_addr);
bool hidlink_check_device_already_discovered(esp_bd_addr_t *device_bd_addr);

#endif
