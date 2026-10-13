#include "app_gether.h"
#include "setup_uart.h"
#include "esp_log.h"
#include "device_pcnt.h"
#include "device_qmi8658.h"
#include <cJSON.h>
#include <string.h>
#include <stdio.h>

// 日志标签
static const char *TAG = "UART_EXAMPLE";

// LD6002 节点数据结构
LD6002_node_t config_ld6002_node = {};

// 缓冲区大小
#define BUF_SIZE 128
// UART 端口
#define UART_PORT UART_NUM_1
#define UART_NUM_2 UART_NUM_2

void app_gether_task(void *param)
{
    static uint8_t data[BUF_SIZE];

    // 初始化日志
    ESP_LOGI(TAG, "init uart ok");

    // 清零 LD6002 数据结构
    memset(&config_ld6002_node, 0, sizeof(config_ld6002_node));

    while (1)
    {
        // 从 UART 读取数据
        int len = uart_read_bytes(UART_PORT, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        if (len > 0)
        {
            uint16_t index = 0;
            uint16_t type, data_len;
            while (index < len - 8)
            {
                // 查找帧头
                uint16_t ret = LD6002_select_head(&data[index], len - index);
                if (ret == 0xFFFF)
                {
                    break;
                }
                index += ret;

                // 获取帧信息
                if (0 != LD6002_get_head_key_info(&data[index], len - index, &type, &data_len))
                {
                    break;
                }

                // 解析数据
                if (0 != LD6002_fetch_data(&config_ld6002_node, &data[index], len - index, type, data_len))
                {
                    break;
                }

                // 发送数据
                if (1)
                {
                    static int send_counter = 0;
                    float heart_rate = config_ld6002_node.heart_rate;
                    float breath_rate = config_ld6002_node.breath_rate;
                    send_counter++;

                    if (send_counter >= 8) // 每8帧发送一次
                    {
                        // 心率帧
                        uint8_t hr_frame[5] = {0};
                        hr_frame[0] = 0xFE;           // 帧头
                        hr_frame[1] = 0x01;           // 心率标识
                        hr_frame[2] = 0x00;           // 高字节
                        hr_frame[3] = (uint8_t)heart_rate; // 低字节
                        hr_frame[4] = 0x01;           // 固定字节

                        // 呼吸率帧
                        uint8_t br_frame[5] = {0};
                        br_frame[0] = 0xFE;           // 帧头
                        br_frame[1] = 0x02;           // 呼吸率标识
                        br_frame[2] = 0x00;           // 高字节
                        br_frame[3] = (uint8_t)breath_rate; // 低字节
                        br_frame[4] = 0x01;           // 固定字节

                        // 发送到语音模块
                        uart_write_bytes(UART_NUM_2, (const char *)hr_frame, 5);
                        uart_write_bytes(UART_NUM_2, (const char *)br_frame, 5);

                        // 打印心率和呼吸率帧（紧凑格式）
                        printf("%02X%02X%02X%02X%02X\n",
                               hr_frame[0], hr_frame[1], hr_frame[2], hr_frame[3], hr_frame[4]);
                        printf(" %02X%02X%02X%02X%02X\n",
                               br_frame[0], br_frame[1], br_frame[2], br_frame[3], br_frame[4]);

                        send_counter = 0; // 重置计数器
                    }
                }
                else
                {
                    printf("Invalid HR or BR values, skipping sending data...\n");
                }

                // 更新索引，跳过当前帧
                index = index + 8 + data_len + 1;
            }
        }
    }
}

void app_gether_data_task(void *param)
{
    int16_t prev_count = 0;
    int16_t current_count = 0;
    float time_interval = 0.1; // 100ms
    uint32_t minute_counter = 0;

    for (;;)
    {
        // 更新 QMI8658 数据
        QMI8658_Loop();

        // 获取计数器值
        current_count = device_pcnt_get_count();
        float speed = (current_count - prev_count) / time_interval;

        // 打印计数和速度
        ESP_LOGI("GETHER_DATA", "Count: %d, Speed: %.2f pulses/s", 
                 current_count, speed);

        // 每分钟重置计数器
        minute_counter++;
        if (minute_counter >= 600) // 600 * 100ms = 60s
        {
            ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
            minute_counter = 0;
            ESP_LOGI("GETHER_DATA", "Counter reset to zero after 1 minute!");
            current_count = 0;
        }

        prev_count = current_count;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}