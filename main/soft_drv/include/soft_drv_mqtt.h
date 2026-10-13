#ifndef SOFT_DRV_MQTT_H_
#define SOFT_DRV_MQTT_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"

extern esp_mqtt_client_handle_t client;

void soft_drv_mqtt_init(void);

#endif
