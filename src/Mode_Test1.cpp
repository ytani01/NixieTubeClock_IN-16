/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Mode_Test1.h"

/**
 *
 */
Mode_Test1::Mode_Test1(): Mode() {
  this->val = 0;
} // Mode_Test1::Mode_Test1()

/**
 *
 */
void Mode_Test1::display() {
  char val_str[9];
  sprintf(val_str, "%d%d:%d%d:%d%d",
          this->val,this->val,this->val,this->val,this->val,this->val);
  log_i("val_str = %s", val_str);

  Nxa->set_string(val_str);

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setCursor(0, 0);
  Disp->setTextSize(2);
  Disp->setTextWrap(true);
  Disp->printf(" %s", val_str);
  Disp->display();
} // Mode_Test1::display()


/** virtual
 *
 */
void Mode_Test1::enter() {
  log_i("enter mode: %s", this->name.c_str());

  this->display();
  
} // Mode_Test1::enter()

/** virtual
 *
 */
void Mode_Test1::exit() {
  log_i("");
  Nxa->end_all_effect();
  this->val = 0;
} // Mode_Test1::exit()

/** virtual
 *
 */
void Mode_Test1::loop() {
  delayOrChangeMode(this->LOOP_DELAY_MS);
} // Mode_Test1::loop()

/**
 *
 */
void Mode_Test1::cbBtn(const ButtonInfo_t& bi,
                     const std::map<std::string, ButtonInfo_t>& btn_info) {
  log_d("%s", Button::info2String(&bi).c_str());

  if ( bi.value == Button::ON ) {
    if ( ! bi.long_pressed ) {
      val = (val + 1) % 10;

      this->display();
      return;
    } // if ( ! long_press )

    Mode::set("Mode_Clock");
    return;
  } // if (ON)

} // Mode_Test1::cbBtn()
