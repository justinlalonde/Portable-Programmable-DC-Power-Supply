#include "tps55287.h"

static i2c_master_dev_handle_t tps_handle;
static bool output_on = false;

static esp_err_t tps55287_write(uint8_t value, uint8_t registerAddress)
{
    if (registerAddress > 0x06) return ESP_ERR_INVALID_ARG;
    uint8_t data[2] = {registerAddress, value};
    return i2c_master_transmit(tps_handle, data, 2, -1);
}
static void tps55287_setup(void)
{
    ESP_ERROR_CHECK(tps55287_write(0xBC, 0x02)); // tps55287 IOUT_LIMIT register write (3A limit)
    ESP_ERROR_CHECK(tps55287_write(0x40, 0x05)); // tps55287 CDC register write (only OCP mask enabled) + cDC uses external resistor
    ESP_ERROR_CHECK(tps55287_write(0x00, 0x06)); // tps55287 MODE register (HICCUP behaviour disabled)
}
void tps55287_init(i2c_master_bus_handle_t bus_handle)
{
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = TPS55287_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &tps_handle));
    tps55287_setup();gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << INT_IO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
}
static esp_err_t tps55287_read(uint8_t *result, uint8_t registerAddress)
{
    if (registerAddress > 0x07) return ESP_ERR_INVALID_ARG;
    ESP_ERROR_CHECK(i2c_master_transmit(tps_handle, &registerAddress, 1, -1));
    ESP_ERROR_CHECK(i2c_master_receive(tps_handle, result, 1, -1));
    return ESP_OK;
}
static esp_err_t tps55287_consecutiveWrite(const uint8_t *values, uint8_t startRegisterAddresss, uint8_t length)
{
    if (startRegisterAddresss + length - 1 > 0x6 || length == 0) return ESP_ERR_INVALID_ARG;
    uint8_t buffer[length + 1];
    buffer[0] = startRegisterAddresss;
    memcpy(&buffer[1], values, length);
    return i2c_master_transmit(tps_handle, buffer, length + 1, -1);
}
void outputOn()
{
    if(output_on) return;
    ESP_ERROR_CHECK(tps55287_write(0x80, 0x06));
    output_on = true;
}
void outputOff(void)
{
    if(!output_on) return;
    ESP_ERROR_CHECK(tps55287_write(0x00, 0x06));
    output_on = false;
}
esp_err_t setOutputValues(float voltageValue, float currentLimit)
{
    if (voltageValue < VOLTAGE_MIN - 0.01f || voltageValue > VOLTAGE_MAX + 0.01f) return ESP_ERR_INVALID_ARG;
    if (currentLimit < CURRENT_MIN - 0.01f || currentLimit > CURRENT_MAX + 0.01f) return ESP_ERR_INVALID_ARG;
    if (voltageValue < VOLTAGE_MIN) voltageValue = VOLTAGE_MIN;
    if (voltageValue > VOLTAGE_MAX) voltageValue = VOLTAGE_MAX;
    uint16_t vRef = (uint16_t)((voltageValue * 0.0564f - 0.045f) / 0.0005645f + 0.5f);
    uint8_t firstRegisters[3];
    firstRegisters[0] = vRef & 0xFF;
    firstRegisters[1] = (vRef >> 8) & 0xFF;
    if (currentLimit < CURRENT_MIN) currentLimit = CURRENT_MIN;
    if (currentLimit > CURRENT_MAX) currentLimit = CURRENT_MAX;
    uint8_t limit = (uint8_t)(currentLimit * 10.0f / 0.5f * CURR_CORR_FACTOR + 0.5f);
    firstRegisters[2] = limit + 0x80;
    return tps55287_consecutiveWrite(firstRegisters, 0x00, 3);
}
bool getOCPStatus(void)
{
    uint8_t statusRegister;
    tps55287_read(&statusRegister, 0x07);
    return statusRegister & 0x40;
}
bool getINTStatus(void)
{
    return gpio_get_level(INT_IO);
}