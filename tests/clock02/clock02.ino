/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include <Ticker.h>

#include "common.h"
#include "Display.h"
#include "Nixie.h"
#include "NixieTubeArray.h"

#include "Mode_Main.h"
#include "Mode_Menu.h"
#include "Mode_Restart.h"

#include "Task_NixieTubeArray.h"
#include "Task_Button.h"
#include "Task_NetMgr.h"
#include "Task_Ntp.h"

#define MYNAME __FILE__

#define PIN_I2C_SDA         8
#define PIN_I2C_SCL         9

// ネット接続がなく、かつ、アイドル時間が経過したら、リセットする
const unsigned long IDEL_RESET = 5 * 60 * 1000; // ms
unsigned long idleStart = 0;

// Modes
#define curMode commonData.cur_mode

std::vector<Mode *> Mode;
Mode_Main *mainMode;
Mode_Menu *menuMode;
Mode_Restart *restartMode;

// common data
CommonData_t commonData;

// Nixie Tube
#define PIN_HV5812_CLK      7
#define PIN_HV5812_STOBE    5
#define PIN_HV5812_DATA     6
#define PIN_HV5812_BLANK    4

#define PIN_COLON_R_TOP     2
#define PIN_COLON_R_BOTTOM  2
#define PIN_COLON_L_TOP     1
#define PIN_COLON_L_BOTTOM  1

uint8_t nixiePins[NIXIE_NUM_N][NIXIE_NUM_DIGIT_N] =
  {
   { 9,  0,  6,  2,  3,  4,  5,  1,  7,  8},
   {15, 14, 18, 12, 11, 10, 19, 13, 17, 16},
   {35, 20, 38, 22, 23, 24, 39, 21, 37, 36},
   {29, 34, 26, 32, 31, 30, 25, 33, 27, 28},
   {55, 54, 58, 52, 51, 50, 59, 53, 57, 56},
   {49, 40, 46, 42, 43, 44, 45, 41, 47, 48}
  };

uint8_t colonPins[NIXIE_COLON_N][NIXIE_COLON_DOT_N] =
  {
   {PIN_COLON_R_TOP},
   {PIN_COLON_L_TOP}
  };

NixieTubeArray *nta = NULL;

Task_NixieTubeArray *TaskNixieTubeArray = NULL;

// OLED
Display_t *Disp;

// Buttons
constexpr uint8_t PIN_BTN_UP = 18;
constexpr uint8_t PIN_BTN_DOWN = 17;
constexpr uint8_t PIN_BTN_MODE = 14;

const String BTN_NAME_UP = "UpBtn";
Task_ButtonWatcher *TaskBtnWatcher_Up = NULL;
ButtonInfo_t btnInfo_Up;

const String BTN_NAME_DOWN = "DownBtn";
Task_ButtonWatcher *TaskBtnWatcher_Down = NULL;
ButtonInfo_t btnInfo_Down;

const String BTN_NAME_MODE = "ModeBtn";
Task_ButtonWatcher *TaskBtnWatcher_Mode = NULL;
ButtonInfo_t btnInfo_Mode;

// WiFi
const String AP_SSID_HDR = "iot";
Task_NetMgr *TaskNetMgr = NULL;
NetMgrInfo_t netMgrInfo;

// NTP
const String NTP_SVR[] = {"ntp.nict.jp", "pool.ntp.org", "time.google.com"};
Task_Ntp *TaskNtp = NULL;
Task_NtpInfo_t ntpInfo;

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
void ntp_cb(Task_NtpInfo_t *ntp_info) {
  log_i("sntp_stat=%s(%d)",
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
        Task_Ntp::get_time_str(), uxTaskPriorityGet(NULL));

  //  delay(TIMER_INTERVAL / 2);

  TickType_t tick2 = xTaskGetTickCount();
  TickType_t d_tick = tick2 - tick1;
  log_d("%d %d", tick1, tick2);
  log_d("[%s] timer test: end(d_tick=%d)",
        Task_Ntp::get_time_str(), d_tick);
} // timer1_cb()

/** callback
 *
 */
void btnCb_Up(ButtonInfo_t *btn_info) {
  idleStart = millis();

  log_i("%s", Button::info2String(btn_info).c_str());
  btnInfo_Up = *btn_info;

  if ( btn_info->long_pressed && btn_info->repeat_count == 0 ) {
    do_restart();
    return;
  }

  if ( btn_info->click_count >= 2 ) {
    log_i("force AP mode");
    TaskNetMgr->set_mode(NETMGR_MODE_AP_INIT);
    return;
  }

  Mode_t dst_mode = Mode[curMode]->btnCb_Up(btn_info);
  if ( dst_mode != MODE_N && dst_mode != curMode ) {
    change_mode(dst_mode);
  }
} // btnCb_Up()

/** callback
 *
 */
