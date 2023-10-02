/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include <Ticker.h>

#include "common.h"
#include "Display.h"

#include "MainMode.h"
#include "MenuMode.h"
#include "RestartMode.h"

#include "ButtonTask.h"
#include "NetMgrTask.h"
#include "NtpTask.h"

#include "ConfFps.h"

#define MYNAME __FILE__

ConfFps *confFps;
static bool dispFps = true;

// ネット接続がなく、かつ、アイドル時間が経過したら、リセットする
const unsigned long IDEL_RESET = 5 * 60 * 1000; // ms
unsigned long idleStart = 0;

// Modes
#define curMode commonData.cur_mode

std::vector<ModeBase *> Mode;
MainMode *mainMode;
MenuMode *menuMode;
RestartMode *restartMode;

// common data
CommonData_t commonData;

// OLED
Display_t *Disp;

// Buttons
constexpr uint8_t PIN_BTN_RE = 14;
const String RE_BTN_NAME = "RotaryEncoderBtn";
ButtonWatcher *reBtnWatcher = NULL;
ButtonInfo_t reBtnInfo;

constexpr uint8_t PIN_BTN_ONBOARD = 17;
const String ONBOARD_BTN_NAME = "OnBoardBtn";
ButtonWatcher *obBtnWatcher = NULL;
ButtonInfo_t obBtnInfo;

// WiFi
const String AP_SSID_HDR = "iot";
NetMgrTask *netMgrTask = NULL;
NetMgrInfo_t netMgrInfo;

// NTP
const String NTP_SVR[] = {"ntp.nict.jp", "pool.ntp.org", "time.google.com"};
NtpTask *ntpTask = NULL;
NtpTaskInfo_t ntpInfo;

// Timer
constexpr TickType_t TIMER_INTERVAL = 2 * 1000; // tick == ms (?)
Ticker timer1;

// Menu
OledMenu *menuTop, *menuSub;

/**
 *
 */
bool change_mode(Mode_t mode) {
  if ( ! Mode[curMode]->exit() ) {
    log_e("%s:exit(): failed", MODE_T_STR[curMode]);
    return false;
  }

  if ( ! Mode[mode]->enter(curMode) ) {
    log_e("%s:enter(): failed", MODE_T_STR[mode]);
  }

  Mode_t prev_mode = curMode;
  curMode = mode;
  log_i("mode: %s ==> %s",
        MODE_T_STR[prev_mode], MODE_T_STR[curMode]);
  return true;
} // change_mode()

/**
 *
 */
void do_restart() {
  log_w("restart..");
  
  commonData.msg = " Reboot.. ";
  delay(1000);
  
  //ESP.restart();
  ESP.deepSleep(100);
  delay(500);
} // do_restart()

/**
 *
 */
void ntp_cb(NtpTaskInfo_t *ntp_info) {
  log_d("sntp_stat=%s(%d)",
        SNTP_SYNC_STATUS_STR[ntp_info->sntp_stat], ntp_info->sntp_stat);
  
  ntpInfo = *ntp_info;
} // ntp_cb()

/**
 * 【注意・重要】
 * コールバック実行中に、次のタイマー時間になると、
 * 次のコールバックが待たされ、あふれるとパニックする。
 */
void timer1_cb() {
  TickType_t tick1 = xTaskGetTickCount();
  log_d("[%s] timer test: start(priority=%d)",
        NtpTask::get_time_str(), uxTaskPriorityGet(NULL));

  //  delay(TIMER_INTERVAL / 2);

  TickType_t tick2 = xTaskGetTickCount();
  TickType_t d_tick = tick2 - tick1;
  log_d("%d %d", tick1, tick2);
  log_d("[%s] timer test: end(d_tick=%d)",
        NtpTask::get_time_str(), d_tick);
} // timer1_cb()

/** callback
 *
 */
void reBtn_cb(ButtonInfo_t *btn_info) {
  idleStart = millis();

  log_i("%s", Button::info2String(btn_info).c_str());
  reBtnInfo = *btn_info;

  Mode_t dst_mode = Mode[curMode]->reBtn_cb(btn_info);
  if ( dst_mode != MODE_N && dst_mode != curMode ) {
    change_mode(dst_mode);
  }
} // reBtn_cb()

/** callback
 *
 */
void obBtn_cb(ButtonInfo_t *btn_info) {
  idleStart = millis();

  log_i("%s", Button::info2String(btn_info).c_str());
  obBtnInfo = *btn_info;

  if ( btn_info->long_pressed && btn_info->repeat_count == 0 ) {
    do_restart();
    return;
  }

  Mode_t dst_mode = Mode[curMode]->obBtn_cb(btn_info);
  if ( dst_mode != MODE_N && dst_mode != curMode ) {
    change_mode(dst_mode);
  }
} // obBtn_cb()

/** callback
 *
 */
/** callback
 *
 */
void menu_cb(String text) {
  log_i("text=%s", text.c_str());

  if ( text == "disp_fps" ) {
    if ( dispFps ) {
      dispFps = false;
    } else {
      dispFps = true;
    }
    log_i("dispFps=%d,%d,%d", dispFps, true, false);
    confFps->disp_fps = dispFps;
    confFps->save();
    change_mode(MODE_MAIN);
    return;
  }

  if ( text == "reboot" ) {
    do_restart();
  }

  if ( text == "clear_ssid" ) {
    netMgrTask->clear_ssid();
    netMgrTask->restart_wifi();
    change_mode(MODE_MAIN);
    return;
  }

  if ( text == "restart_wifi" ) {
    //    netMgrTask->restart_wifi();
    commonData.msg = "restart_wifi";
    change_mode(MODE_MAIN);
    return;
  }
} // menu_cb()

