#include "setup_i2c.h"
#include "setup_gpio.h"
#include <string.h>

// #define I2C_MASTER_FREQ_HZ (400000)
// #define I2C_MASTER_RX_BUF_DISABLE 0
// #define I2C_MASTER_TX_BUF_DISABLE 0

void setup_i2c_init(void)
{
    i2c_config_t i2c_cfg = {};
    i2c_cfg.mode = I2C_MODE_MASTER;
    i2c_cfg.scl_io_num = I2C_TOUCH_SCL;
    i2c_cfg.sda_io_num = I2C_TOUCH_SDA;
    i2c_cfg.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_cfg.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_cfg.master.clk_speed = I2C_MASTER_FREQ_HZ;
    ESP_ERROR_CHECK(i2c_param_config(I2C_TOUCH_PORT, &i2c_cfg));
    i2c_driver_install(I2C_TOUCH_PORT, i2c_cfg.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

    i2c_config_t i2c_cfg1 = {};
    i2c_cfg1.mode = I2C_MODE_MASTER;
    i2c_cfg1.scl_io_num = I2C_SCL_IO;
    i2c_cfg1.sda_io_num = I2C_SDA_IO;
    i2c_cfg1.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_cfg1.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_cfg1.master.clk_speed = I2C_MASTER_FREQ_HZ;
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &i2c_cfg1));
    i2c_driver_install(I2C_MASTER_NUM, i2c_cfg1.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}



// 这个把demo里面的.c文件里面的最后两个函数移植过来后，有未标明的，I2C_MASTER_NUM这种 于是我把demo里面的h文件的define也拿过来用了，也确实不报错了
//Reg addr is 8 bit
esp_err_t I2C_Write(uint8_t Driver_addr, uint8_t Reg_addr, const uint8_t *Reg_data, uint32_t Length)
{
    uint8_t buf[Length+1];
    
    buf[0] = Reg_addr;
    // Copy Reg_data to buf starting at buf[1]
    memcpy(&buf[1], Reg_data, Length);
    return i2c_master_write_to_device(I2C_MASTER_NUM, Driver_addr, buf, Length+1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}



esp_err_t I2C_Read(uint8_t Driver_addr, uint8_t Reg_addr, uint8_t *Reg_data, uint32_t Length)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, Driver_addr, &Reg_addr, 1, Reg_data, Length, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}
