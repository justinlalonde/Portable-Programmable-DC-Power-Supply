#ifndef MISC_H
#define MISC_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "ads1015.h"
#include "display7seg.h"
#include "led.h"
#include "lp5024.h"
#include "tps2115.h"
#include "tps55287.h"
#include "tps259470.h"

#include "constants.h"

void potentiometer_init(void);
float getBatteryVoltage(void);
uint8_t getPot0(void);
uint8_t getPot1(void);
float getOutputVoltage(void);
float getOutputCurrent(void);
void updateTargets(float* voltageTarget, float* currentTarget);

#endif