/**
 *
 */
void setup() {
  Serial.begin(115200);
  do {
    delay(500);
    Serial.print('.');
  } while (!Serial);  // Serial Init Wait
  Serial.println();
  Serial.println("===== start: " + String(MYNAME) + " =====");
  log_i("portTICK_PERIOD_MS=%d", portTICK_PERIOD_MS);

  confFps = new ConfFps();
  confFps->load();
  dispFps = confFps->disp_fps;
  log_i("dispFps=%d", dispFps);

  // init commonData
  commonData.netmgr_info = &netMgrInfo;
  commonData.ntp_info = &ntpInfo;

  // init Display
  Wire.setPins(9, 8);
  Disp = new Display_t(DISPLAY_W, DISPLAY_H);
  Disp->DispBegin(0x3C);
  Disp->setRotation(0); // XXX
  Disp->clearDisplay();
  Disp->display();

  // Tasks
  unsigned long task_interval = 10;

  netMgrTask = new NetMgrTask("NetMgr", AP_SSID_HDR, &netMgrInfo);
  netMgrTask->start();
  delay(1000); // NTPなどより先に実行することが重要(?)

  ntpTask = new NtpTask((String *)NTP_SVR, &netMgrTask, ntp_cb);
  ntpTask->start();
  delay(task_interval);

  reBtnWatcher = new ButtonWatcher(RE_BTN_NAME, PIN_BTN_RE, reBtn_cb);
  reBtnWatcher->start();
  delay(task_interval);

  obBtnWatcher = new ButtonWatcher(ONBOARD_BTN_NAME, PIN_BTN_ONBOARD, obBtn_cb);
  obBtnWatcher->start();
  delay(task_interval);

  // start timer1
  timer1.attach_ms(TIMER_INTERVAL, timer1_cb);
  log_i("start Timer: %.1f sec", TIMER_INTERVAL / 1000.0);

  // init Mode[]
  mainMode = new MainMode("MainMode", &commonData);
  Mode.push_back(mainMode);

  menuMode = new MenuMode("MenuMode", &commonData, &menu_cb);
  Mode.push_back(menuMode);

  restartMode = new RestartMode("RestartMode", &commonData);
  Mode.push_back(restartMode);

  for (int i=0; i < Mode.size(); i++) {
    log_i("%d:%s", i, Mode[i]->get_name().c_str());
    Mode[i]->setup();
  }
  change_mode(MODE_MAIN);

  idleStart = millis();
} // setup()

/**
 *
 */
void loop() {
  static unsigned long prev_ms = millis();
  unsigned long cur_ms = millis();
  int d_ms = cur_ms - prev_ms;
  prev_ms = cur_ms;

  /*
   * loop
   */
  Mode[curMode]->loop(cur_ms);

  /*
   * display
   */
  Disp->clearDisplay();
  
  if ( commonData.msg.length() > 0 ) {
    log_i("msg:\"%s\"", commonData.msg.c_str());

    if ( commonData.msg == "restart_wifi" ) {
      netMgrTask->restart_wifi();
    }
    
    Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, WHITE);
    Disp->setTextColor(BLACK, WHITE);

    Disp->setFont(NULL);
    Disp->setTextSize(1);
    Disp->setCursor(0, 10);
    Disp->setTextWrap(true);
    Disp->printf(" %s", commonData.msg.c_str());

    Disp->display();
    commonData.msg = "";
    delay(1000);

    change_mode(MODE_MAIN);
    return;
  }

  Mode[curMode]->display(Disp);

  // idle ms
  unsigned long idle_ms = millis() - idleStart;
  if ( idle_ms >= IDEL_RESET
       && commonData.netmgr_info->mode != NETMGR_MODE_WIFI_ON ) {
    log_i("idle_ms=%d .. reboot", idle_ms);
    //ESP.restart();
    ESP.deepSleep(100);
    delay(500);
  }
  
  // fps
  if ( dispFps ) {
    float fps = 0.0;
    static float min_fps = 99999.9;
    static unsigned long min_fps_ms = millis();
    if ( d_ms != 0 ) {
      fps = 1000.0 / (float)d_ms;
      if ( fps < min_fps ) {
        min_fps = fps;
        min_fps_ms = cur_ms;
      } else if ( cur_ms - min_fps_ms > 3000 ) {
        min_fps = fps;
      }
    }

    int w = 30;
    int h = 7;
    int x = DISPLAY_W - w;
    int y = DISPLAY_H - 6;
    Disp->setFont(&Picopixel);
    Disp->setTextSize(1);
    Disp->fillRect(x, y, w, h, BLACK);
    Disp->setCursor(x + 2, y + 5);
    Disp->printf("%4.1f FPS", min_fps);
    Disp->setFont(NULL);

    Disp->setCursor(DISPLAY_W - DISPLAY_CH_W * 6,
                    DISPLAY_H - DISPLAY_CH_H * 2);
    Disp->printf("%6d", idle_ms);
  } // if (dispFps);

  Disp->display();
  delay(1);
} // loop()
