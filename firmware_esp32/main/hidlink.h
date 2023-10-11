#ifndef __HIDLINK__
#define __HIDLINK__

typedef enum {
    HIDLINK_STATE_API_INIT = 0,
    HIDLINK_STATE_API_DEINIT,
	HIDLINK_STATE_IDLE,
	HIDLINK_STATE_SCAN_START,
	HIDLINK_STATE_SCAN_WAIT,
	HIDLINK_STATE_SCAN_RESULTS,
	
	
} hidlink_state_t;


void hidlink_main_task();

#endif
