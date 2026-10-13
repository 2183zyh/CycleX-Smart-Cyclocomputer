# #Add sources from ui directory
file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")

idf_component_register(
    SRCS 
        "main.c"
        "./setup/setup_gpio.c"
        "./setup/setup_spi.c"
        "./setup/setup_i2c.c"
        "./setup/setup_uart.c"
        "./setup/setup_nvs.c"
        "./setup/setup_wifi.c"
        "./device/device_lcd.c"
        "./device/device_qmi8658.c"
        "./device/device_touch.c"
        "./device/device_cst328.c"
        "./device/device_pcnt.c"
        "./app/app_gether.c"
        "./app/app_sent.c"
        "./app/app_display.c"
        "./soft_drv/soft_drv_lvgl_port.c"
        "./soft_drv/soft_drv_mqtt.c"
        "./lv_ui/lv_ui.c"

        ${SRC_UI}
    INCLUDE_DIRS
        "."
        "./setup/include"
        "./device/include"
        "./soft_drv/include"
        "./app/include"
        "./lv_ui/include"
        "./ui"
)