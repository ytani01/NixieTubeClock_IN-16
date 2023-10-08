/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_MAIN_H_
#define _MODE_MAIN_H_

#include "commonlib.h"
#include "Mode.h"

/** constructor
 *
 */
class Mode_Main: public Mode {
 public:
  char mac_addr_str[13];

  Mode_Main(String name, CommonData_t *common_data);

  virtual Mode_t btnCb_Mode(ButtonInfo_t *bi);
  virtual Mode_t btnCb_Up(ButtonInfo_t *bi);
  virtual Mode_t btnCb_Down(ButtonInfo_t *bi);

  virtual void display(Display_t *disp);

 protected:
  void drawHum(Display_t *disp, int x, int y, float hum);
  void drawPres(Display_t *disp, int x, int y, float pres);
  void drawThi(Display_t *disp, int x, int y, float thi);

  void drawWiFi(Display_t *disp, int x, int y,
                NetMgrInfo_t *ni);
  void drawNtp(Display_t *disp, int x, int y,
               Task_NtpInfo_t *ntp_info,
               NetMgrInfo_t *netmgr_info);
  void drawDateTime(Display_t *disp, int x, int y, struct tm *ti);
}; // class Mode_Main

#endif // _MODE_MAIN_H_
