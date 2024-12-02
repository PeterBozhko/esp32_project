#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_gen.h"
#include "driver/mcpwm_cmpr.h"

static const char *TAG = "motor_driver";

#define MCPWM_TIMER_RESOLUTION_HZ 1000000 // 1MHz, 1 tick = 1us (timer freqeuncy)
#define MCPWM_PERIOD_TICKS        100    // ticks count (max duty in tiks)
#define MCPWM_REAL_PWM_FREQ_HZ      (MCPWM_TIMER_RESOLUTION_HZ / MCPWM_PERIOD_TICKS) // maximum value we can set for the duty cycle, in ticks
#define MCPWM_DEAD_TIME_TICKS           3 // 3 * 1 = 3us deadtime

mcpwm_gen_handle_t genA = NULL;
mcpwm_gen_handle_t genB = NULL;
mcpwm_cmpr_handle_t comparatorA = NULL;
mcpwm_cmpr_handle_t comparatorB = NULL;

static inline uint32_t forse_to_compare(int percent)
{
    return percent * MCPWM_PERIOD_TICKS / 100;
}

void create_motor(int GPIO_A, int GPIO_B){
    ESP_LOGI(TAG, "Create timer and operator");
    ESP_LOGI(TAG, "Real pwm frequency = %d hz", MCPWM_REAL_PWM_FREQ_HZ);

    mcpwm_timer_handle_t timer = NULL;

    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = MCPWM_TIMER_RESOLUTION_HZ,
        .period_ticks = MCPWM_PERIOD_TICKS,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    mcpwm_oper_handle_t operA = NULL;
    mcpwm_oper_handle_t operB = NULL;

    mcpwm_operator_config_t operator_config = {
        .group_id = 0, // operator must be in the same group to the timer
    };

    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operA));
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operB));

    ESP_LOGI(TAG, "Connect timer and operator");
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operA, timer));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operB, timer));

    
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };

    ESP_ERROR_CHECK(mcpwm_new_comparator(operA, &comparator_config, &comparatorA));

    ESP_ERROR_CHECK(mcpwm_new_comparator(operB, &comparator_config, &comparatorB));

    ESP_LOGI(TAG, "Create generator from the operator");

    
    mcpwm_generator_config_t generator_configA = {
        .gen_gpio_num = GPIO_A,
    };

    ESP_ERROR_CHECK(mcpwm_new_generator(operA, &generator_configA, &genA));

    
    mcpwm_generator_config_t generator_configB = {
        .gen_gpio_num = GPIO_B,
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(operB, &generator_configB, &genB));

    
    // mcpwm_dead_time_config_t dt_config = {
    //     .negedge_delay_ticks = MCPWM_DEAD_TIME_TICKS,
    // };

    // // Set posedge delay to generator A
    // ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(genA, genA, &dt_config))

    // // go high on counter empty
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(genA,
                                                              MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(genB,
                                                              MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    // go low on compare threshold
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(genA,
                                                                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparatorA, MCPWM_GEN_ACTION_LOW)));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(genB,
                                                                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparatorB, MCPWM_GEN_ACTION_LOW)));

    ESP_LOGI(TAG, "Enable and start timer");

    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

}

void rotateMotor(char direction, int forse){

    ESP_LOGI(TAG, "Set new forse to %d in %d direction", forse, direction);

    if (direction != 0){
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparatorA, forse_to_compare(forse)));
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparatorB, forse_to_compare(0)));
    } else {
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparatorA, forse_to_compare(0)));
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparatorB, forse_to_compare(forse)));
    }
    
}