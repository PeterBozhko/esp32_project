#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include <stdlib.h> // для функций calloc и malloc
#include <font.h>

#define I2C_MASTER_SCL_IO     22
#define I2C_MASTER_SDA_IO     21
#define CACHE_SIZE            1024 // 128 * 8

static char cache[CACHE_SIZE];
static char display_mem[CACHE_SIZE];

static const char *TAG = "i2c_driver";


i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = I2C_NUM_0,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;
uint8_t _counter;

void init_i2c(){
    ESP_LOGI(TAG, "Init I2C");
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    // ESP_ERROR_CHECK(i2c_master_probe(bus_handle, 0x22, -1));

    // i2c_device_config_t dev_cfg = {
        // .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        // .device_address = 0x58,
        // .scl_speed_hz = 100000,
    // };

    // i2c_master_dev_handle_t dev_handle;
    // ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    
    

    // ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
    // i2c_master_receive(dev_handle, data_rd, DATA_LENGTH, -1);

    // uint8_t buf[20] = {0x20};
    // uint8_t buffer[2];
    // ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, buf, sizeof(buf), buffer, 2, -1));
}

uint16_t findDevice(){
    // выделение памяти инициализированной нулями
    // uint16_t *d= (uint16_t*)calloc(128, sizeof(uint16_t));
    ESP_LOGI(TAG, "Start find I2C");
    esp_err_t ack = 0;
    uint16_t deviceAddress = 0;
    for (uint16_t i = 0; i < 127; i++){
        ack = i2c_master_probe(bus_handle, i, -1);
        ESP_LOGI(TAG, "check %d address - %d", i, ack == ESP_OK);
        if (ack == ESP_OK){
            deviceAddress = i;
        }
    }
    return deviceAddress;
}

void add_device_i2c(uint16_t deviceId){
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = deviceId,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
}


void del_i2c(){
    ESP_ERROR_CHECK(i2c_master_bus_rm_device(dev_handle));
    ESP_ERROR_CHECK(i2c_del_master_bus(bus_handle));
}


#define DISPLAY_OFF                           (uint8_t)0xAE
#define SET_MUX_RATIO                         0xA8
#define SET_DISPLAY_OFFSET                    0xD3
#define SET_DISPLAY_START_LINE                0x40
#define SET_SEGMENT_REMAP                     0xA0
#define SET_COM_OUTPUT_SCAN_DIRECTION         0xC0
#define SET_COM_PINS_HARDWARE_CONFIGURATION   0xDA
#define SET_CONTRAST_CONTROL                  0x81
#define DISABLE_ENTIRE_DISPLAY_ON             0xA4
#define SET_NORMAL_DISPLAY                    0xA6
#define SET_OSC_FREQUENCY                     0xD5
#define ENABLE_CHARGE_PUMP_REGULATOR          0x8D
#define DISPLAY_ON                            0xAF

void sendCommand(uint8_t data);
void sendData(uint8_t data);

