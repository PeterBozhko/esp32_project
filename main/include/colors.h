#ifndef COLORS_H
#define COLORS_H
  // @const Colors
#pragma once
#include "WS2812B.h"

static const WS2812B_led_color_t WHITE   = {.color = 0xFFFFFFFF};
static const WS2812B_led_color_t RED     = {.color = 0xFFFF0000};
static const WS2812B_led_color_t GREEN   = {.color = 0xFF00FF00};
static const WS2812B_led_color_t BLUE    = {.color = 0xFF0000FF};
static const WS2812B_led_color_t YELLOW  = {.color = 0xFFFFFF00};
static const WS2812B_led_color_t CYAN    = {.color = 0xFF00FFFF};
static const WS2812B_led_color_t MAGENTA = {.color = 0xFFFF00FF};
#endif