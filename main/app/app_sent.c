#include "app_sent.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <driver/uart.h>
#include "cJSON.h"
#include "soft_drv_mqtt.h"  // MQTT 头文件
#include "app_display.h"    // 显示头文件
#include "device_qmi8658.h" // 传感器头文件
#include "app_gether.h"
#include "device_pcnt.h"    // PCNT 头文件

// 日志标签
static const char *TAG = "APP_SENT";

// 静态变量，用于记录上一次的 PCNT 计数
static int16_t prev_count = 0;

/**
 * @brief 生成传感器数据的 JSON 字符串
 * @return char* 返回 JSON 字符串的指针，调用者需要释放内存
 */
char* create_sensor_json(void)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return NULL;
    }

    // 添加心跳数据
    cJSON *heart = cJSON_CreateObject();
    cJSON_AddNumberToObject(heart, "rate", config_ld6002_node.heart_rate);
    cJSON_AddNumberToObject(heart, "phase", config_ld6002_node.heart_phase);
    cJSON_AddItemToObject(root, "heart", heart);

    // 添加呼吸数据
    cJSON *breath = cJSON_CreateObject();
    cJSON_AddNumberToObject(breath, "rate", config_ld6002_node.breath_rate);
    cJSON_AddNumberToObject(breath, "phase", config_ld6002_node.breath_phase);
    cJSON_AddItemToObject(root, "breath", breath);

    // 添加范围数据和总相位
    cJSON_AddNumberToObject(root, "range_data", config_ld6002_node.range_data);
    cJSON_AddNumberToObject(root, "total_phase", config_ld6002_node.total_phase);

    // 添加加速度数据
    cJSON *acceleration = cJSON_CreateObject();
    cJSON_AddNumberToObject(acceleration, "x", Accel.x);
    cJSON_AddNumberToObject(acceleration, "y", Accel.y);
    cJSON_AddNumberToObject(acceleration, "z", Accel.z);
    cJSON_AddItemToObject(root, "acceleration", acceleration);

    // 添加陀螺仪数据
    cJSON *gyroscope = cJSON_CreateObject();
    cJSON_AddNumberToObject(gyroscope, "x", Gyro.x);
    cJSON_AddNumberToObject(gyroscope, "y", Gyro.y);
    cJSON_AddNumberToObject(gyroscope, "z", Gyro.z);
    cJSON_AddItemToObject(root, "gyroscope", gyroscope);

    // 添加 PCNT 数据（计数和速度）
    int16_t current_count = device_pcnt_get_count();
    float time_interval = 0.5; // 0.5 秒（任务每 500 毫秒运行一次）
    float speed = device_pcnt_calculate_speed(prev_count, current_count, time_interval);
    cJSON_AddNumberToObject(root, "pcnt_count", current_count);
    cJSON_AddNumberToObject(root, "pcnt_speed", speed);
    prev_count = current_count; // 更新上一次计数

    // 生成 JSON 字符串
    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root); // 释放 JSON 对象内存

    if (json_str == NULL)
    {
        ESP_LOGE(TAG, "Failed to print JSON");
    }
    return json_str;
}

/**
 * @brief MQTT 数据发送任务
 * @param param 任务参数（未使用）
 */
void app_sent_data_task(void *param)
{
    while (1)
    {
        if (client != NULL)
        {
            char *json_str = create_sensor_json();
            if (json_str != NULL)
            {
                // 通过 MQTT 发送数据
                const char *topic = "/topic/sensor_data";
                int msg_id = esp_mqtt_client_publish(client, topic, json_str, 0, 1, 0);
                if (msg_id > 0)
                {
                    ESP_LOGI(TAG, "消息发送成功，ID: %d, 数据: %s", msg_id, json_str);
                }
                else
                {
                    ESP_LOGE(TAG, "消息发送失败");
                }
                free(json_str); // 释放 JSON 字符串内存

                // 通过 UART 发送数据给语音模块
                send_data_via_uart();
            }
            else
            {
                ESP_LOGE(TAG, "生成 JSON 字符串失败");
            }
        }
        else
        {
            ESP_LOGE(TAG, "MQTT 客户端未初始化");
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // 每 500 毫秒执行一次
    }
}

/**
 * @brief 通过 UART 发送数据给语音模块
 */
void send_data_via_uart(void)
{
    // 获取 LD6002 雷达数据
    float heart_rate = config_ld6002_node.heart_rate;
    float breath_rate = config_ld6002_node.breath_rate;

    // 检查数据有效性（心率和呼吸率应在合理范围内，例如 0-255）
    if (heart_rate <= 0 || heart_rate > 255 || breath_rate <= 0 || breath_rate > 255)
    {
        ESP_LOGE(TAG, "无效的 LD6002 数据: 心率: %.2f, 呼吸率: %.2f", heart_rate, breath_rate);
        return; // 如果数据无效，不发送
    }

    // 定义一个 5 字节的 uint8_t 数组
    uint8_t temp[5] = {0};

    // 设置数据包格式
    temp[0] = 0xFE;                // 起始字节
    temp[1] = 0x01;                // 命令字节（假设 0x01 表示语音播报）
    temp[2] = (uint8_t)heart_rate;  // 心率（取整数部分）
    temp[3] = (uint8_t)breath_rate; // 呼吸率（取整数部分）
    temp[4] = temp[0] ^ temp[1] ^ temp[2] ^ temp[3]; // 校验字节（异或校验）

    // 通过 UART 发送数据
    int bytes_written = uart_write_bytes(UART_NUM_2, (const char *)temp, 5);
    if (bytes_written == 5)
    {
        ESP_LOGI(TAG, "UART 数据发送成功: %02X %02X %02X %02X %02X (心率: %.2f, 呼吸率: %.2f)",
                 temp[0], temp[1], temp[2], temp[3], temp[4], heart_rate, breath_rate);
    }
    else
    {
        ESP_LOGE(TAG, "UART 数据发送失败，发送字节数: %d", bytes_written);
    }
}