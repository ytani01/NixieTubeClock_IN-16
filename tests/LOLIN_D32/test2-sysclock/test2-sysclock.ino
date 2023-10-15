/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
//#include <Arduino.h>
#include <esp32-hal-log.h>
#include "Task_ButtonWatcher.h"
#include "SysClock.h"

static const uint8_t PIN_BTN0 = 14;
static const uint8_t PIN_BTN1 = 17;
static const uint8_t PIN_BTN2 = 18;

static const String BTN_NAME0 = "Btn0";
static const String BTN_NAME1 = "Btn1";
static const String BTN_NAME2 = "Btn2";

Task_ButtonWatcher *taskBtnWatcher = NULL;

/**
 *
 */
void cbBtn(ButtonInfo_t *bi) {
  if ( bi->click_count >= 1 ) {
    log_d("%s", Button::info2String(bi).c_str());
    return;
  }
  if ( bi->value == Button::ON ) {
    log_d("%s", Button::info2String(bi).c_str());

    struct tm *tm = SysClock::now_tm();
    log_d("tm: %s", tm2str(tm));

    if (String(bi->name) == "Btn1") {
      tm->tm_mon++;
      log_d(">tm: %s", tm2str(tm));
    }

    if (String(bi->name) == "Btn2") {
      tm->tm_mon--;
      log_d(">tm: %s", tm2str(tm));
    }

    SysClock::set(tm);
    return;
  }
} // cbBtn()

/**
 *
 */
void setup() {
  //Serial.begin(115200);
  delay(2000); // 2000 ms 以上が安全
  //Serial.println("start");
  log_i("===== start %s =====", get_mac_addr_String().c_str());

  log_i("now_str=%s", SysClock::now_str());
  SysClock::set(2023, 10, 15, 22, 46, 0);
  log_i("now_str=%s", SysClock::now_str());

  taskBtnWatcher = new Task_ButtonWatcher(cbBtn);
  taskBtnWatcher->addBtn(BTN_NAME0, PIN_BTN0);
  taskBtnWatcher->addBtn(BTN_NAME1, PIN_BTN1);
  taskBtnWatcher->addBtn(BTN_NAME2, PIN_BTN2);
  taskBtnWatcher->start();
} // setup()

/**
 *
 */
void loop() {
  log_i("%s, enable", SysClock::now_str());
  taskBtnWatcher->enable();
  delay(5000);

  log_i("%s, disable", SysClock::now_str());
  taskBtnWatcher->disable();
  delay(2000);
} // loop()
