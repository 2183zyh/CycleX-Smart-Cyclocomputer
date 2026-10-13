#ifndef LD6002_H_
#define LD6002_H_

#include <stdio.h>

typedef uint8_t LD_U8;
typedef uint16_t LD_U16;
typedef float LD_FLOAT;
typedef uint32_t LD_U32;

typedef struct LD6002_node
{
    LD_FLOAT total_phase;  /* 总相位 */
    LD_FLOAT breath_phase; /* 呼吸相位 */
    LD_FLOAT heart_phase;  /* 心跳相位 */

    LD_FLOAT heart_rate;  /* 心跳速率 */
    LD_FLOAT breath_rate; /* 呼吸速率 */

    LD_U32 range_flag;   /* 距离flag，为1是输出距离 */
    LD_FLOAT range_data; /* 距离cm */
} LD6002_node_t;

LD_U8 LD6002_init(LD6002_node_t *node);
LD_U8 LD6002_sum_check(const LD_U8 *buffer, LD_U16 len);
LD_U16 LD6002_select_head(const LD_U8 *buffer, LD_U16 len);
LD_U8 LD6002_get_head_key_info(const LD_U8 *buffer, LD_U16 len, LD_U16 *type, LD_U16 *data_len);
LD_U8 LD6002_fetch_data(LD6002_node_t *node, const LD_U8 *buffer, LD_U16 len, LD_U16 type, LD_U16 data_len);

#endif
