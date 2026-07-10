#ifndef LED_H
#define LED_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "constants.h"

void led_init(void);
void ledGreenOn(void);
void ledGreenOff(void);
void ledRedOn(void);
void ledRedOff(void);

#endif