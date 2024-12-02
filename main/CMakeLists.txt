idf_component_register(SRCS "motor_driver.c" "main.c" "motor_driver.h"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_mcpwm)