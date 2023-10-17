#include "main.h"

enum {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint INPUT_PIN = 2;

void led_blinking_task(void);
//void hid_task(void);

int main() {
    
    gpio_init(INPUT_PIN);
    gpio_set_dir(INPUT_PIN, GPIO_IN);
    gpio_pull_up(INPUT_PIN);

    board_init();
    //tusb_init();
    tud_init(BOARD_DEVICE_RHPORT_NUM);
    //board_init_after_tusb();

    hidlink_uart_init();

    board_led_write(false);

    while (1) {
        tud_task();
        led_blinking_task();
        //hid_task();
    }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
    blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
    blink_interval_ms = BLINK_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// void hid_task(void) {

//     // Poll every 10ms
//     const uint32_t interval_ms = 10;
//     static uint32_t start_ms = 0;
//     static bool has_keyboard_key = false;
//     uint32_t btn = 0;

//     if (board_millis() - start_ms < interval_ms)
//         return;

//     start_ms += interval_ms;

//     btn = !gpio_get(INPUT_PIN);

//     // Remote wakeup
//     if (tud_suspended() && btn) {
//         // Wake up host if we are in suspend mode
//         // and REMOTE_WAKEUP feature is enabled by host
//         tud_remote_wakeup();
//     } else if (tud_hid_ready()) {
//         if (btn) {
//             uint8_t keycode[6] = {
//                 0
//             };
//             keycode[0] = HID_KEY_A;
//             tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
//             has_keyboard_key = true;
//         } else {
//             // send empty key report if previously has key pressed
//             if (has_keyboard_key)
//                 tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
//             has_keyboard_key = false;
//         }
//     }
// }

// Invoked when sent REPORT successfully to host
void tud_hid_report_complete_cb(uint8_t instance, uint8_t
    const * report, uint16_t len) {

    // #TODO: respond to esp host success on hid report
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t * buffer, uint16_t reqlen) {
    // TODO not Implemented
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t
    const * buffer, uint16_t bufsize) {

    (void) instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD) {
            
            // bufsize should be (at least) 1
            if (bufsize < 1)
                return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
                // Capslock On: disable blink, turn led on
                blink_interval_ms = 300;
                //board_led_write(false);
            } else {
                // Caplocks Off: back to normal blink
                //board_led_write(true);
                blink_interval_ms = BLINK_MOUNTED;
            }
        }
    }
}

void led_blinking_task(void) {
    static uint32_t start_ms = 0;
    static bool led_state = false;

    if (blink_interval_ms == 0) {
        board_led_write(false);
        return;
    }

    if (board_millis() - start_ms < blink_interval_ms)
        return;
    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state;
}