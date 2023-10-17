#include "main.h"


typedef struct {
    uart_inst_t *port;
    uint baudrate;
    int irq;
} hidlink_uart_t;


static hidlink_uart_t hidlink_uart = {
    .port = uart0,
    .baudrate = 115200,
    .irq = UART0_IRQ,
};


static void hidlink_uart_rx_isr() {
    
    while (uart_is_readable(hidlink_uart.port)) {
        uint8_t data_rx = uart_getc(hidlink_uart.port);
        
        if (tud_suspended()) {
            tud_remote_wakeup();
        } 
        else if (tud_hid_ready()) {
            
            // uint8_t keycode[6] = {0};
            // keycode[0] = HID_KEY_A;
            tud_hid_report(0, &data_rx, 1);
            //tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, &data_rx);
            
        }
    }
}


void hidlink_uart_init() {

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



void hidlink_uart_send_buffer(uint8_t *data, uint32_t len) {

    uart_write_blocking (hidlink_uart.port, data, len);
}