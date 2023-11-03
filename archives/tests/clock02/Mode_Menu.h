/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_MENU_H_
#define _MODE_MENU_H_

#include "Mode.h"
#include "OledMenu.h"

/**
 *
 */
class Mode_Menu: public Mode {
public:
  Mode_Menu(String name, CommonData_t *common_data,
           void (*cb)(String text)=NULL);

  virtual void setup();
  virtual bool enter(Mode_t prev_mode);
  //virtual bool exit();
  
  virtual Mode_t reBtn_cb(ButtonInfo_t *bi);
  virtual Mode_t obBtn_cb(ButtonInfo_t *bi);
  virtual void display(Display_t *disp);

protected:
  OledMenu *topMenu, *clockMenu, *thermometerMenu, *wifiMenu, *systemMenu;
  OledMenu *curMenu;
  void (*cb)(String text) = NULL;
}; // class Mode_Menu

#endif // _MODE_MENU_H_
