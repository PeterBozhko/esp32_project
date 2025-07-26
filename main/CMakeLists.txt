idf_component_register(SRCS "led_strip_encoder.c" "WS2812B.c" "i2c_display_driver.c" "motor_driver.c" "main.c" "motor_driver.h" "i2c_display_driver.h" "WS2812B.h" "WS2812B.c"
                    INCLUDE_DIRS "." "include"
                    REQUIRES esp_driver_mcpwm driver)