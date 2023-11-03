/**
 * Copyright (c) 2021 Yoichi Tanibayashi
 *
 *  ==========================================================================
 *  example1
 *  --------------------------------------------------------------------------
 *  :
 *  #include "Button.h"
 *  :
 *  Button *btn;
 *  :
 *  void IRAM_ATTR btn_intr_hdr(void *arg_btn) {
 *    Button *btn = static_cast<Button *>(arg_btn);
 *    static unsigned long __prev_ms = 0;
 *    unsigned long __cur_ms = millis();
 *    if ( __cur_ms - __prev_ms < Button::DEBOUNCE ) {
 *      return;
 *    }
 *    __prev_ms = __cur_ms;
 *    if ( ! btn->get() ) {
 *      return;
 *    }
 *    log_d("btn->info.name=%s", btn->info.name);
 *    // ここまで定形
 *
 *    app_btn_intr_hdr();
 *
 *  } // btn_intr_hdr()
 *  
 *  void setup() {
 *    btn = new Button("button_name", pin, btn_intr_hdr);
 *    :
 *  } // setup()
 *  
 *  void loop() {
 *    :
 *    if ( btn->get() ) {
 *      app_btn_loop_hdr(btn);
 *    }
 *    :
 *  } // loop()
 *  ==========================================================================
 */
#ifndef _BUTTON_H_
#define _BUTTON_H_
#include <Arduino.h>
#include <esp32-hal-log.h>

static const unsigned long BUTTON_NAME_SIZE = 16;

typedef uint8_t	ButtonCount_t;

/**
 *
 */
typedef struct {
  char name[BUTTON_NAME_SIZE + 1];
  uint8_t pin;
  bool active;
  bool value;
  bool prev_value;
  unsigned long first_press_start;
  unsigned long press_start;
  ButtonCount_t push_count;
  ButtonCount_t click_count;
  bool long_pressed;
  ButtonCount_t repeat_count;;
} ButtonInfo_t;

/**
 *
 */
class Button {
 public:
  static const unsigned long ON                 = LOW;
  static const unsigned long OFF                = HIGH;

  static const unsigned long DEBOUNCE        	=  100;
  static const unsigned long LONG_PRESS_MSEC 	= 1000;
  static const unsigned long REPEAT_MSEC     	=  300;
  static const unsigned long CLICK_MSEC		=  600;

  ButtonInfo_t info;

  Button(String name, uint8_t pin, void (*intr_hdr)(void *btn));

  bool get();

  void enable();
  void disable();
  bool is_active();

  String get_name();
  bool get_value();
  ButtonCount_t get_push_count();
  ButtonCount_t get_click_count();
  bool is_long_pressed();
  ButtonCount_t get_repeat_count();

  static String info2String(ButtonInfo_t *info, bool interrupted=false);
  String toString(bool interrupted=false);
};
#endif // _BUTTON_H_
