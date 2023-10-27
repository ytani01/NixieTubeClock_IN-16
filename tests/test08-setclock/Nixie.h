/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 *
 *----------------------------------------------------------------------------
 * [ Data structure ( not a class tree ) ]
 *
 *  NixieTubeArray
 *   |
 *   +- NixieTube num[NIXIE_NUM_N]
 *   |   |
 *   |   +- NixieElement element[NIXIE_NUM_DIGIT_N]
 *   |   |
 *   |   +- NixieEffect
 *   |
 *   +- NixieTube colon[NIXIE_COLON_N]
 *       |
 *       +- NixieElement element[NIXIE_COLON_DOT_N]
 *       |
 *       +- NixieEffect
 *----------------------------------------------------------------------------
 */
#ifndef NIXIE_H
#define NIXIE_H
#include <Arduino.h>
#include <esp32-hal-log.h>

typedef unsigned int  brightness_t;

constexpr brightness_t BRIGHTNESS_RESOLUTION = 16;  // 明るさの段階数 2^n
constexpr brightness_t BRIGHTNESS_MIN        = 4;   // 明るさ最小値

#define NIXIE_NUM_N          6  // 数字の桁数
#define NIXIE_NUM_DIGIT_N   10  // 一桁の数字のパターン数

#define NIXIE_COLON_N        2
#define NIXIE_COLON_L        0
#define NIXIE_COLON_R        1
#define NIXIE_COLON_DOT_N    1  // 現在は、上下同じピンなので、ドットは一つ
#define NIXIE_COLON_DOT_UP   0  // UPとDOWNは、同じ
#define NIXIE_COLON_DOT_DOWN 0  // UPとDOWNは、同じ

#define NIXIE_ELEMENT_N_MAX  (NIXIE_NUM_DIGIT_N > NIXIE_COLON_DOT_N ? NIXIE_NUM_DIGIT_N : NIXIE_COLON_DOT_N)

// 省略記法
#define Nta           this->_nta
#define NtaNum(i)     Nta->num[i]
#define NtaNumEl(i,j) NtaNum(i).element[j]
#define NtaCol(i)     Nta->colon[i]
#define NtaColEl(i,j) NtaCol(i).element[j]

#endif // NIXIE_H
