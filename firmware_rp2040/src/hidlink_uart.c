#include "main.h"


typedef enum {
    HLU_STATE_HEADER = 0,
    HLU_STATE_LEN,
    HLU_STATE_DATA,
    HLU_STATE_CHECKSUM,
} hidlink_uart_rx_state_t;


typedef struct {
    uart_inst_t *port;
    uint baudrate;
    int irq;
    ring_handle_t rx;
    hidlink_uart_rx_state_t state;
    uint32_t rx_index;
    uint32_t rx_count;
    uint8_t rx_buffer[32];    
} hidlink_uart_t;


static hidlink_uart_t hidlink_uart = {
    .port = uart0,
    .baudrate = 115200,
    .irq = UART0_IRQ,
};


static bool led_state = true;

static void hidlink_uart_rx_isr() {

    board_led_write(led_state);
    led_state ^= 1;
    
    while (uart_is_readable(hidlink_uart.port)) {
        
        ring_push(hidlink_uart.rx, uart_getc(hidlink_uart.port));

        
    }
}


void hidlink_uart_init() {

    hidlink_uart.rx = ring_create(64);
    hidlink_uart.rx_index = 0;

    uart_init(hidlink_uart.port, hidlink_uart.baudrate);
    //gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_baudrate(hidlink_uart.port, hidlink_uart.baudrate);
    uart_set_hw_flow(hidlink_uart.port, false, false);
    uart_set_format(hidlink_uart.port, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(hidlink_uart.port, false);
    irq_set_exclusive_handler(hidlink_uart.irq, hidlink_uart_rx_isr);
    irq_set_enabled(hidlink_uart.irq, true);
    uart_set_irq_enables(hidlink_uart.port, true, false);
}



void hidlink_uart_task() {

    uint8_t rx_data;

    while (ring_pop(hidlink_uart.rx, &rx_data) == true) {

        if (hidlink_uart.rx_index < sizeof(hidlink_uart.rx_buffer))
            hidlink_uart.rx_buffer[hidlink_uart.rx_index++];

        switch(hidlink_uart.state) {

            case HLU_STATE_HEADER: {
                if (rx_data == 0xaa) {
                    hidlink_uart.rx_buffer[0] = rx_data;
                    hidlink_uart.rx_index = 1;
                    hidlink_uart.state = HLU_STATE_LEN;
                }
                break;
            }

            case HLU_STATE_LEN: {
                if (rx_data <= 16) {
                    hidlink_uart.rx_count = rx_data;
                    hidlink_uart.state = HLU_STATE_DATA;
                }
                else {
                    hidlink_uart.state = HLU_STATE_HEADER;
                }
                break;
            }

            case HLU_STATE_DATA: {
                if (--hidlink_uart.rx_count == 0) {
                    hidlink_uart.state = HLU_STATE_CHECKSUM;
                }
                break;
            }

            case HLU_STATE_CHECKSUM: {

                uint32_t i;
                uint8_t checksum = 0;

                for (i = 0; i < hidlink_uart.rx_index; i++) {
                    checksum += hidlink_uart.rx_buffer[i];
                }

                if (checksum == 0) {

                    // valid report from esp32

                    // initial dev: if len is 9 bytes, send report
                    if (hidlink_uart.rx_buffer[1] == 9) {

                        

                        // if (tud_suspended()) {
                            
                        //     tud_remote_wakeup();
                        // } 
                        // else if (tud_hid_ready()) {

                        //     tud_hid_keyboard_report(REPORT_ID_KEYBOARD, hidlink_uart.rx_buffer[1], &hidlink_uart.rx_buffer[4]);    
                        // }
                    }
                }
                break;
            }

            default: {

                hidlink_uart.state = HLU_STATE_HEADER;
            }
        }
    }
}


void hidlink_uart_send_buffer(uint8_t *data, uint32_t len) {

    uart_write_blocking (hidlink_uart.port, data, len);
}