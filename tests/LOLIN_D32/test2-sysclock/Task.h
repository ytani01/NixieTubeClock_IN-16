/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
/**
```
// Example

#include "Task.h"

class task1: public Task {
};
```
 */
#ifndef _TASK_H_
#define _TASK_H_

#include <Arduino.h>
//#include <freertos/FreeRTOS.h>
//#include <freertos/task.h>
#include <esp32-hal-log.h>

static const unsigned long TASK_NAME_LEN = 64;

/**
 *
 */
typedef struct {
  char name[TASK_NAME_LEN + 1];
  TaskHandle_t handle;
  uint32_t stack_size;
  UBaseType_t priority;
  UBaseType_t core;
} TaskConf_t;

/**
 *
 */
class Task {
public:
  static const uint32_t STACK_SIZE_DEF = 4 * 1024; // bytes
  static const UBaseType_t PRIORITY_DEF = 2;
  static const UBaseType_t CORE_DEF = APP_CPU_NUM; // PRO_CPU_NUM|APP_CPU_NUM
  
  TaskConf_t conf;
  
  Task(String name="[NO_NAME_TASK]",
       uint32_t stack_size=STACK_SIZE_DEF,
       UBaseType_t priority=PRIORITY_DEF,
       UBaseType_t core=CORE_DEF);
  void start();
  bool is_active();

  void __task_main();

 protected:
  bool _active;

  virtual void setup();
  virtual void loop();
  
  static void call_task_main(void *this_instance);
}; // class Task
#endif // _TASK_H_
