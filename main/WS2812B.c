#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "WS2812B.h"
#include "led_strip_encoder.h"
#include <string.h>

static const char *TAG = "WS2812B";

/*
0b07654321
   3      ---
 2   4   |   |
   1      ---
 7   5   |   |
   6      ---
*/
static const uint8_t SYMBOLS[10] = {
    // 0b01000000, // 0
    0b01111110, // 0
    0b00011000, // 1
    0b01101101, // 2
    0b00111101, // 3
    0b00011011, // 4
    0b00110111, // 5
    0b01110111, // 6
    0b00011100, // 7
    0b01111111, // 8
    0b00111111, // 9
};

#define MEM_BLOCK_SYMBOLS              64          // memory block size, 64 * 4 = 256 Bytes
#define TIMER_RESOLUTION_HZ            10000000    // 10MHz tick resolution, i.e., 1 tick = 0.01 µs
#define TRANS_QUEUE_DEPTH              4           // set the number of transactions that can pend in the background
#define COLOR_DELAY                    10          // ms
#define LEDS_IN_SEGMENT                1 //2           // leds count in one segment
#define TOTAL_LEDS_COUNT               LEDS_IN_SEGMENT * 7

static uint8_t LEDS[TOTAL_LEDS_COUNT * 3];

rmt_channel_handle_t tx_chan = NULL;
rmt_encoder_handle_t led_encoder = NULL;
rmt_transmit_config_t tx_config = {
        .loop_count = 0, // no transfer loop
    };


void updateLeds(){
    ESP_ERROR_CHECK(rmt_transmit(tx_chan, led_encoder, LEDS, sizeof(LEDS), &tx_config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_chan, portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(COLOR_DELAY));
}

void setPixelColor(WS2812B_led_color_t new_color, int pixel){
    // ESP_LOGI(TAG, "New Color");
    if (pixel < TOTAL_LEDS_COUNT) {
        LEDS[pixel * 3] = new_color.g;
        LEDS[pixel * 3 + 1] = new_color.r;
        LEDS[pixel * 3 + 2] = new_color.b;
    }
    updateLeds();
}

void setColor(WS2812B_led_color_t color){
    for (int i = 0; i < TOTAL_LEDS_COUNT; i++){
        LEDS[i * 3] = color.g;
        LEDS[i * 3 + 1] = color.r;
        LEDS[i * 3 + 2] = color.b;
    }
    updateLeds();
}

void setColorByPixel(WS2812B_led_color_t color){
    for (int i = 0; i < TOTAL_LEDS_COUNT; i++){
        setPixelColor(color, i);
    }
}
void setSymbol(WS2812B_led_color_t color, int sym){
    ESP_LOGI(TAG, "New symbol %d", sym);
    if (sym < 0 || sym > 10){
        ESP_LOGI(TAG, "Wrong symbol");
        return;
    }
    WS2812B_led_color_t new_color;
    uint8_t symBytes = SYMBOLS[sym];
    for (int i = 0; i < 8; i++){

        if (symBytes & (1 << i)) {
            new_color = color;
        }
        else{
            new_color.r = 0;
            new_color.g = 0;
            new_color.b = 0;
        }
        
        for (int j = 0; j < LEDS_IN_SEGMENT; j++){
                int led_num = i * LEDS_IN_SEGMENT + j;
                LEDS[led_num * 3] = new_color.g;
                LEDS[led_num * 3 + 1] = new_color.r;
                LEDS[led_num * 3 + 2] = new_color.b;
            }
    }
    updateLeds();
}

void init_led_blink(){
    WS2812B_led_color_t color = {
        .raw = {0x44, 0x44, 0x44}
    };
    for (int i = 0; i < TOTAL_LEDS_COUNT; i++){
        setColor(color);
    }
    vTaskDelay(100);
    memset(LEDS, 0, sizeof(LEDS));
    updateLeds();
    vTaskDelay(100);
    memset(LEDS, 0x44, sizeof(LEDS));
    updateLeds();
    vTaskDelay(100);
    memset(LEDS, 0, sizeof(LEDS));
    updateLeds();
    vTaskDelay(100);
}

void init_led_strip(int gpio_num){
    memset(LEDS, 0, sizeof(LEDS));
    ESP_LOGI(TAG, "Create led strip channel");

    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,         // select source clock
        .gpio_num = gpio_num,                   // GPIO number
        .mem_block_symbols = MEM_BLOCK_SYMBOLS, // memory block size, 64 * 4 = 256 Bytes
        .resolution_hz = TIMER_RESOLUTION_HZ,   // 80 MHz tick resolution, i.e., 1 tick = 0.0125 µs
        .trans_queue_depth = TRANS_QUEUE_DEPTH, // set the number of transactions that can pend in the background
        .flags.invert_out = false,              // do not invert output signal
        .flags.with_dma = false,                // do not need DMA backend
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));

    ESP_LOGI(TAG, "Install led strip encoder");

    led_strip_encoder_config_t encoder_config = {
        .resolution = TIMER_RESOLUTION_HZ,
    };

    ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));

    ESP_LOGI(TAG, "Enable RMT TX channel");
    ESP_ERROR_CHECK(rmt_enable(tx_chan));
    updateLeds();
    init_led_blink();
}

void del(){
    ESP_LOGI(TAG, "Delete RMT channel");
    ESP_ERROR_CHECK(rmt_disable(tx_chan));
    ESP_ERROR_CHECK(rmt_del_channel(tx_chan));
}
