idf_component_register(SRCS "i2c_display_driver.c" "motor_driver.c" "main.c" "motor_driver.h" "i2c_display_driver.h"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_mcpwm driver)