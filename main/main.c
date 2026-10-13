#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "setup_gpio.h"
#include "setup_spi.h"
#include "setup_i2c.h"
#include "setup_uart.h"
#include "setup_nvs.h"
#include "setup_wifi.h"
#include "device_lcd.h"
#include "device_touch.h"
#include "device_pcnt.h"
#include "app_gether.h"
#include "app_display.h"
#include "app_sent.h"
#include "soft_drv_lvgl_port.h"
#include "soft_drv_mqtt.h"
#include "lv_ui.h"

#include "device_qmi8658.h"

// 定义日志标签
static const char *TAG = "Main";

void app_main(void)
{
    setup_nvs_init();
    setup_gpio_init();
    setup_spi_init();
    setup_i2c_init();
    setup_wifi_sta_init();
    setup_uart_init();
    device_lcd_init();
    device_touch_init();
    QMI8658_Init();
    device_pcnt_init();
    // MQTT 在 Wi-Fi 获取 IP 后由 setup_wifi.c 的事件回调初始化，此处不要重复初始化
    soft_drv_lvgl_port_init();
    lv_ui_init();

    ESP_LOGI(TAG, "App main started");
    esp_log_level_set("*", ESP_LOG_NONE);  // 禁用所有日志

    xTaskCreate(app_gether_task, "gether", 4096, NULL, 10, NULL); // 雷达的 gether
    xTaskCreate(app_display_task, "display", 4096, NULL, 9, NULL);
    xTaskCreate(app_gether_data_task, "gether_data", 4096, NULL, 8, NULL); // QMI 和 PCNT 的 gether
    xTaskCreate(app_sent_data_task, "send", 4096, NULL, 7, NULL); // 启用数据发送任务
    
}