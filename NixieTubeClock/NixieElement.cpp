/**
 * (c) 2021 Yoichi Tanibayashi
 */
#include "NixieElement.h"

void NixieElement::setup(uint8_t pin) {
  this->set_pin(pin);

  this->set_brightness(0);
  this->_on = false;
}

void NixieElement::set_brightness(uint8_t brightness) {
  this->_brightness = brightness;
}
void NixieElement::inc_brightness() {
  if (this->_brightness < BRIGHTNESS_RESOLUTION) {
    this->_brightness++;
  }
}
void NixieElement::dec_brightness() {
  if (this->_brightness > 0) {
    this->_brightness--;
  }
}
unsigned int NixieElement::get_brightness() {
  return this->_brightness;
}

void NixieElement::set_pin(uint8_t pin) {
  this->_pin = pin;
}
uint8_t NixieElement::get_pin() {
  return this->_pin;
}

void NixieElement::on() {
  this->_on = true;
}
void NixieElement::off() {
  this->_on = false;
}
boolean NixieElement::is_on() {
  return this->_on;
}
