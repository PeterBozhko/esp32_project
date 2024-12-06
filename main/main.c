#include <stdio.h>
#include <motor_driver.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_display_driver.h"
#include "esp_log.h"

static const char *TAG = "MAIN";

#define MCPWM_GPIO_A              12
#define MCPWM_GPIO_B              14
#define MIN_MOTOR_FORSE           40
#define MAX_MOTOR_FORSE           80

#define DATA_SIZE                 1024

void app_main(void)
{
    init_i2c();

    uint16_t deviceId = findDevice();

    ESP_LOGI(TAG, "Find new I2C device - %d", deviceId);
    ESP_LOGI(TAG, "Add I2C device - %d", deviceId);

    add_device_i2c(deviceId);

    ESP_LOGI(TAG, "Config I2C device - %d", deviceId);

    config_i2c();
    vTaskDelay(200);

    
    uint8_t picture[DATA_SIZE];
    for (int i = 0; i < DATA_SIZE; i++){
        if (i % 3 == 0) {
            picture[i] = 0xAA;
        }
        else{
            picture[i] = 0x00;
        }
    }
    clearDisplay();
    ESP_LOGI(TAG, "Set picture - %d", deviceId);
    setPic(&picture);

    vTaskDelay(2000);

    ESP_LOGI(TAG, "Delete I2C device - %d", deviceId);
    del_i2c();
    // create_motor(MCPWM_GPIO_B, MCPWM_GPIO_A);

    // char direction = 0;
    // int forse = MIN_MOTOR_FORSE;
    // int step = 1;
    // while (1)
    // {
    //     if (forse == MAX_MOTOR_FORSE) step = -1;
    //     if (forse == MIN_MOTOR_FORSE){
    //         step = 1;
    //         direction = 1 - direction;
    //     }
        
    //     forse += step;
    //     rotateMotor(direction, forse);

    //     vTaskDelay(5);
    // }
    
}
