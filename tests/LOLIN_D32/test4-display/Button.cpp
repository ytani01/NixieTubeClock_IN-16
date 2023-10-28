/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Button.h"

/**
 *
 */
Button::Button(String name, uint8_t pin, void (*intr_hdr)(void *btn)) {
  if ( name.length() > BUTTON_NAME_SIZE ) {
    strcpy(this->info.name, name.substring(0, BUTTON_NAME_SIZE).c_str());
  } else {
    strcpy(this->info.name, name.c_str());
  }
  this->info.pin = pin;
  this->info.intr_hdr = intr_hdr;

  this->info.value = Button::OFF;
  this->info.prev_value = Button::OFF;
  this->info.press_start = 0;
  this->info.first_press_start = 0;
  this->info.push_count = 0;
  this->info.click_count = 0;
  this->info.long_pressed = false;
  this->info.repeat_count = 0;

  this->info.active = false;

  pinMode(this->info.pin, INPUT_PULLUP);

  if ( intr_hdr != NULL ) {
    this->info.intr_pin = digitalPinToInterrupt(this->info.pin);
    log_d("%s: intr_pin=%d", this->info.name, this->info.intr_pin);
    this->enable();
  }
} // Button::Button()

/**
 * return:
 *	true	changed
 *	false	to be ignored
 */
boolean Button::get() {
  unsigned long cur_msec = millis();
  boolean 	ret = false;

  if ( ! this->info.active ) {
    return false;
  }

  // Active
  this->info.prev_value = this->info.value;
  this->info.value = digitalRead(this->info.pin);
  this->info.click_count = 0;

  if ( this->info.value == Button::OFF ) {
    if ( this->info.push_count > 0 ){
      if ( cur_msec - this->info.first_press_start > CLICK_MSEC ) {
        // click count is detected
        this->info.click_count = this->info.push_count;
        this->info.push_count = 0;
        // log_d("[%s] click_count=%d", this->info.name, this->info.click_count);
        ret = true;
      }
    }

    // Released button then refresh some flags and do nothing any more
    this->info.press_start = 0;
    this->info.long_pressed = false;
    this->info.repeat_count = 0;

    if ( this->info.prev_value == Button::ON) {
      // Released now !
      // log_d("[%s] released", this->info.name);
      return true;
    }
    // if ( ret ) log_d("[%s] ret=%d", this->info.name, ret);
    return ret;
  }

  // this->info.value == Button::ON
  if ( this->info.prev_value == Button::OFF ) {
    // Pushed now !
    this->info.press_start = cur_msec;
    this->info.push_count++;
    if ( this->info.push_count == 1 ) {
      this->info.first_press_start = cur_msec;
    }
    // log_d("[%s] pushed", this->info.name);
    return true;
  }

  // check long pressed
  if ( ! this->info.long_pressed ) {
    if ( cur_msec - this->info.press_start > LONG_PRESS_MSEC ) {
      this->info.long_pressed = true;
      this->info.press_start = cur_msec;
      // log_d("[%s] long", this->info.name);
      return true;
    } else {
      // if ( ret ) log_d("[%s] ret=%d", this->info.name, ret);
      return ret;
    }
  }

  // long pressed .. check repeat
  if ( cur_msec - this->info.press_start > REPEAT_MSEC ) {
    this->info.repeat_count++;
    this->info.press_start = cur_msec;
    // log_d("[%s] repeat", this->info.name);
    return true;
  }

  // if ( ret ) log_d("[%s] ret=%d", this->info.name, ret);
  return ret;
} // Button::get()

/**
 *
 */
void Button::enable() {
  this->info.active = true;
  attachInterruptArg(this->info.intr_pin, this->info.intr_hdr, this, CHANGE);
} // Button::enable()

/**
 *
 */
void Button::disable() {
  detachInterrupt(this->info.intr_pin);
  this->info.active = false;
} // Button::disable()

/**
 *
 */
boolean Button::is_active() {
  return this->info.active;
} // Button::is_active()

/**
 *
 */
String Button::get_name() {
  return String(this->info.name);
} // Button::get_name()

/**
 *
 */
boolean Button::get_value() {
  return this->info.value;
} // Button::get_value()

/**
 *
 */
ButtonCount_t Button::get_push_count() {
  return this->info.push_count;
} // Button::get_push_count()

/**
 *
 */
ButtonCount_t Button::get_click_count() {
  return this->info.click_count;
} // Button::get_click_count()

/**
 *
 */
boolean Button::is_long_pressed() {
  return this->info.long_pressed;
} // Button::is_long_pressed()

/**
 *
 */
ButtonCount_t Button::get_repeat_count() {
  return this->info.repeat_count;
} // Button::get_repeat_count()

/** [static]
 *
 */
char* Button::info2str(ButtonInfo_t *info, bool interrupted) {
  static char buf[128];

  String intrString = interrupted ? "!" : "*";
  String valueString = info->value ? "H(OFF)" : "L(ON )";
  String longPressedString = info->long_pressed ? "L" : "-";
  
  sprintf(buf, "%sBTN[%s:%d] %s P:%d C:%d %s R:%d",
          intrString.c_str(),
          info->name, info->pin, valueString.c_str(),
          info->push_count, info->click_count,
          longPressedString.c_str(), info->repeat_count);

  return buf;
} // Button::info2String()

/** [static]
 *
 */
String Button::info2String(ButtonInfo_t *info, bool interrupted) {
  return Button::info2str(info, interrupted);
} // Button::info2String()

/**
 *
 */
String Button::toString(bool interrupted) {
  return Button::info2String(&(this->info), interrupted);
} // Button::toString()