void btnCb_Down(ButtonInfo_t *btn_info) {
  idleStart = millis();

  log_i("%s", Button::info2String(btn_info).c_str());
  btnInfo_Down = *btn_info;

  if ( btn_info->long_pressed && btn_info->repeat_count == 0 ) {
    do_restart();
    return;
  }

  Mode_t dst_mode = Mode[curMode]->btnCb_Down(btn_info);
  if ( dst_mode != MODE_N && dst_mode != curMode ) {
    change_mode(dst_mode);
  }
} // btnCb_Down()

/** callback
 *
 */
void btnCb_Mode(ButtonInfo_t *btn_info) {
  idleStart = millis();

  log_i("%s", Button::info2String(btn_info).c_str());
  btnInfo_Mode = *btn_info;

  Mode_t dst_mode = Mode[curMode]->btnCb_Mode(btn_info);
  if ( dst_mode != MODE_N && dst_mode != curMode ) {
    change_mode(dst_mode);
  }
} // btnCb_Mode()

/** callback
 *
 */
/** callback
 *
 */
void menu_cb(String text) {
  log_i("text=%s", text.c_str());

  if ( text == "reboot" ) {
    do_restart();
  }

  if ( text == "clear_ssid" ) {
    TaskNetMgr->clear_ssid();
    TaskNetMgr->restart_wifi();
    change_mode(MODE_MAIN);
    return;
  }

  if ( text == "restart_wifi" ) {
    //    TaskNetMgr->restart_wifi();
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
  delay(500);
  Serial.println("Start");

  log_i("===== start: %s =====", MYNAME);
  log_i("portTICK_PERIOD_MS=%d", portTICK_PERIOD_MS);

  // create Nixie Tube object
  nta = new NixieTubeArray(PIN_HV5812_CLK,  PIN_HV5812_STOBE,
                           PIN_HV5812_DATA, PIN_HV5812_BLANK,
                           nixiePins, colonPins);

  // init commonData
  commonData.netmgr_info = &netMgrInfo;
  commonData.ntp_info = &ntpInfo;
  commonData.nta = nta;

  // init OLED
  Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
  Disp = new Display_t(DISPLAY_W, DISPLAY_H);
  Disp->DispBegin(0x3C);
  Disp->setRotation(0); // XXX
  Disp->clearDisplay();
  Disp->display();

  // Tasks
  delay(1000);

  unsigned long task_interval = 10;

  TaskNixieTubeArray = new Task_NixieTubeArray(nta);
  TaskNixieTubeArray->start();
  delay(100);

  TaskNetMgr = new Task_NetMgr("NetMgr", AP_SSID_HDR, &netMgrInfo);
  TaskNetMgr->start();
  delay(100); // NTPなどより先に実行することが重要(?)

  TaskNtp = new Task_Ntp((String *)NTP_SVR, &TaskNetMgr, ntp_cb);
  TaskNtp->start();
  delay(task_interval);

  TaskBtnWatcher_Up
    = new Task_ButtonWatcher(BTN_NAME_UP, PIN_BTN_UP, btnCb_Up);
  TaskBtnWatcher_Up->start();
  delay(task_interval);

  TaskBtnWatcher_Down
    = new Task_ButtonWatcher(BTN_NAME_DOWN, PIN_BTN_DOWN, btnCb_Down);
  TaskBtnWatcher_Down->start();
  delay(task_interval);

  TaskBtnWatcher_Mode
    = new Task_ButtonWatcher(BTN_NAME_MODE, PIN_BTN_MODE, btnCb_Mode);
  TaskBtnWatcher_Mode->start();
  delay(task_interval);

  // start timer1
  timer1.attach_ms(TIMER_INTERVAL, timer1_cb);
  log_i("start Timer: %.1f sec", TIMER_INTERVAL / 1000.0);

  // init Mode[]
  mainMode = new Mode_Main("Mode_Main", &commonData);
  Mode.push_back(mainMode);

  menuMode = new Mode_Menu("Mode_Menu", &commonData, &menu_cb);
  Mode.push_back(menuMode);

  restartMode = new Mode_Restart("Mode_Restart", &commonData);
  Mode.push_back(restartMode);

  for (int i=0; i < Mode.size(); i++) {
    log_i("%d:%s", i, Mode[i]->get_name().c_str());
    Mode[i]->setup();
  }
  change_mode(MODE_MAIN);

  nta->display(0);

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
      TaskNetMgr->restart_wifi();
    }
    
    Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, WHITE);
    Disp->setTextColor(BLACK, WHITE);

    Disp->setFont(NULL);
    Disp->setTextSize(1);
    Disp->setCursor(0, 1);
    Disp->setTextWrap(true);
    Disp->printf("%s", commonData.msg.c_str());
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
  
  //nta->display(cur_ms);
  Disp->display();
  delay(1);
} // loop()
