/**
 * (c) 2021 Yoichi Tanibayashi
 */
#include "Button.h"

/**
 *
 */
Button::Button(uint8_t pin, String name)
{
  this->_pin               = pin;
  this->_name              = name;

  this->_value             = Button::OFF;
  this->_prev_value        = Button::OFF;
  this->_press_start       = 0;
  this->_first_press_start = 0;
  this->_count             = 0;
  this->_click_count	   = 0;
  this->_long_pressed      = false;
  this->_repeated          = false;

  this->_is_enabled        = true;

  pinMode(this->_pin, INPUT_PULLUP);
} // Button::Button()

/**
 * return:
 *	true	changed
 *	false	to be ignored
 */
boolean Button::get()
{
  unsigned long cur_msec = millis();
  boolean 	ret = false;
  
  if ( ! this->_is_enabled ) {
    return false;
  }

  // is Enabled
  
  this->_prev_value = this->_value;
  this->_value = digitalRead(this->_pin);

  this->_click_count = 0;
  if ( this->_count > 0 ){
    if ( cur_msec - this->_first_press_start > CLICK_MSEC ) {
      // click count is detected
      this->_click_count = this->_count;
      this->_count = 0;
      ret = true;
    }
  }

  if ( this->_value == Button::OFF ) {
    // Released button then refresh some flags and do nothing any more
    this->_press_start = 0;
    this->_long_pressed = false;
    this->_repeated = false;

    if ( this->_prev_value == Button::ON) {
      // Released now !
      return true;
    }
    return ret;
  }

  // this->_value == Button::ON
  if ( this->_prev_value == Button::OFF ) {
    // Pushed now !
    this->_press_start = cur_msec;
    this->_count++;
    if ( this->_count == 1 ) {
      this->_first_press_start = cur_msec;
    }
    return true;
  }

  // check long pressed
  if ( ! this->_long_pressed ) {
    if ( cur_msec - this->_press_start > LONG_PRESS_MSEC ) {
      this->_long_pressed = true;
      this->_press_start = cur_msec;
      return true;
    } else {
      return ret;
    }
  }

  // long pressed .. check repeat
  if ( cur_msec - this->_press_start > REPEAT_MSEC ) {
    this->_repeated = true;
    this->_press_start = cur_msec;
    return true;
  }

  return ret;
} // Button::get()

void Button::enable()
{
  this->_is_enabled = true;
}
void Button::disable()
{
  this->_is_enabled = false;
}
boolean Button::is_enabled()
{
  return this->_is_enabled;
}

String Button::get_name()
{
  return this->_name;
}
boolean Button::get_value()
{
  return this->_value;
}
count_t Button::get_count()
{
  return this->_count;
}
count_t Button::get_click_count()
{
  return this->_click_count;
}
boolean Button::is_long_pressed()
{
  return this->_long_pressed;
}
boolean Button::is_repeated()
{
  return this->_repeated;
}

void Button::print() {
  print(false);
}
void Button::print(boolean interrupt)
{
  String str = interrupt ? "!" : " ";
  
  str += "Btn[" + this->_name + "] ";
  str += this->_value ? "OFF "  : "ON  ";
  str += String(this->_count) + " ";
  str += String(this->_click_count) + " ";
  str += this->_long_pressed ? "Long " : "---- ";
  str += this->_repeated ? "Repeat "  : "------ ";

  Serial.println(str);
}
