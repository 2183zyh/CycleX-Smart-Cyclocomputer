#ifndef DEVICE_PCNT_H
#define DEVICE_PCNT_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

// 定义 PCNT 的 GPIO 引脚
#define PCNT_PIN GPIO_NUM_15  // PCNT 的输入引脚，使用 GPIO 15

// PCNT 单元句柄，声明为外部变量
extern pcnt_unit_handle_t pcnt_unit;

void device_pcnt_init(void);
int16_t device_pcnt_get_count(void);
float device_pcnt_calculate_speed(int16_t prev_count, int16_t current_count, float time_interval);

#endif // DEVICE_PCNT_H