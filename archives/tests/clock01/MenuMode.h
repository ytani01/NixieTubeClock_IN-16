/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _MENU_MODE_H_
#define _MENU_MODE_H_

#include "ModeBase.h"
#include "OledMenu.h"

/**
 *
 */
class MenuMode: public ModeBase {
public:
  MenuMode(String name, CommonData_t *common_data,
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
}; // class MenuMode

#endif // _MENU_MODE_H_
