/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
/**

# has-a tree

  Task_ButtonWatcher
    +- Task_ButtonWorker
         +- Button

### Example1 (recommeded): Task_ButtonWatcher ###
```
#include "Task_ButtonWatcher.h"
:
static const std::map<const char *, uint8_t> PIN_BTN = {
  {"Btn0", 10},
  {"Btn1", 11},
  {"Btn2", 12},
};
:
Task_ButtonWatcher *taskBtnWatcher=NULL;
:
void cb(ButtonInfo *bi, std:map<std::string, ButtonInfo_t>& btn_info) {
  log_d("%s", Button::info2String(bi).c_str());
  :
  if ( String(bi->name) == "Btn0" ) {
    :
    if ( btn_info["Btn1"].value == Button::ON ) {
      :
    }
  }
  :
  if ( bi->click_count >= 1 ) {
    :
  }
  :
}
:
void setup() {
  :
  taskBtnWatcher = new Task_ButtonWatcher(cb);
  for (auto btn: PIN_BTN) {
    taskBtnWatcher->addBtn(String(btn.first), btn.second);
  }
  taskBtnWatcher->start();
  :
}
:
void loop() {
  :
  taskBtnWatcher->disable();
  // critial region
  taskBtnWatcher->enbale();
  :
}
```
 */

#ifndef _TASK_BUTTON_WATCHER_H_
#define _TASK_BUTTON_WATCHER_H_

#include <vector>
#include <map>
#include "Task.h"
#include "Button.h"
#include "Task_ButtonWorker.h"

/**
 *
 */
class Task_ButtonWatcher: public Task {
public:
  static const uint32_t STACK_SIZE = 4 * 1024;
  static const UBaseType_t PRIORITY = 0;
  static const UBaseType_t CORE = APP_CPU_NUM;

  std::map<std::string, ButtonInfo_t> btn_info;

  Task_ButtonWatcher(void (*cb)(ButtonInfo_t *bi,
                                std::map<std::string, ButtonInfo_t>& btn_info),
                     uint32_t stack_size=STACK_SIZE,
                     UBaseType_t priority=PRIORITY,
                     UBaseType_t core=CORE);

  virtual bool addBtn(String name, uint8_t pin);

  virtual void enable();
  virtual void disable();

protected:
  virtual void setup();
  virtual void loop();

  Task_ButtonWorker *worker;

  void (*_cb)(ButtonInfo_t *bi,
              std::map<std::string, ButtonInfo_t>& btn_info);
  static void def_cb(ButtonInfo_t *bi,
                     std::map<std::string, ButtonInfo_t>& btn_info);
}; // class Task_ButtonWatcher
#endif // _TASK_BUTTON_WATCHER_H_
