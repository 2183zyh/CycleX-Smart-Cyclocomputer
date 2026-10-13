#include "app_display.h"
#include "app_gether.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "ui.h"
#include "device_qmi8658.h"
#include "device_pcnt.h"

static const char *TAG = "DISPLAY";

void app_display_task(void *param)
{
    lv_coord_t heart_data[10] = {0};
    lv_coord_t breath_data[10] = {0};
    lv_coord_t total_data[10] = {0};
    lv_coord_t gyro_data1[10] = {0};
    lv_coord_t gyro_data2[10] = {0};
    lv_coord_t gyro_data3[10] = {0};

    static int16_t prev_count = 0;
    static float time_interval = 0.034; // 34 毫秒 = 0.034 秒，与 vTaskDelay 匹配

    lvgl_port_lock(0);
    lv_chart_series_t *ui_Chart1_series_1 = lv_chart_add_series(ui_Chart1, lv_color_hex(0xFF0000), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t *ui_Chart1_series_2 = lv_chart_add_series(ui_Chart1, lv_color_hex(0x00FF00), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t *ui_Chart1_series_3 = lv_chart_add_series(ui_Chart1, lv_color_hex(0x0000FF), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t *ui_Chart1_series_4 = lv_chart_add_series(ui_Chart2, lv_color_hex(0xFF0000), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t *ui_Chart1_series_5 = lv_chart_add_series(ui_Chart2, lv_color_hex(0x00FF00), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t *ui_Chart1_series_6 = lv_chart_add_series(ui_Chart2, lv_color_hex(0x0000FF), LV_CHART_AXIS_PRIMARY_Y);
    lvgl_port_unlock();

    for (;;)
    {
        lvgl_port_lock(0);
        lv_obj_t *act_scr = lv_disp_get_scr_act(NULL);

        if (act_scr == ui_Screen2)
        {
            for (uint8_t i = 0; i < 9; i++)
            {
                heart_data[i] = heart_data[i + 1];
                breath_data[i] = breath_data[i + 1];
                total_data[i] = total_data[i + 1];
            }
            heart_data[9] = config_ld6002_node.heart_phase * 10;
            breath_data[9] = config_ld6002_node.breath_phase * 11;
            total_data[9] = config_ld6002_node.total_phase * 9;
            lv_chart_set_ext_y_array(ui_Chart1, ui_Chart1_series_1, heart_data);
            lv_chart_set_ext_y_array(ui_Chart1, ui_Chart1_series_2, breath_data);
            lv_chart_set_ext_y_array(ui_Chart1, ui_Chart1_series_3, total_data);
        }
        else if (act_scr == ui_Screen3)
        {
            char text[128];
            snprintf(text, sizeof(text),
                     "Heart Rate: %.2f\n\nBreath Rate: %.2f\n\nRange Data: %.2f",
                     config_ld6002_node.heart_rate,
                     config_ld6002_node.breath_rate,
                     config_ld6002_node.range_data);
            lv_textarea_set_text(ui_TextArea1, text);
        }
        else if (act_scr == ui_Screen4)
        {
            char text[128];
            snprintf(text, sizeof(text),
                     "Accel-x: %.2f\nAccel-y: %.2f\nAccel-z: %.2f",
                     Accel.x, Accel.y, Accel.z);
            lv_textarea_set_text(ui_TextArea2, text);

            for (uint8_t i = 0; i < 9; i++)
            {
                gyro_data1[i] = gyro_data1[i + 1];
                gyro_data2[i] = gyro_data2[i + 1];
                gyro_data3[i] = gyro_data3[i + 1];
            }
            gyro_data1[9] = Gyro.x / 1.25;
            gyro_data2[9] = Gyro.y / 1.25;
            gyro_data3[9] = Gyro.z / 1.25;
            lv_chart_set_ext_y_array(ui_Chart2, ui_Chart1_series_4, gyro_data1);
            lv_chart_set_ext_y_array(ui_Chart2, ui_Chart1_series_5, gyro_data2);
            lv_chart_set_ext_y_array(ui_Chart2, ui_Chart1_series_6, gyro_data3);
        }
        else if (act_scr == ui_Screen5)
        {
            static int16_t last_count = 0;
            static int32_t accumulated_delta = 0;
            static uint32_t update_counter = 0;
            static float last_speed = 0.0; // 保存上一次计算的速度

            int16_t current_count = device_pcnt_get_count();
            int16_t delta = current_count - last_count;
            accumulated_delta += delta;
            last_count = current_count;
            update_counter++;

            float speed = last_speed; // 默认使用上一次的速度
    if (update_counter >= 29) // 每 29 次更新（约 1 秒，29 * 34ms = 986ms）计算一次速度
    {
        speed = (float)accumulated_delta / (time_interval * update_counter); // 原始速度（脉冲/秒）
        speed = speed / 1.25; // 缩小 1.25 倍
        last_speed = speed; // 保存当前速度
        ESP_LOGI(TAG, "Accumulated Delta: %ld, Speed: %.2f pulses/sec", accumulated_delta, speed);
        accumulated_delta = 0;
        update_counter = 0;
    }

            char text[128];
            snprintf(text, sizeof(text),
                     "Current Count: %d\n\nCurrent Speed: %.2f pulses/sec",
                     current_count, speed);
            lv_label_set_text(ui_Label8, text);
        }

        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(34)); // 保持 34 毫秒刷新周期，约 30 帧/秒
    }
}