/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 *
### Example1 (recommeded): ButtonWatcher ###
```
#include "ButtonTask.h"
:
ButtonWatcher *btnWatcher=NULL;
:
void cb(ButtonInfo *btn_info) {
  log_i("%s", Button::info2String(btn_info).c_str());
  :
}
:
btnWatcher = new ButtonWatcher("Button name", pin, cb);
btnWatcher.start();
:
```

### Example2: ButtonTask ###
```
#include "ButtonTask.h"
:
NtpTask *btnTask = NULL;
:
btnTask = new ButtonTask("Button name", pin);
btnTask->start();
:
whie (true) {
  ButtonInfo btn_info;
  if ( btnTask == NULL ) { // other task
    continue;
  }

  portBASE_TYPE ret = btnTask->get(&btn_info);
  if ( ret == pdPASS ) {
    log_i("%s", Button::info2String(&btn_info).c_str();
    :
  }
}
:
```
 */
#ifndef _BUTTON_TASK_H_
#define _BUTTON_TASK_H_

#include "Task.h"
#include "Button.h"

/**
 *
 */
class ButtonTask: public Task {
public:
  static const UBaseType_t Q_SIZE = 16;
  static const uint32_t STACK_SIZE_DEF = 4 * 1024;
  static const UBaseType_t PRIORITY_DEF = 0;
  static const UBaseType_t CORE_DEF = APP_CPU_NUM;

  String btn_name;
  uint8_t pin;
  Button *btn = NULL;

  ButtonTask(String btn_name, uint8_t pin,
                  uint32_t stack_size=STACK_SIZE_DEF,
                  UBaseType_t priority=PRIORITY_DEF,
                  UBaseType_t core=CORE_DEF);

  portBASE_TYPE get(ButtonInfo_t *btn_info);

protected:
  virtual void setup();
  virtual void loop();

  QueueHandle_t _out_que;

  static void intr_hdr(void *btn_obj);
}; // class ButtonTask

/**
 *
 */
class ButtonWatcher: public Task {
public:
  static const uint32_t STACK_SIZE_DEF = 4 * 1024;
  static const UBaseType_t PRIORITY_DEF = 0;
  static const UBaseType_t CORE_DEF = APP_CPU_NUM;

  ButtonWatcher(String btn_name,
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
  ButtonTask *_btn_task;
  void (*_cb)(ButtonInfo_t *btn_info);

  uint32_t _stack_size;
  UBaseType_t _priority;
  UBaseType_t _core;
}; // class ButtonWatcher
#endif // _BUTTON_TASK_H_
