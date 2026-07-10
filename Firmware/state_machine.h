#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

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

#include "misc.h"

#include "constants.h"

typedef enum { ES, OO, SC, CC, CV } pcb_state_t;
typedef struct { bool ES; bool OO; bool SC; bool OC; } pcb_flags_t;
esp_err_t state_machine_step(void);

#endif