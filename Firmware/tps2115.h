#ifndef TPS2115_H
#define TPS2115_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "constants.h"

void tps2115_init(void);
bool getSTATStatus(void);

#endif