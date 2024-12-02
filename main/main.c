#include <stdio.h>
#include <motor_driver.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MCPWM_GPIO_A              12
#define MCPWM_GPIO_B              14
#define MIN_MOTOR_FORSE           40
#define MAX_MOTOR_FORSE           80

void app_main(void)
{
    create_motor(MCPWM_GPIO_B, MCPWM_GPIO_A);

    char direction = 0;
    int forse = MIN_MOTOR_FORSE;
    int step = 1;
    while (1)
    {
        if (forse == MAX_MOTOR_FORSE) step = -1;
        if (forse == MIN_MOTOR_FORSE){
            step = 1;
            direction = 1 - direction;
        }
        
        forse += step;
        rotateMotor(direction, forse);

        vTaskDelay(5);
    }
    
}
