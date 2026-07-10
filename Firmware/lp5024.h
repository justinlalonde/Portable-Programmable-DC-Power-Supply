#ifndef LP5024_H
#define LP5024_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "constants.h"

void lp5024_init(i2c_master_bus_handle_t bus_handle);
void setBrightness(uint8_t brightness, char chipID);
esp_err_t displayOn(char chipID);
esp_err_t displayOff(char chipID);
esp_err_t display(uint32_t value, char chipID);

#endif