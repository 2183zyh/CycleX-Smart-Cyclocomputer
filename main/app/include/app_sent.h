
#ifndef APP_SENT_H
#define APP_SENT_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"  // 引入 cJSON 库的头文件

// 声明生成传感器 JSON 数据的函数
char* create_sensor_json(void);

/**
 * @brief MQTT 数据发送任务
 * @param param 任务参数（未使用）
 */
void app_sent_data_task(void *param);

/**
 * @brief 通过 UART 发送数据给语音模块
 */
void send_data_via_uart(void);

#endif // APP_SENT_H

