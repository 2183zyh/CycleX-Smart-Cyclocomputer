#ifndef APP_GETHER_H_
#define APP_GETHER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "LD6002.h"

//#define UART_PORT UART_NUM_1
//#define BUF_SIZE 256

extern LD6002_node_t config_ld6002_node;

void app_gether_task(void *param);
void app_gether_data_task(void *param);
     
#endif
