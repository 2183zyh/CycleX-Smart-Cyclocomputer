#include "LD6002.h"

LD_U8 LD6002_init(LD6002_node_t *node)
{
    node->breath_phase = 0.0;
    node->heart_phase = 0.0;
    node->total_phase = 0.0;

    node->heart_rate = 0.0;
    node->breath_rate = 0.0;

    node->range_data = 0.0;
    node->range_flag = 0;

    return 0;
}

/**
 * @brief 校验数据和
 * @param buffer 原始数据
 * @param len 需要被检验的数据长度
 * @return 返回值不为0x00则为计算后的校验和
 */
LD_U8 LD6002_sum_check(const LD_U8 *buffer, LD_U16 len)
{
    LD_U16 i;
    LD_U8 ret = 0;
    // 校验和
    for (i = 0; i < len; i++)
    {
        ret ^= buffer[i];
    }
    ret = ~ret;
    return ret;
}

/**
 * @brief 查找标头
 * @param buffer 数据
 * @param len 数据长度
 * @return 返回0xFFFF为没有找到，index代表下标
 */
LD_U16 LD6002_select_head(const LD_U8 *buffer, LD_U16 len)
{
    LD_U16 index = 0;

    if (len < 8)
    {
        return 0xFFFF;
    }

    for (index = 0; index < len - 8; index++)
    {
        if (0x01 == buffer[index] &&
            0x0A == buffer[index + 5] &&
            LD6002_sum_check(&buffer[index], 7) == buffer[index + 7])
        {
            return index;
        }
    }

    return 0xFFFF;
}

/**
 * @brief 获取帧头中的关键信息
 * @param buffer 数据
 * @param len 数据长度
 * @param type 类型指针
 * @param data_len 数据长度指针
 * @return 0成功
 */
LD_U8 LD6002_get_head_key_info(const LD_U8 *buffer, LD_U16 len, LD_U16 *type, LD_U16 *data_len)
{
    if (len < 8)
    {
        return 1;
    }

    *type = (((LD_U16)buffer[5]) << 8) | buffer[6];
    *data_len = (((LD_U16)buffer[3]) << 8) | buffer[4];

    return 0;
}

/**
 * @brief 解析数据，获取各项参数
 * @param node 实体
 * @param buffer 原始数据
 * @param len 数据长度
 * @param type 类型
 * @param data_len 原始数据data长度
 * @return 0为成功
 */
LD_U8 LD6002_fetch_data(LD6002_node_t *node, const LD_U8 *buffer, LD_U16 len, LD_U16 type, LD_U16 data_len)
{
    LD_U32 temp = 0;
    LD_U8 i, j;

    if (len < 8 + data_len + 1)
    {
        return 1;
    }

    if (LD6002_sum_check(&buffer[8], data_len) != buffer[8 + data_len])
    {
        return 2;
    }

    for (i = 0; i < data_len / 4; i++)
    {
        temp = 0;
        for (j = 0; j < 4; j++)
        {
            temp >>= 8;
            temp |= (((LD_U32)(buffer[8 + (i * 4) + j])) << 24);
        }
        switch (type)
        {
        case 0x0A13:
            if (0 == i)
            {
                node->total_phase = *(LD_FLOAT *)&temp;
            }
            else if (1 == i)
            {
                node->breath_phase = *(LD_FLOAT *)&temp;
            }
            else if (2 == i)
            {
                node->heart_phase = *(LD_FLOAT *)&temp;
            }
            break;
        case 0x0A14:
            if (0 == i)
            {
                node->breath_rate = *(LD_FLOAT *)&temp;
            }
            break;
        case 0x0A15:
            if (0 == i)
            {
                node->heart_rate = *(LD_FLOAT *)&temp;
            }
            break;
        case 0x0A16:
            if (0 == i)
            {
                node->range_flag = temp;
            }
            else if (1 == i)
            {
                if(node->range_flag != 0)
                {
                    node->range_data = *(LD_FLOAT *)&temp;
                }
            }
            break;

        default:
            return 3;
        }
    }

    return 0;
}
