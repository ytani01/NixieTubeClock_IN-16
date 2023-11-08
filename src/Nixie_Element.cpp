/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Nixie_Element.h"

void Nixie_Element::setup(uint8_t pin) {
  this->set_pin(pin);

  this->set_brightness(0);
  this->_on = false;
}

/**
 *
 */
brightness_t Nixie_Element::max_brightness() {
  return this->_max_brightness;
} // Nixie_Element::max_brightness()

/**
 *
 */
brightness_t Nixie_Element::set_max_brightness(brightness_t bri) {
  this->_max_brightness = bri;
  return this->_max_brightness;
} // Nixie_Element::set_max_brightness()

/**
 *
 */
brightness_t Nixie_Element::set_brightness_to_max() {
  this->_brightness = this->_max_brightness;
  return this->_brightness;
}

/**
 *
 */
brightness_t Nixie_Element::set_brightness(brightness_t brightness) {
  this->_brightness = brightness;
  return this->_brightness;
} // Nixie_Element::set_brightness()

/**
 *
 */
brightness_t Nixie_Element::inc_brightness() {
  if ( this->_brightness < this->_max_brightness ) {
    this->_brightness++;
  }
  return this->_brightness;
}
brightness_t Nixie_Element::dec_brightness() {
  if ( this->_brightness > 0 ) {
    this->_brightness--;
  }
  return this->_brightness;
}
unsigned int Nixie_Element::brightness() {
  return this->_brightness;
}

void Nixie_Element::set_pin(uint8_t pin) {
  this->_pin = pin;
}
uint8_t Nixie_Element::get_pin() {
  return this->_pin;
}

void Nixie_Element::on() {
  this->_on = true;
}
void Nixie_Element::off() {
  this->_on = false;
}
boolean Nixie_Element::is_on() {
  return this->_on;
}