void config_i2c(){
    // uint8_t control_byte = 0b0;
    // uint8_t data_buf[24] = {
    //     0x00, 
    //     SET_MUX_RATIO, 
    //     0x00, 
    //     SET_DISPLAY_OFFSET, 
    //     0x00,
    //     SET_DISPLAY_START_LINE,
    //     0x00, 
    //     SET_SEGMENT_REMAP,
    //     0x00, 
    //     SET_COM_OUTPUT_SCAN_DIRECTION,
    //     0x00, 
    //     SET_COM_PINS_HARDWARE_CONFIGURATION,
    //     0x00, 
    //     SET_CONTRAST_CONTROL,
    //     0x00, 
    //     DISABLE_ENTIRE_DISPLAY_ON,
    //     0x00, 
    //     SET_NORMAL_DISPLAY,
    //     0x00, 
    //     SET_OSC_FREQUENCY,
    //     0x00, 
    //     ENABLE_CHARGE_PUMP_REGULATOR,
    //     0x00, 
    //     DISPLAY_ON
    //     };

    sendCommand(DISPLAY_OFF);
    sendCommand(SET_OSC_FREQUENCY);
    sendCommand(0x80);
    sendCommand(SET_MUX_RATIO);
    sendCommand(0x3F);
    sendCommand(SET_DISPLAY_OFFSET);
    sendCommand(0x00);
    sendCommand(SET_DISPLAY_START_LINE);
    sendCommand(ENABLE_CHARGE_PUMP_REGULATOR);
    sendCommand(0x14);
    sendCommand(0x20);
    sendCommand(0x00);
    sendCommand(SET_SEGMENT_REMAP);
    sendCommand(0xC8);
    sendCommand(SET_COM_PINS_HARDWARE_CONFIGURATION);
    sendCommand(0x12);
    sendCommand(SET_COM_OUTPUT_SCAN_DIRECTION);
    sendCommand(SET_CONTRAST_CONTROL);
    sendCommand(0xCF);
    sendCommand(0xD9); //SETPRECHARGE
    sendCommand(0xF1);
    sendCommand(0xDB); //SETVCOMDETECT
    sendCommand(0x40);
    sendCommand(DISABLE_ENTIRE_DISPLAY_ON);
    sendCommand(SET_NORMAL_DISPLAY);
    sendCommand(DISPLAY_ON);
    _counter = 0;
        // ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, data_buf, DATA_LENGTH, -1));
}

void flush(){
// позиционирование
    for (uint8_t page = 0xB0; page < 0xB8; page++){
        for (uint8_t seg = 0x00; seg < 0x80; seg++){
            // позиционирование
            if (cache[(page & 0x0F) * 128 + seg] == display_mem[(page & 0x0F) * 128 + seg]) continue;
            sendCommand(page);
            sendCommand(0x0F & seg);
            sendCommand(0x10 | (seg >> 4));
            // ESP_LOGI(TAG, "SET page - %d, col - %d, byte - %d", (page & 0x0F), seg, data[(page & 0x0F) * 128 + seg]);
            // установка цвета
            sendData(cache[(page & 0x0F) * 128 + seg]);
            display_mem[(page & 0x0F) * 128 + seg] = cache[(page & 0x0F) * 128 + seg];
            // vTaskDelay();
        }
    }
}

#define DATA_LENGTH 1024
void setPic(uint8_t *data){
    // позиционирование
    for (uint8_t page = 0xB0; page < 0xB8; page++){
        for (uint8_t seg = 0x00; seg < 0x80; seg++){
            // позиционирование
            sendCommand(page);
            sendCommand(0x0F & seg);
            sendCommand(0x10 | (seg >> 4));
            ESP_LOGI(TAG, "SET page - %d, col - %d, byte - %d", (page & 0x0F), seg, data[(page & 0x0F) * 128 + seg]);
            // установка цвета
            sendData(data[(page & 0x0F) * 128 + seg]);
            // vTaskDelay();
        }
    }
}

void clearDisplay(){
    for(short i = 0; i < CACHE_SIZE; i++){
        cache[i] = 0x00;
        display_mem[i] = 0xFF;
    };
    flush();
}

// void setData(uint8_t page, uint8_t col, uint8_t data){
//     cache[page * 128 + col] = data;
// }

void drawChar(char c){
    uint8_t i = 0;
    _counter %= 1023;
    
    while (i < CHARS_COLS_LENGTH) {
        cache[_counter++] = FONT[c-32][i++];
        _counter %= 1023;
    }
    _counter++;
    _counter %= 1023;
    flush();
}

void sendCommand(uint8_t data){
    uint8_t buffer[2];
    buffer[0] = 0x00;
    buffer[1] = data;
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, buffer, 2, -1));
}

void sendData(uint8_t data){
    uint8_t buffer[2];
    buffer[0] = 0x40;
    buffer[1] = data;
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, buffer, 2, -1));
}