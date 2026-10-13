#ifndef SETUP_GPIO_H_
#define SETUP_GPIO_H_

#include "driver/gpio.h"

// spi lcd pin
#define LCD_BLK_NUM GPIO_NUM_5
#define LCD_SCLK_NUM GPIO_NUM_40
#define LCD_MOSI_NUM GPIO_NUM_45
#define LCD_MISO_NUM GPIO_NUM_46
#define LCD_RST_NUM GPIO_NUM_39
#define LCD_DC_NUM GPIO_NUM_41

// spi cs pin
#define LCD_CS_NUM GPIO_NUM_42
#define LCD_TOUCH_CS_NUM GPIO_NUM_9

// touch i2c pin
#define I2C_TOUCH_SDA GPIO_NUM_1
#define I2C_TOUCH_SCL GPIO_NUM_3

// touch touch pin
#define I2C_TOUCH_INT GPIO_NUM_4
#define I2C_TOUCH_RST GPIO_NUM_2

// uart set pin
#define UART_RX_PIN GPIO_NUM_43
#define UART_TX_PIN GPIO_NUM_44
#define UART_RX_PIN1 GPIO_NUM_NC  // 未连接
#define UART_TX_PIN1 GPIO_NUM_18  // 语音模块的 TX

// hall-pcnt set pin
// PCNT pins
#define PCNT_PIN GPIO_NUM_15  // 仅使用 GPIO15 作为 PCNT 输入
// hr pin

void setup_gpio_init(void);

#endif