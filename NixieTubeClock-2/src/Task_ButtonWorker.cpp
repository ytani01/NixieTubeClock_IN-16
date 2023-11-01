/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Task_ButtonWorker.h"

// static variable
QueueHandle_t Task_ButtonWorker::BtnQue = (QueueHandle_t)NULL;
std::map<std::string, bool> Task_ButtonWorker::BtnVal = {}; // to be deprecated
std::map<std::string, ButtonInfo_t> Task_ButtonWorker::BtnInfo = {};

/**
 *
 */
Task_ButtonWorker::Task_ButtonWorker(uint32_t stack_size,
                                     UBaseType_t priority,
                                     UBaseType_t core)
  : Task(__CLASS_NAME__, stack_size, priority, core)
{
  Task_ButtonWorker::BtnQue = xQueueCreate(Task_ButtonWorker::Q_SIZE,
                                           sizeof(ButtonInfo_t));
  if ( Task_ButtonWorker::BtnQue == NULL ) {
    log_e("create out_que: failed .. HALT");
    while (true) { // HALT
      delay(1);
    }
  }
  log_d("Task_ButtonWorker::BtnQue = 0x%X", Task_ButtonWorker::BtnQue);
} // Task_ButtonWorker::Task_ButtonWorker

/**
 *
 */
void Task_ButtonWorker::addBtn(String name, uint8_t pin) {
  log_d("%s:%d", name, pin);

  Button *btn = new Button(name, pin, this->intr_hdr);

  this->btn_ent.push_back(btn);
  Task_ButtonWorker::BtnVal[name.c_str()] = Button::OFF;

  strcpy(Task_ButtonWorker::BtnInfo[name.c_str()].name, name.c_str());
  Task_ButtonWorker::BtnInfo[name.c_str()].pin = pin;
  Task_ButtonWorker::BtnInfo[name.c_str()].intr_hdr = this->intr_hdr;

  log_d("btn_ent.size: %d, BtnVal.size: %d",
        this->btn_ent.size(), Task::ButtonWorker::BtnVal.size());
} // Task_ButtonWorker::addBtn();

/**
 *
 */
void Task_ButtonWorker::setup() {
  log_d("%s: this->btn_ent: %d",
        this->conf.name.c_str(), this->btn_ent.size());
} // Task_ButtonWorker::setup()

/**
 *
 */
void Task_ButtonWorker::loop() {
  for (auto btn: this->btn_ent) {
    if ( btn->get() ) {
      Task_ButtonWorker::BtnVal[btn->info.name] = btn->info.value;
      Task_ButtonWorker::BtnInfo[btn->info.name].value = btn->info.value;
      log_v("btn->info.name=%s, btn->info.value=%d",
            btn->info.name, btn->info.value);
      portBASE_TYPE ret = xQueueSend(Task_ButtonWorker::BtnQue,
                                     (void *)&(btn->info), 10);
      if ( ret == pdPASS ) {
        log_v("que < %s", btn->toString().c_str());
      } else {
        log_w("que X< %s: ret=%d", btn->toString().c_str(), ret);
      }
    } // if
  } // for(itr)

  delay(10);
} // Task_ButtonWorker::loop()

/**
 *
 */
void Task_ButtonWorker::enable() {
  log_v("%s", this->conf.name);
  for (auto btn: this->btn_ent) {
    btn->enable();
  }
} // Task_ButtonWorker::enable()

/**
 *
 */
void Task_ButtonWorker::disable() {
  log_v("%s", this->conf.name);
  for (auto btn: this->btn_ent) {
    btn->disable();
  }
} // Task_ButtonWorker::disable()

/**
 * ボタンイベントのキューからget
 */
portBASE_TYPE Task_ButtonWorker::get(ButtonInfo_t *btn_info,
                                     TickType_t timeout) {
  portBASE_TYPE ret = xQueueReceive(Task_ButtonWorker::BtnQue,
                                    (void *)btn_info, timeout);
  if ( ret == pdPASS ) {
    log_v("que > %s", Button::info2String(btn_info).c_str());
  } else {
    log_v("ret=0x%X, timout=%d", ret, timeout);
  }
  return ret;
} // Task_ButtonWorker::get()

/** to be deprecated
 *
 */
std::map<std::string, bool> Task_ButtonWorker::get_BtnVal() {
  return Task_ButtonWorker::BtnVal;
} // Task_ButtonWorker::get_BtnVal() {

/** [static]
 *
 */
void IRAM_ATTR Task_ButtonWorker::intr_hdr(void *btn_obj) {
  static unsigned long __prev_ms = 0;
  unsigned long __cur_ms = millis();

  // check debounce
  if ( __cur_ms - __prev_ms < Button::DEBOUNCE ) {
    return;
  }
  __prev_ms = __cur_ms;

  // update button status
  Button *btn = static_cast<Button *>(btn_obj);
  if ( ! btn->get() ) {
    return;
  }
  isr_log_v("btn->info.name=%s", btn->info.name);

  // BtnVal[]
  Task_ButtonWorker::BtnVal[btn->info.name] = btn->info.value;
  Task_ButtonWorker::BtnInfo[btn->info.name].value = btn->info.value;

  isr_log_v("btn->info.name=%s, btn->info.value=%d",
            btn->info.name, btn->info.value);

  // BtnInfo[]
  Task_ButtonWorker::BtnInfo[btn->info.name].value = btn->info.value;

  // send to queue
  static BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  portBASE_TYPE ret = xQueueSendFromISR(Task_ButtonWorker::BtnQue,
                                        (void *)&(btn->info),
                                        &xHigherPriorityTaskWoken);
  if ( ret == pdPASS) {
    isr_log_v("que < %s", btn->toString().c_str());
  } else {
    isr_log_e("send que failed: %s: ret=%d", btn->toString().c_str(), ret);
  }

  // XXX TBD
  if ( xHigherPriorityTaskWoken ) {
    isr_log_v("portYIELD_FROM_ISR()");
    portYIELD_FROM_ISR();
  }
} // Task_ButtonWorker::intr_hdr()
