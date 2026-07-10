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
#include "misc.h"
#include "tps2115.h"
#include "tps55287.h"
#include "tps259470.h"

#include "state_machine.h"

#include "constants.h"

static i2c_master_bus_handle_t bus_handle;

static void i2c_init(void)
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_PORT,
        .sda_io_num = I2C_SDA_IO,
        .scl_io_num = I2C_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false, // using external pullups
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));
}
void app_main(void)
{
    i2c_init();
    ads1015_init(bus_handle);
    lp5024_init(bus_handle);
    tps2115_init();
    tps55287_init(bus_handle);
    tps259470_init();
    display_init();
    led_init();
    potentiometer_init();
    
    float batteryVoltage = getBatteryVoltage();
    setDisplayValue(batteryVoltage,'v');
    for (int i = 0; i < 3; i++)
    {
        displayOn('v');
        vTaskDelay(pdMS_TO_TICKS(BATT_VOLT_FLASH_TCIK));
        displayOff('v');
        if(i < 2) vTaskDelay(pdMS_TO_TICKS(BATT_VOLT_FLASH_TCIK));
    }

    TickType_t last_wake = xTaskGetTickCount();
    while (1)
    {
        ESP_ERROR_CHECK(state_machine_step());
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(STATE_MACHINE_TICK_MS));
    }
}