/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _TASK_BUTTON_WORKER_H_
#define _TASK_BUTTON_WORKER_H_

#include <vector>
#include <map>
#include "Task.h"
#include "Button.h"

/**
 *
 */
class Task_ButtonWorker: public Task {
public:
  static const TickType_t DEF_RECV_QUE_TIMEOUT = 1000;
  static const UBaseType_t Q_SIZE = 16;

  static const uint32_t STACK_SIZE = 4 * 1024;
  static const UBaseType_t PRIORITY = 8;
  static const UBaseType_t CORE = APP_CPU_NUM;

  static QueueHandle_t BtnQue; // static for interrupt function
  static std::map<std::string, ButtonInfo_t> BtnInfo;

  Task_ButtonWorker(uint32_t stack_size=STACK_SIZE,
                    UBaseType_t priority=PRIORITY,
                    UBaseType_t core=CORE);

  virtual void addBtn(String name, uint8_t pin);

  virtual void enable();
  virtual void disable();

  portBASE_TYPE get(ButtonInfo_t *btn_info,
                    TickType_t timeout=DEF_RECV_QUE_TIMEOUT);

  std::map<std::string, bool> get_BtnVal(); // to be deprecated

protected:
  std::vector<Button *> btn_ent;

  virtual void setup();
  virtual void loop();

  static std::map<std::string, bool> BtnVal; // to be deprecated
  static void intr_hdr(void *btn_obj);
}; // class Task_ButtonWorker
#endif // _TASK_BUTTON_WORKER_H_
