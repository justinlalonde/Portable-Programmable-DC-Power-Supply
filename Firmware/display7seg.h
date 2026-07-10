#ifndef DISPLAY7SEG_H
#define DISPLAY7SEG_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "constants.h"
#include "lp5024.h"

void display_init(void);
void setDisplayValue(float value, char chipID);
void displaySCTag(char chipID);
void displayErrTag(char chipID);

#endif