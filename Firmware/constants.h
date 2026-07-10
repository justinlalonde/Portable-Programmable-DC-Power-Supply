#define I2C_SCL_IO              18
#define I2C_SDA_IO              19
#define I2C_PORT                I2C_NUM_0
#define I2C_FREQ_HZ             100000

#define ADS1015_ADDR0           0x48 
#define ADS1015_ADDR1           0x49 
#define LP5024_V_ADDR           0x28
#define LP5024_A_ADDR           0x29
#define LP5024_W_ADDR           0x2A
#define LP5024_BROADCAST_ADDR   0x3C
#define TPS55287_ADDR           0x75 

#define AUXOFF_IO               14       
#define NFAULT_IO               27
#define STAT_IO                 32
#define INT_IO                  13          
#define LEDG_IO                 22
#define LEDR_IO                 23
#define RDY0_IO                 33
#define RDY1_IO                 25

#define BATT_VOLT_FLASH_TCIK    (START_TIMER_TICKS) * STATE_MACHINE_TICK_MS

#define STATE_MACHINE_TICK_MS   50
#define OC_BUFFER_TICKS         4   
#define OC_LAST_TICKS           3
#define ES_LAST_TICKS           5
#define OO_STREAK_TICKS         4 
#define SC_STREAK_TICKS         4  
#define START_TIMER_TICKS       5 

#define CURRENT_LOW_THRSH       0.005f

#define VOLTAGE_MAX             20.0f
#define VOLTAGE_MIN             3.0f
#define CURRENT_MAX             3.0f
#define CURRENT_MIN             0.0f

#define CURR_CORR_FACTOR        1.0f/0.8532f

#define VOLTAGE_MAX_SENS        2.50f // V/s 
#define VOLTAGE_MIN_SENS        0.01f // V/s 
#define CURRENT_MAX_SENS        0.75f // V/s 
#define CURRENT_MIN_SENS        0.05f // V/s 
#define POT_DEAD_ZONES          8     // int on 8 bits   

#define DISPLAY_BRIGHTNESS_V    0x80
#define DISPLAY_BRIGHTNESS_A    0x70
#define DISPLAY_BRIGHTNESS_W    0x20