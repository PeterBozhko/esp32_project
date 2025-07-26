#pragma once

// typedef struct 
// {
//     uint8_t g;
//     uint8_t r;
//     uint8_t b;
// } WS2812B_led_color_t;

typedef union  
{
    struct {
        union {
            uint8_t b;     ///< Blue channel value
            uint8_t blue;  ///< @copydoc b
        };
        union {
            uint8_t g;      ///< Green channel value
            uint8_t green;  ///< @copydoc g
        };
        union {
            uint8_t r;    ///< Red channel value
            uint8_t red;  ///< @copydoc r
        };
        union {
            uint8_t a;     ///< Alpha channel value
            uint8_t alpha; ///< @copydoc a
        };
    };
    /// Access the alpha, red, green, and blue data as an array.
    /// Where:
    /// * `raw[3]` is the alpha value
    /// * `raw[2]` is the red value
    /// * `raw[1]` is the green value
    /// * `raw[0]` is the blue value

    uint8_t raw[4]; 
    uint32_t color; // 0xAARRGGBB
} WS2812B_led_color_t;


void init_led_strip(int gpio);
void setPixelColor(WS2812B_led_color_t color, int pixel);
void setColor(WS2812B_led_color_t color);
void setColorByPixel(WS2812B_led_color_t color);
void del();
void setSymbol(WS2812B_led_color_t color, int sym);

// WS2812B_led_color_t(uint32_t colorcode){
//     r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF), b((colorcode >> 0) & 0xFF)
// };

// IRmtLedStrip* create_rmt_led_strip(
//         uint16_t T0H, uint16_t T0L, uint16_t T1H, uint16_t T1L, uint32_t TRESET, // Timing is in nanoseconds
//         int pin, uint32_t max_leds, bool is_rgbw);

// tipical color - 0xFFFAF4