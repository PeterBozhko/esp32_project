#include <stdio.h>
#include <motor_driver.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_display_driver.h"
#include "esp_log.h"
#include "font.h"
#include "colors.h"
#include "WS2812B.h"

static const char *TAG = "MAIN";

#define MCPWM_GPIO_A              12
#define MCPWM_GPIO_B              14
#define MIN_MOTOR_FORSE           40
#define MAX_MOTOR_FORSE           80

#define DATA_SIZE                 1024


void set_color_brightness(WS2812B_led_color_t *color, int brightness){
    color->r = color->r & brightness;
    color->g = color->g & brightness;
    color->b = color->b & brightness;
}

void app_main(void)
{
    // init_i2c();
    // uint16_t deviceId = findDevice();

    // ESP_LOGI(TAG, "Find new I2C device - %d", deviceId);
    // ESP_LOGI(TAG, "Add I2C device - %d", deviceId);

    // add_device_i2c(deviceId);

    // ESP_LOGI(TAG, "Config I2C device - %d", deviceId);

    // config_i2c();
    // vTaskDelay(200);
    
    // // uint8_t picture[DATA_SIZE];
    // // for (int i = 0; i < DATA_SIZE; i++){
    // //     if (i % 3 == 0) {
    // //         picture[i] = 0xAA;
    // //     }
    // //     else{
    // //         picture[i] = 0x00;
    // //     }
    // // }
    // clearDisplay();
    // ESP_LOGI(TAG, "Set picture - %d", deviceId);
    // // setPic(&picture);

    // vTaskDelay(200);

    // drawChar('H');
    // drawChar('e');
    // drawChar('l');
    // drawChar('l');
    // drawChar('o');
    // drawChar(',');
    // drawChar(' ');
    // drawChar('P');
    // drawChar('e');
    // drawChar('t');
    // drawChar('e');
    // drawChar('r');
    // // vTaskDelay(2000);
    // clearDisplay();
    // vTaskDelay(200);
    // ESP_LOGI(TAG, "Delete I2C device - %d", deviceId);
    // del_i2c();

    // vTaskDelay(200);
    int led_gpio = 3;// 25;
    int led_count = 3; //179; //42
    init_led_strip(led_gpio);
    WS2812B_led_color_t color = RED;
    int brightness = 0xAA;
    set_color_brightness(&color, brightness);
    setSymbol(color, 8);
    vTaskDelay(100);

    while(1){
        color = WHITE;
        brightness += 0x44;
        if (brightness == 0xFF) brightness = 0x11;
        set_color_brightness(&color, brightness);
        ESP_LOGI(TAG, "WHITE : %d %d %d", color.r, color.g, color.b);
        setColorByPixel(color);
        vTaskDelay(200);
        color = RED;
        set_color_brightness(&color, brightness);
        ESP_LOGI(TAG, "RED : %d %d %d", color.r, color.g, color.b);
        setColorByPixel(color);
        vTaskDelay(200);
        color = GREEN;
        ESP_LOGI(TAG, "GREEN");
        set_color_brightness(&color, brightness);
        ESP_LOGI(TAG, "GREEN : %d %d %d", color.r, color.g, color.b);
        setColorByPixel(color);
        vTaskDelay(200);
        color = BLUE;
        ESP_LOGI(TAG, "BLUE");
        set_color_brightness(&color, brightness);
        ESP_LOGI(TAG, "BLUE : %d %d %d", color.r, color.g, color.b);    
        setColorByPixel(color);
        vTaskDelay(200);
        color = YELLOW;
        ESP_LOGI(TAG, "YELLOW");
        set_color_brightness(&color, brightness);
        ESP_LOGI(TAG, "YELLOW : %d %d %d", color.r, color.g, color.b);
        setColorByPixel(color);
        vTaskDelay(200);
        color = CYAN;   
        ESP_LOGI(TAG, "CYAN");
        set_color_brightness(&color, brightness);
        ESP_LOGI(TAG, "CYAN : %d %d %d", color.r, color.g, color.b);
        setColorByPixel(color);
        vTaskDelay(200);
        color = MAGENTA;
        ESP_LOGI(TAG, "MAGENTA");
        set_color_brightness(&color, brightness);
        ESP_LOGI(TAG, "MAGENTA : %d %d %d", color.r, color.g, color.b);
        setColorByPixel(color);
        vTaskDelay(200);   
    }
    
    
    del();

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

//     static httpd_handle_t server = NULL;

//     ESP_ERROR_CHECK(nvs_flash_init());
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());

//     /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
//      * Read "Establishing Wi-Fi or Ethernet Connection" section in
//      * examples/protocols/README.md for more information about this function.
//      */
//     ESP_ERROR_CHECK(example_connect());

//     /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
//      * and re-start it upon connection.
//      */
// #if !CONFIG_IDF_TARGET_LINUX
// #ifdef CONFIG_EXAMPLE_CONNECT_WIFI
//     ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
//     ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
// #endif // CONFIG_EXAMPLE_CONNECT_WIFI
// #ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
//     ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &connect_handler, &server));
//     ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &disconnect_handler, &server));
// #endif // CONFIG_EXAMPLE_CONNECT_ETHERNET
// #endif // !CONFIG_IDF_TARGET_LINUX

//     /* Start the server for the first time */
//     server = start_webserver();

//     while (server) {
//         sleep(5);
//     }
    
}