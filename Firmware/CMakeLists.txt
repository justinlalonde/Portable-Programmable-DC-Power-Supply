idf_component_register(
    SRCS "misc.c" "led.c" "state_machine.c" "tps2115.c" "tps259470.c" "main.c" "tps55287.c" "ads1015.c" "display7seg.c" "lp5024.c"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_i2c
    REQUIRES esp_driver_gpio
    )