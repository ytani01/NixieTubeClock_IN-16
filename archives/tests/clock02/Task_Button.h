/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 *
### Example1 (recommeded): Task_ButtonWatcher ###
```
#include "Task_Button.h"
:
Task_ButtonWatcher *taskBtnWatcher=NULL;
:
void cb(ButtonInfo *btn_info) {
  log_i("%s", Button::info2String(btn_info).c_str());
  :
}
:
taskBtnWatcher = new Task_ButtonWatcher("Button name", pin, cb);
taskBtnWatcher.start();
:
```

### Example2: Task_Button ###
```
#include "Task_Button.h"
:
NtpTask *taskBtn = NULL;
:
taskBtn = new Task_Button("Button name", pin);
taskBtn->start();
:
whie (true) {
  ButtonInfo btn_info;
  if ( taskBtn == NULL ) { // other task
    continue;
  }

  portBASE_TYPE ret = taskBtn->get(&btn_info);
  if ( ret == pdPASS ) {
    log_i("%s", Button::info2String(&btn_info).c_str();
    :
  }
}
:
```
 */
#ifndef _TASK_BUTTON_H_
#define _TASK_BUTTON_H_

#include "Task.h"
#include "Button.h"

/**
 *
 */
class Task_Button: public Task {
public:
  static const UBaseType_t Q_SIZE = 16;
  static const uint32_t STACK_SIZE_DEF = 4 * 1024;
  static const UBaseType_t PRIORITY_DEF = 0;
  static const UBaseType_t CORE_DEF = APP_CPU_NUM;

  String btn_name;
  uint8_t pin;
  Button *btn = NULL;

  Task_Button(String btn_name, uint8_t pin,
                  uint32_t stack_size=STACK_SIZE_DEF,
                  UBaseType_t priority=PRIORITY_DEF,
                  UBaseType_t core=CORE_DEF);

  portBASE_TYPE get(ButtonInfo_t *btn_info);

protected:
  virtual void setup();
  virtual void loop();

  QueueHandle_t _out_que;

  static void intr_hdr(void *btn_obj);
}; // class Task_Button

/**
 *
 */
class Task_ButtonWatcher: public Task {
public:
  static const uint32_t STACK_SIZE_DEF = 4 * 1024;
  static const UBaseType_t PRIORITY_DEF = 0;
  static const UBaseType_t CORE_DEF = APP_CPU_NUM;

  Task_ButtonWatcher(String btn_name,
                     uint8_t pin,
                     void (*cb)(ButtonInfo_t *btn_info)=NULL,
                     uint32_t stack_size=STACK_SIZE_DEF,
                     UBaseType_t priority=PRIORITY_DEF,
                     UBaseType_t core=CORE_DEF);

  ButtonInfo_t *get_btn_info();

protected:
  virtual void setup();
  virtual void loop();

  String _btn_name;
  uint8_t _pin;
  Task_Button *_btn_task;
  void (*_cb)(ButtonInfo_t *btn_info);

  uint32_t _stack_size;
  UBaseType_t _priority;
  UBaseType_t _core;
}; // class Task_ButtonWatcher
#endif // _TASK_BUTTON_H_
