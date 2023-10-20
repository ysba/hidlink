#ifndef __UART__
#define __UART__

#include "hardware/uart.h"
#include "hardware/irq.h"

#define UART_TX_PIN     0
#define UART_RX_PIN     1

void hidlink_uart_init();
void hidlink_uart_task();

#endif