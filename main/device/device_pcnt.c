#include "device_pcnt.h"

// 日志标签
static const char *TAG = "DEVICE_PCNT";

// 定义 PCNT 单元句柄（在头文件中声明为 extern）
pcnt_unit_handle_t pcnt_unit = NULL;

/**
 * @brief 初始化 PCNT 模块
 */
void device_pcnt_init(void)
{
    // 配置 GPIO 引脚
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << PCNT_PIN),  // 仅配置 GPIO15
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    // 配置 PCNT 单元
    pcnt_unit_config_t unit_config = {
        .high_limit = 32767,
        .low_limit = -32768,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    // 配置 PCNT 通道
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = PCNT_PIN,  // 使用 GPIO15 检测边沿
        .level_gpio_num = -1,       // 不使用 level_gpio
    };
    pcnt_channel_handle_t pcnt_chan = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));

    // 设置边沿动作：上升沿增加计数，下降沿保持不变
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));

    // 设置去抖滤波器
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &(pcnt_glitch_filter_config_t){
        .max_glitch_ns = 1250,
    }));

    // 启用 PCNT 单元
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    ESP_LOGI(TAG, "PCNT initialized successfully for single pin mode");
}

/**
 * @brief 获取当前 PCNT 计数器的值
 * @return int16_t 返回当前的计数器值
 */
int16_t device_pcnt_get_count(void)
{
    int count = 0;
    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &count));
    return (int16_t)count;
}

/**
 * @brief 计算速度（每秒脉冲数）
 * @param prev_count 上一次的计数
 * @param current_count 当前计数
 * @param time_interval 时间间隔（秒）
 * @return float 返回速度（脉冲/秒）
 */
float device_pcnt_calculate_speed(int16_t prev_count, int16_t current_count, float time_interval)
{
    int16_t delta = current_count - prev_count;
    return (float)delta / time_interval;
}