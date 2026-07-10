#ifndef TPS259470_H
#define TPS259470_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "constants.h"

void tps259470_init(void);
bool getAUXOFFStatus(void);
bool getNFAULTStatus(void);

#endif