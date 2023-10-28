/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _TASK_BUTTON_WORKER_H_
#define _TASK_BUTTON_WORKER_H_

#include <vector>
#include "Task.h"
#include "Button.h"

/**
 *
 */
class Task_ButtonWorker: public Task {
public:
  static const TickType_t DEF_RECV_QUE_TIMEOUT = 1000;

  static const uint32_t STACK_SIZE_DEF = 4 * 1024;
  static const UBaseType_t PRIORITY_DEF = 0;
  static const UBaseType_t CORE_DEF = APP_CPU_NUM;

  Task_ButtonWorker(QueueHandle_t btn_que,
                    uint32_t stack_size=STACK_SIZE_DEF,
                    UBaseType_t priority=PRIORITY_DEF,
                    UBaseType_t core=CORE_DEF);

  virtual void addBtn(String name, uint8_t pin);

  virtual void enable();
  virtual void disable();

  portBASE_TYPE get(ButtonInfo_t *btn_info,
                    TickType_t timeout=DEF_RECV_QUE_TIMEOUT);

protected:
  std::vector<Button *> btn_ent;

  virtual void setup();
  virtual void loop();

  static void intr_hdr(void *btn_obj);
}; // class Task_ButtonWorker
#endif // _TASK_BUTTON_WORKER_H_
