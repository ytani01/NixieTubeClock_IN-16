/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 *
### Example1 (recommeded): Task_ButtonWatcher ###
```
#include "Task_ButtonWatcher.h"
:
static const uint8_t PIN_BTN0 = 10;
static const uint8_t PIN_BTN1 = 11;
:
Task_ButtonWatcher *taskBtnWatcher=NULL;
:
void cb(ButtonInfo *btn_info) {
  log_d("%s", Button::info2String(btn_info).c_str());
  :
}
:
void setup() {
  :
  taskBtnWatcher = new Task_ButtonWatcher(cb);
  taskBtnWatcher->addBtn("Button0", PIN_BTN0);
  taskBtnWatcher->addBtn("Button1", PIN_BTN1);
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
#include "Task.h"
#include "Button.h"
#include "Task_ButtonWorker.h"

/**
 *
 */
class Task_ButtonWatcher: public Task {
public:
  static const uint32_t STACK_SIZE_DEF = 4 * 1024;
  static const UBaseType_t PRIORITY_DEF = 0;
  static const UBaseType_t CORE_DEF = APP_CPU_NUM;

  static const UBaseType_t Q_SIZE = 16;

  Task_ButtonWatcher(void (*cb)(ButtonInfo_t *btn_info)=NULL,
                     uint32_t stack_size=STACK_SIZE_DEF,
                     UBaseType_t priority=PRIORITY_DEF,
                     UBaseType_t core=CORE_DEF);

  virtual bool addBtn(String name, uint8_t pin);

  virtual void enable();
  virtual void disable();

protected:
  virtual void setup();
  virtual void loop();

  String _btn_name;
  uint8_t _pin;

  Task_ButtonWorker *worker;

  QueueHandle_t btn_que;

  void (*_cb)(ButtonInfo_t *btn_info);
}; // class Task_ButtonWatcher
#endif // _TASK_BUTTON_WATCHER_H_
