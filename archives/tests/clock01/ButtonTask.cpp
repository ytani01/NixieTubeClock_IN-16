/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "ButtonTask.h"

static QueueHandle_t outQue = (QueueHandle_t)NULL; // for static function

/**
 *
 */
ButtonTask::ButtonTask(String btn_name, uint8_t pin,
                                 uint32_t stack_size,
                                 UBaseType_t priority,
                                 UBaseType_t core):
  Task(btn_name + "Task", stack_size, priority, core) {

  this->btn_name = btn_name;
  this->pin = pin;

  this->btn = new Button(this->btn_name, this->pin, this->intr_hdr);

  if ( outQue == NULL ) {
    this->_out_que = xQueueCreate(ButtonTask::Q_SIZE,
                                  sizeof(ButtonInfo_t));
    if ( this->_out_que == NULL ) {
      log_e("create out_que: failed .. HALT");
      while (true) { // HALT
        delay(1);
      }
    }
    outQue = this->_out_que;
    log_i("new Que: %X", outQue);
  } else {
    this->_out_que = outQue;
    log_i("reuse Que: %X", outQue);
  }
} // ButtonTask::ButtonTask

/**
 *
 */
void ButtonTask::setup() {
  log_d("%s", this->btn_name.c_str());
} // ButtonTask::setup()

/**
 *
 */
void ButtonTask::loop() {
  if ( this->btn->get() ) {
    portBASE_TYPE ret = xQueueSend(outQue,
                                   (void *)&(this->btn->info), 10);
    if ( ret == pdPASS ) {
      log_d("que < %s", this->btn->toString().c_str());
    } else {
      log_w("que X< %s: ret=%d", this->btn->toString().c_str(), ret);
    }
  }
  delay(10);
} // ButtonTask::loop()

/** [static]
 *
 */
void IRAM_ATTR ButtonTask::intr_hdr(void *btn_obj) {
  // check debounce
  static unsigned long __prev_ms = 0;
  unsigned long __cur_ms = millis();
  if ( __cur_ms - __prev_ms < Button::DEBOUNCE ) {
    return;
  }
  __prev_ms = __cur_ms;

  // update button status
  Button *btn = static_cast<Button *>(btn_obj);
  if ( ! btn->get() ) {
    return;
  }
  isr_log_d("btn->info.name=%s", btn->info.name);

  // send to queue
  static BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  portBASE_TYPE ret = xQueueSendFromISR(outQue,
                                        (void *)&(btn->info),
                                        &xHigherPriorityTaskWoken);
  if ( ret == pdPASS) {
    isr_log_d("que < %s", btn->toString().c_str());
  } else {
    isr_log_e("send que failed: %s: ret=%d", btn->toString().c_str(), ret);
  }
  
  if ( xHigherPriorityTaskWoken ) {
    isr_log_d("portYIELD_FROM_ISR()");
    portYIELD_FROM_ISR();
  }
} // ButtonTask::intr_hdr()

/**
 *
 */
portBASE_TYPE ButtonTask::get(ButtonInfo_t *btn_info) {
  portBASE_TYPE ret = xQueuePeek(outQue, (void *)btn_info, 1000);
  if ( ret == pdPASS ) {
    if ( String(btn_info->name) == this->btn_name ) {
      ret = xQueueReceive(outQue, (void *)btn_info, 0);
      log_d("%s:que > %s", this->btn_name.c_str(),
            Button::info2String(btn_info).c_str());
    } else {
      log_d("%s:que >X %s", this->btn_name.c_str(), 
            Button::info2String(btn_info).c_str());
      ret = pdFALSE;
      delay(2);
    }
  }
  return ret;
} // ButtonTask::get()

/**
 * defulat callback
 */
static void _button_cb(ButtonInfo_t *btn_info) {
  log_i("%s", Button::info2String(btn_info).c_str());
} // _button_cb()

/**
 *
 */
ButtonWatcher::ButtonWatcher(String btn_name, uint8_t pin,
                                       void (*cb)(ButtonInfo_t *btn_info),
                                       uint32_t stack_size,
                                       UBaseType_t priority,
                                       UBaseType_t core):
  Task(btn_name + "Watcher", stack_size, priority, core) {

  this->_btn_name = btn_name;
  this->_pin = pin;
  this->_cb = cb;
  this->_stack_size = stack_size;
  this->_priority = priority;
  this->_core = core;

  if ( cb == NULL ) {
    this->_cb = _button_cb;
  }

  this->_btn_task=NULL;
} // ButtonWatcher::ButtonWatcher()

/**
 *
 */
ButtonInfo_t *ButtonWatcher::get_btn_info() {
  return &(this->_btn_task->btn->info);
} // ButtonWatcher::get_btn_info()

/**
 *
 */
void ButtonWatcher::setup() {
  this->_btn_task = new ButtonTask(this->_btn_name, this->_pin,
                                        this->_stack_size,
                                        this->_priority,
                                        this->_core);
  this->_btn_task->start();
} // ButtonWatcher::setup()

/**
 *
 */
void ButtonWatcher::loop() {
  if ( this->_btn_task == NULL ) {
    return;
  }
  
  ButtonInfo_t btn_info;
  portBASE_TYPE ret = this->_btn_task->get(&btn_info);
  if ( ret == pdPASS ) {
      (*(this->_cb))(&btn_info);
  }
} // ButtonWatcher::loop()
