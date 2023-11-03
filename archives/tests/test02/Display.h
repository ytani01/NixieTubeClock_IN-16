/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

typedef Adafruit_SSD1306 Display_t;
#define DispBegin(addr) begin(SSD1306_SWITCHCAPVCC, addr)

static constexpr uint8_t DISPLAY_I2C_ADDR = 0x3C;

static constexpr uint16_t DISPLAY_W = 128;
//static constexpr uint16_t DISPLAY_H = 64;
static constexpr uint16_t DISPLAY_H = 32;
static constexpr uint16_t DISPLAY_CH_W = 6;
static constexpr uint16_t DISPLAY_CH_H = 8;

#endif // _DISPLAY_H_
