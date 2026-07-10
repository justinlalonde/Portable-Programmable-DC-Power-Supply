#include "ads1015.h"

static i2c_master_dev_handle_t ads_handle_0;
static i2c_master_dev_handle_t ads_handle_1;

static esp_err_t ads1015_write(uint16_t value, uint8_t registerAddress, uint8_t chipIndex)
{
    if (registerAddress == 0x00 || registerAddress > 0x03) return ESP_ERR_INVALID_ARG;
    if (chipIndex > 1) return ESP_ERR_INVALID_ARG;
    uint8_t data[3];
    data[0] = registerAddress;      // Register pointer for config register
    data[1] = (value >> 8) & 0xFF;  // MSB byte
    data[2] = value & 0xFF;         // LSB byte
    return i2c_master_transmit(chipIndex ? ads_handle_1 : ads_handle_0, data, 3, -1);
}
static void ads1015_setup(void)
{
    ESP_ERROR_CHECK(ads1015_write(0x4303, 0x01, 0)); // ADS1015 config register write (0 100 001 1 100 0 0 0 00) 
    ESP_ERROR_CHECK(ads1015_write(0x0000, 0x02, 0)); // ADS1015 lo_thresh register write
    ESP_ERROR_CHECK(ads1015_write(0x8000, 0x03, 0)); // ADC1015 hi_thresh register write
    ESP_ERROR_CHECK(ads1015_write(0x4303, 0x01, 1)); // ADS1015 config register write (0 100 001 1 100 0 0 0 00)
    ESP_ERROR_CHECK(ads1015_write(0x0000, 0x02, 1)); // ADS1015 lo_thresh register write
    ESP_ERROR_CHECK(ads1015_write(0x8000, 0x03, 1)); // ADC1015 hi_thresh register write
}
void ads1015_init(i2c_master_bus_handle_t bus_handle)
{
    i2c_device_config_t dev_config_0 = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ADS1015_ADDR0,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    i2c_device_config_t dev_config_1 = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ADS1015_ADDR1,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config_0, &ads_handle_0));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config_1, &ads_handle_1));
    ads1015_setup();
    gpio_config_t io_conf_0 = {
        .pin_bit_mask = (1ULL << RDY0_IO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf_0));
    gpio_config_t io_conf_1 = {
        .pin_bit_mask = (1ULL << RDY1_IO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf_1));
}
static esp_err_t ads1015_read(uint16_t *result, uint8_t registerAddress, uint8_t chipIndex)
{
    if (registerAddress > 0x03) return ESP_ERR_INVALID_ARG;
    uint8_t data[2];
    ESP_ERROR_CHECK(i2c_master_transmit(chipIndex ? ads_handle_1 : ads_handle_0, &registerAddress, 1, -1));
    ESP_ERROR_CHECK(i2c_master_receive(chipIndex ? ads_handle_1 : ads_handle_0, data, 2, -1));
    *result = (data[0] << 8) | data[1];
    return ESP_OK;
}
esp_err_t ads1015_sample(int16_t *result, uint8_t channel)
{
    if (channel > 7 || channel == 1 || channel == 3 || channel == 6) return ESP_ERR_INVALID_ARG;
    const uint16_t configs[8] = {0xC380, 0, 0xE380, 0, 0xC380, 0xD380, 0, 0xFF80};
    ESP_ERROR_CHECK(ads1015_write(configs[channel], 0x01, channel / 4)); // ADS1015 config register write (1 XXX 001 1 100 0 0 0 00) = 0x8380 = 33664
    uint16_t config;
    do {
        ads1015_read(&config, 0x01, channel / 4); // read config register OS bit for end of conversion
    } while ((config & 0x8000) == 0); 
    uint16_t value;
    ESP_ERROR_CHECK(ads1015_read(&value, 0x00, channel / 4)); // AD1015 conversion regiter read
    *result = (int16_t)value / 16; // signed arithmetic shift 
    return ESP_OK;
}
bool getRDY0Status(void)
{
    return gpio_get_level(RDY0_IO);
}
bool getRDY1Status(void)
{
    return gpio_get_level(RDY1_IO);
}
