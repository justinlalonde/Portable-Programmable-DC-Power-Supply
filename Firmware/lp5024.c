#include "lp5024.h"

static i2c_master_dev_handle_t lp_v_handle; //voltage handle
static i2c_master_dev_handle_t lp_a_handle; //current handle
static i2c_master_dev_handle_t lp_w_handle; //power handle
static i2c_master_dev_handle_t lp_b_handle; //broadcast handle

static uint32_t old_v_value = 0; // dernières activations des 24 OUT pins du lp5024 concerné
static uint32_t old_a_value = 0;
static uint32_t old_w_value = 0;
static uint32_t old_b_value = 0;

static bool display_v_on = false, display_a_on = false, display_w_on = false;

static uint8_t writeBuffer[24];

static esp_err_t lp5024_write(uint8_t value, uint8_t registerAddress, char chipID)
{
    // chipID prend la valeur v, a, w ou b (tension, courant, puissance ou broadcast)
    if (registerAddress > 0x27) return ESP_ERR_INVALID_ARG;
    uint8_t data[2] = {registerAddress, value};
    switch(chipID){
        case 'v' :
            return i2c_master_transmit(lp_v_handle, data, 2, -1);
        case 'a' :
            return i2c_master_transmit(lp_a_handle, data, 2, -1);
        case 'w' :
            return i2c_master_transmit(lp_w_handle, data, 2, -1);
        case 'b' :
            return i2c_master_transmit(lp_b_handle, data, 2, -1);
        default : 
            return ESP_ERR_INVALID_ARG;
    }
}
static void lp5024_setup(void)
{
    ESP_ERROR_CHECK(lp5024_write(0x40, 0x00, 'b')); // lp5024 config0 register write (x 1 xx xxxx) chip EN
    ESP_ERROR_CHECK(lp5024_write(0x1D, 0x01, 'b')); // lp5024 config1 register write (xx 0 1 1 1 0 1); all LED off
    ESP_ERROR_CHECK(lp5024_write(0x00, 0x02, 'b')); // lp5024 config2 register write (0000 0000)
}
void lp5024_init(i2c_master_bus_handle_t bus_handle)
{
    i2c_device_config_t dev_v_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LP5024_V_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    i2c_device_config_t dev_a_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LP5024_A_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    i2c_device_config_t dev_w_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LP5024_W_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    i2c_device_config_t dev_b_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LP5024_BROADCAST_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_v_config, &lp_v_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_a_config, &lp_a_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_w_config, &lp_w_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_b_config, &lp_b_handle));
    lp5024_setup();
    for (int i = 0; i < 24; i++) writeBuffer[i] = 0xFF;
}
static esp_err_t lp5024_read(uint8_t *result, uint8_t registerAddress, char chipID)
{
    if (registerAddress > 0x26) return ESP_ERR_INVALID_ARG;
    switch(chipID){
        case 'v' :
            ESP_ERROR_CHECK(i2c_master_transmit(lp_v_handle, &registerAddress, 1, -1));
            ESP_ERROR_CHECK(i2c_master_receive(lp_v_handle, result, 1, -1));
            break;
        case 'a' :
            ESP_ERROR_CHECK(i2c_master_transmit(lp_a_handle, &registerAddress, 1, -1));
            ESP_ERROR_CHECK(i2c_master_receive(lp_a_handle, result, 1, -1));
            break;
        case 'w' :
            ESP_ERROR_CHECK(i2c_master_transmit(lp_w_handle, &registerAddress, 1, -1));
            ESP_ERROR_CHECK(i2c_master_receive(lp_w_handle, result, 1, -1));
            break;
        default : //pas de read avec broadcast
            return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}
static esp_err_t lp5024_consecutiveWrite(const uint8_t *values, uint8_t startRegisterAddresss, char chipID, uint8_t length)
{
    if (startRegisterAddresss + length - 1 > 0x27 || length == 0) return ESP_ERR_INVALID_ARG;
    uint8_t buffer[length + 1];
    buffer[0] = startRegisterAddresss;
    memcpy(&buffer[1], values, length);
    switch(chipID){
        case 'v' :
            return i2c_master_transmit(lp_v_handle, buffer, length + 1, -1);
        case 'a' :
            return i2c_master_transmit(lp_a_handle, buffer, length + 1, -1);
        case 'w' :
            return i2c_master_transmit(lp_w_handle, buffer, length + 1, -1);
        case 'b' :
            return i2c_master_transmit(lp_b_handle, buffer, length + 1, -1);
        default : 
            return ESP_ERR_INVALID_ARG;
    }
}
void setBrightness(uint8_t brightness, char chipID)
{
    uint8_t values[8];
    for (int i = 0; i < 8; i++) values[i] = brightness;
    ESP_ERROR_CHECK(lp5024_consecutiveWrite(values, 0x07, chipID, 8));
}
esp_err_t displayOn(char chipID)
{
    bool* onVariable;
    switch(chipID){
        case 'v' : onVariable = &display_v_on; break;
        case 'a' : onVariable = &display_a_on; break;
        case 'w' : onVariable = &display_w_on; break;
        case 'b' : 
            ESP_ERROR_CHECK(displayOn('v'));
            ESP_ERROR_CHECK(displayOn('a'));
            ESP_ERROR_CHECK(displayOn('w'));
            return ESP_OK;
        default : return ESP_ERR_INVALID_ARG;
    }
    if(!*onVariable){
        ESP_ERROR_CHECK(lp5024_write(0x1C, 0x01, chipID)); // lp5024 config1 register write (xx 0 1 1 1 0 0); normal operation
        *onVariable = true;
    }
    return ESP_OK;
}
esp_err_t displayOff(char chipID)
{
    bool* onVariable;
    switch(chipID){
        case 'v' : onVariable = &display_v_on; break;
        case 'a' : onVariable = &display_a_on; break;
        case 'w' : onVariable = &display_w_on; break;
        case 'b' : 
            ESP_ERROR_CHECK(displayOff('v'));   
            ESP_ERROR_CHECK(displayOff('a'));
            ESP_ERROR_CHECK(displayOff('w'));
            return ESP_OK;
        default : return ESP_ERR_INVALID_ARG;
    }
    if(*onVariable){
        ESP_ERROR_CHECK(lp5024_write(0x1D, 0x01, chipID)); // lp5024 config1 register write (xx 0 1 1 1 0 1); all LED off
        *onVariable = false;
    }
    return ESP_OK;
}
esp_err_t display(uint32_t value, char chipID)
{
    if (value > 0x00FFFFFF) return ESP_ERR_INVALID_ARG;
    uint32_t *old_value_ptr;
    switch (chipID){
        case 'v' : old_value_ptr = &old_v_value; break;
        case 'a' : old_value_ptr = &old_a_value; break;  
        case 'w' : old_value_ptr = &old_w_value; break;
        case 'b' : old_value_ptr = &old_b_value; break;
        default  : return ESP_ERR_INVALID_ARG;
    }
    uint32_t diff = value ^ *old_value_ptr;
    for(int i = 0, changeLength = 0; i <= 24; i++){
        if(diff & (1UL<<i)){ // if this output has to change state
            writeBuffer[changeLength] = value & (1UL<<i) ? 0xFF : 0x00; // is this output now high or low
            changeLength++;
        }
        else if (changeLength > 0)
        {
            ESP_ERROR_CHECK(lp5024_consecutiveWrite(writeBuffer, 0x0F + i - changeLength, chipID, changeLength));
            changeLength = 0;
        }
    }
    *old_value_ptr = value;
    return ESP_OK;
}
