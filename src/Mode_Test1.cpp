/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Mode_Test1.h"

/**
 *
 */
Mode_Test1::Mode_Test1(): Mode() {
  this->tube = 0;
  this->num = 0;
} // Mode_Test1::Mode_Test1()

/**
 *
 */
void Mode_Test1::display() {
  char val[] = {'_', '_', '_', '_', '_', '_'};
  val[this->tube] = this->num + '0';

  char val_str[9];
  sprintf(val_str, "%c%c %c%c %c%c",
          val[0], val[1], val[2], val[3], val[4], val[5]);
  log_i("val_str = \"%s\"", val_str);

  Nxa->set_string(val_str);

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setCursor(0, 0);
  Disp->setTextSize(1);
  Disp->printf("  ----------------\n  ");
  Disp->setTextSize(2);
  Disp->setTextWrap(true);
  Disp->printf("%s\n", val_str);
  Disp->setTextSize(1);
  Disp->printf("  ----------------");
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

  if ( std::string(bi.name) == "Btn0" ) {
    if ( bi.value == Button::ON ) {
      if ( ! bi.long_pressed ) {
        this->tube = (this->tube + 1) % 6;
        log_i("tube = %d", this->tube);

        this->display();
        return;
      }

      Mode::set("Mode_Clock");
      return;
    }

    return;
  } // if (ON)

  if ( std::string(bi.name) == "Btn1" ) {
    if ( bi.value == Button::ON ) {
      this->num = (this->num + 1) % 10;
      log_i("num = %d", this->num);

      this->display();
      return;
    }

    return;
  } // if (ON)

  if ( std::string(bi.name) == "Btn2" ) {
    if ( bi.value == Button::ON ) {
      this->num = (this->num - 1 + 10) % 10;
      log_i("num = %d", this->num);

      this->display();
      return;
    }

    return;
  } // if (ON)

} // Mode_Test1::cbBtn()
