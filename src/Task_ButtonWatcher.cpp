/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Task_ButtonWatcher.h"

/**
 *
 */
Task_ButtonWatcher::Task_ButtonWatcher(void (*cb)(ButtonInfo_t *bi,
                                                  std::map<std::string,
                                                  ButtonInfo_t> btn_info),
                                       uint32_t stack_size,
                                       UBaseType_t priority,
                                       UBaseType_t core)
  : Task(__CLASS_NAME__, stack_size, priority, core)
{
  this->_cb = cb;
  if ( cb == NULL ) {
    this->_cb = Task_ButtonWatcher::def_cb;
  }

  this->worker = new Task_ButtonWorker();
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

  strcpy(this->btn_info[name.c_str()].name, name.c_str());
  this->btn_info[name.c_str()].pin = pin;
  this->btn_info[name.c_str()].value = Button::OFF;
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
    delay(1);
    return;
  }
  
  ButtonInfo_t btn_info1;
  portBASE_TYPE ret = this->worker->get(&btn_info1);
  if ( ret == pdPASS ) {
    this->btn_info = Task_ButtonWorker::BtnInfo;

    (*(this->_cb))(&btn_info1, this->btn_info);
  }
} // Task_ButtonWatcher::loop()

/** static
 * defulat callback
 */
void Task_ButtonWatcher::def_cb(ButtonInfo_t *bi,
                                std::map<std::string, ButtonInfo_t> btn_info) {
  log_d("%s", Button::info2String(bi).c_str());
} // _def_button_cb()
