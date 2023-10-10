#ifndef __HIDLINK__
#define __HIDLINK__

#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_hidh_api.h"


typedef enum {
    HIDLINK_STATE_INIT_ESP_API = 0,
    HIDLINK_STATE_INIT_BT_API,
	HIDLINK_STATE_IDLE,
	HIDLINK_STATE_SCAN_START,
	HIDLINK_STATE_SCAN_WAIT,
	HIDLINK_STATE_SCAN_RESULTS,
	HIDLINK_STATE_DEINIT_BT_API,
	
} hidlink_state_t;


void hidlink_main_task();

#endif
