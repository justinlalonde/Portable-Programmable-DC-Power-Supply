#include "misc.h"

static bool voltagePotEnable, currentPotEnable = false;
static int firstVoltagePotValue, firstCurrentPotValue;
void potentiometer_init()
{
    firstVoltagePotValue = getPot1();
    firstCurrentPotValue = getPot0();
}
float getBatteryVoltage()
{
    int16_t analog_raw;
    ESP_ERROR_CHECK(ads1015_sample(&analog_raw, 2)); 
    return analog_raw * 4.096f * 8.52f / 2048.0f;
}
uint8_t getPot0()
{
    int16_t analog_raw;
    ESP_ERROR_CHECK(ads1015_sample(&analog_raw, 0)); 
    return (uint8_t)(analog_raw * 4.096f * 256.0f / 2048.0f / 3.3f);
}
uint8_t getPot1()
{
    int16_t analog_raw;
    ESP_ERROR_CHECK(ads1015_sample(&analog_raw, 5)); 
    return (uint8_t)(analog_raw * 4.096f * 256.0f / 2048.0f / 3.3f);
}
float getOutputVoltage()
{
    int16_t analog_raw;
    ESP_ERROR_CHECK(ads1015_sample(&analog_raw, 4)); 
    return analog_raw * 4.096f * 7.6667f / 2048.0f;
}
float getOutputCurrent()
{
    int16_t analog_raw;
    ESP_ERROR_CHECK(ads1015_sample(&analog_raw, 4)); 
    return analog_raw * 0.256f / 2048.0f / 0.01f / 20.0f;
}
void updateTargets(float* voltageTarget, float* currentTarget)
{
    uint8_t voltagePot = getPot1();
    uint8_t currentPot = getPot0();

    if(voltagePotEnable)
    {
        float voltageRate;
        int diff = (int)voltagePot - 128;
        if (diff > -POT_DEAD_ZONES && diff < POT_DEAD_ZONES) voltageRate = 0;
        else voltageRate = ((diff > 0) ? 1 : -1)*(VOLTAGE_MIN_SENS + (float)abs(diff) / 128.0f * (VOLTAGE_MAX_SENS - VOLTAGE_MIN_SENS));
    
        if (voltageRate < -VOLTAGE_MIN_SENS / 2 || voltageRate > VOLTAGE_MIN_SENS / 2)
        {
            float newVoltageTarget = *voltageTarget + voltageRate * (float)STATE_MACHINE_TICK_MS / 1000.0f;
            if (newVoltageTarget > VOLTAGE_MAX) *voltageTarget = VOLTAGE_MAX;
            else if (newVoltageTarget < VOLTAGE_MIN) *voltageTarget = VOLTAGE_MIN;
            else *voltageTarget = newVoltageTarget;
        }
    }
    else
    {
        int diff = voltagePot - firstVoltagePotValue;
        if (diff < -POT_DEAD_ZONES || diff > POT_DEAD_ZONES) voltagePotEnable = true;
    }

    if(currentPotEnable)
    {
        float currentRate;
        int diff = (int)currentPot - 128;
        if (diff > -POT_DEAD_ZONES && diff < POT_DEAD_ZONES) currentRate = 0;
        else currentRate = ((diff > 0) ? 1 : -1) * (CURRENT_MIN_SENS + (float)abs(diff) / 128.0f * (CURRENT_MAX_SENS - CURRENT_MIN_SENS));
    
        if (currentRate < -CURRENT_MIN_SENS / 2 || currentRate > CURRENT_MIN_SENS / 2)
        {
            float newcurrentTarget = *currentTarget + currentRate * (float)STATE_MACHINE_TICK_MS / 1000.0f;
            if (newcurrentTarget > CURRENT_MAX) *currentTarget = CURRENT_MAX;
            else if (newcurrentTarget < CURRENT_MIN) *currentTarget = CURRENT_MIN;
            else *currentTarget = newcurrentTarget;
        }
    }
    else
    {
        int diff = currentPot - firstCurrentPotValue;
        if (diff < -POT_DEAD_ZONES || diff > POT_DEAD_ZONES) currentPotEnable = true;
    }
}
