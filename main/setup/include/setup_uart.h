#ifndef SETUP_UART_H_
#define SETUP_UART_H_

#include "driver/uart.h"

#define UART_PORT UART_NUM_1
#define UART_PORT1 UART_NUM_2
#define BUF_SIZE (1024)
#define BAUD_RATE 1382400
#define BAUD_RATE1 9600
void setup_uart_init(void);

#endif


