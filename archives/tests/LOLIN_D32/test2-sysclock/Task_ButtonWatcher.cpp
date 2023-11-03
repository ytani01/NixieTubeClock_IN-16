/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Task_ButtonWatcher.h"


/**
 * defulat callback
 */
static void _def_button_cb(ButtonInfo_t *btn_info) {
  log_d("%s", Button::info2String(btn_info).c_str());
} // _def_button_cb()

/**
 *
 */
Task_ButtonWatcher::Task_ButtonWatcher(void (*cb)(ButtonInfo_t *btn_info),
                                       uint32_t stack_size,
                                       UBaseType_t priority,
                                       UBaseType_t core)
  : Task("ButtonWatcher", stack_size, priority, core)
{
  this->_cb = cb;
  if ( cb == NULL ) {
    this->_cb = _def_button_cb;
  }

  this->btn_que = xQueueCreate(Task_ButtonWatcher::Q_SIZE,
                               sizeof(ButtonInfo_t));
  if ( this->btn_que == NULL ) {
    log_e("create out_que: failed .. HALT");
    while (true) { // HALT
      delay(1);
    }
  }
  log_d("new Que: %X", this->btn_que);

  this->worker = new Task_ButtonWorker(this->btn_que);
  this->worker->start();
} // Task_ButtonWatcher::Task_ButtonWatcher()

/**
 *
 */
bool Task_ButtonWatcher::addBtn(String name, uint8_t pin) {
  log_d("%s:%d: worker=0x%X", name, pin, this->worker);
  
  if ( this->worker == NULL ) {
    return false;
  }

  this->worker->addBtn(name, pin);
  return true;
} // Task_ButtonWatcher::addBtn()

/**
 *
 */
void Task_ButtonWatcher::enable() {
  log_v("%s", this->conf.name);
  if ( this->worker != NULL ) {
    this->worker->enable();
  }
} // Task_Button::enable()

/**
 *
 */
void Task_ButtonWatcher::disable() {
  log_v("%s", this->conf.name);
  if ( this->worker != NULL ) {
    this->worker->disable();
  }
} // Task_Button::disable()

/**
 *
 */
void Task_ButtonWatcher::setup() {
  log_v("%s", this->conf.name);
} // Task_ButtonWatcher::setup()

/**
 *
 */
void Task_ButtonWatcher::loop() {
  if ( this->worker == NULL ) {
    return;
  }
  
  ButtonInfo_t btn_info;
  portBASE_TYPE ret = this->worker->get(&btn_info);
  if ( ret == pdPASS ) {
      (*(this->_cb))(&btn_info);
  }
} // Task_ButtonWatcher::loop()
