// #ifndef APP_DISPLAY_H
// #define APP_DISPLAY_H

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"


// void app_display_task(void *param);

// #endif


#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "app_gether.h" // 包含 app_gether.h 以访问 config_ld6002_node

extern lv_coord_t heart_data[10];
extern lv_coord_t breath_data[10];
extern lv_coord_t total_data[10];
extern lv_coord_t gyro_data1[10];
extern lv_coord_t gyro_data2[10];
extern lv_coord_t gyro_data3[10];

void app_display_task(void *param);

// 不需要重复声明 config_ld6002_node，因为它已在 app_gether.h 中声明

#endif