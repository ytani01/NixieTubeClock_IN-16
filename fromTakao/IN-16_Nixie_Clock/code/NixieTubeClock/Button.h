/**
 * (c) 2021 Yoichi Tanibayashi
 *
 *  ==========================================================================
 *  example1
 *  --------------------------------------------------------------------------
 *  :
 *  #include "Button.h"
 *  :
 *  Button btn;
 *  :
 *  void btn_intr_hdr() {
 *    if ( btn.get() ) {
 *      app_btn_intr_hdr(&btn);
 *    }
 *  }
 *  
 *  void setup() {
 *    btn.setup(pin, "button_name");
 *    :
 *    attachInterrupt(digitalPinToInterrupt(pin), btn_intr_hdr, CHANGE);
 *  }
 *  
 *  void loop() {
 *    :
 *    if ( btn.get() ) {
 *      app_btn_loop_hdr(&btn);
 *    }
 *    :
 *  }
 *  ==========================================================================
 */
#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

typedef uint8_t		button_event_t;
typedef uint8_t		count_t;

class Button {
 public:
  static const unsigned long    ON                      = LOW;
  static const unsigned long    OFF                     = HIGH;

  static const unsigned long	DEBOUNCE        	=   10;
  static const unsigned long	LONG_PRESS_MSEC 	= 1000;
  static const unsigned long	REPEAT_MSEC     	=  300;
  static const unsigned long	CLICK_MSEC		=  500;

  static const button_event_t	EVENT_NONE		= 0x00;
  static const button_event_t	EVENT_LONG_PRESSED 	= 0x01;
  static const button_event_t	EVENT_REPEAT		= 0x03;
  static const button_event_t	EVENT_INTRRUPT		= 0x10;
  static const button_event_t	EVENT_PESSED		= 0x30;
  static const button_event_t	EVENT_RELEASED		= 0x50;
  static const button_event_t	EVENT_CLICK		= 0x54;

  Button(uint8_t pin, String name);

  boolean	get();

  void		enable();
  void		disable();
  boolean	is_enabled();

  String  	get_name();
  boolean 	get_value();
  count_t    	get_count();
  count_t	get_click_count();
  boolean	is_long_pressed();
  boolean	is_repeated();

  void    	print();
  void    	print(boolean interrupt);

 private:
  String	_name;
  boolean	_is_enabled = false;
  uint8_t	_pin;
  boolean	_value;      // OFF/ON
  boolean	_prev_value; // OFF/ON
  unsigned long	_first_press_start; // msec
  unsigned long	_press_start;       // msec
  count_t	_count;
  count_t	_click_count;
  boolean	_long_pressed;
  boolean	_repeated;
};

#endif
