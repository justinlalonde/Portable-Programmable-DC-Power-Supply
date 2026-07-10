#ifndef TPS55287_H
#define TPS55287_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "constants.h"

void tps55287_init(i2c_master_bus_handle_t bus_handle);
void outputOn(void);
void outputOff(void);
esp_err_t setOutputValues(float voltageValue, float currentLimit);
bool getOCPStatus(void);
bool getINTStatus(void);

#endif