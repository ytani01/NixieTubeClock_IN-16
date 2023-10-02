/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "Task.h"

/**
 *
 */
Task::Task(String name,
           uint32_t stack_size, UBaseType_t priority, UBaseType_t core) {
  String nameString = name;
  if ( nameString.length() > TASK_NAME_LEN ) {
    nameString = nameString.substring(0, TASK_NAME_LEN);
  }
  log_d("nameString=%s", nameString.c_str());
  strcpy(this->conf.name, nameString.c_str());

  this->conf.handle = NULL;
  this->conf.stack_size = stack_size;
  this->conf.priority = priority;
  this->conf.core = core;

  this->_active = false;
  
  delay(100); // XXX これがないと、危ない(!?)
} // Task::Task()

/**
 *
 */
void Task::start() {
  BaseType_t ret = xTaskCreateUniversal(Task::call_task_main,
                                        this->conf.name,
                                        this->conf.stack_size,
                                        this,
                                        this->conf.priority,
                                        &(this->conf.handle),
                                        this->conf.core);
  log_i("Start:%s(Stack:%dB,Priority:%d,CPU:%d,handle:%X): ret=%d",
        this->conf.name, this->conf.stack_size, this->conf.priority,
        this->conf.core, this->conf.handle,
        ret);
  if ( ret != pdPASS ) {
    log_e("ret=%d .. HALT", ret);
    while (true) { // !!! halt !!!
      delay(1);
    }
  }
  delay(100);
  return;
} // Task::start()

/**
 *
 */
bool Task::is_active() {
  return this->_active;
}

/**
 *
 */
void Task::setup() {
  log_d("");
} // Task::setup()

/**
 *
 */
void Task::loop() {
  log_d("");
  delay(1000);
} // Task::loop()

/**
 * TaskFunctionは、staticでなければならいので、
 * static関数を定義して、その引数に``this``を渡して、
 * task_main()を呼び出す
 */
void Task::call_task_main(void *this_instance) {
  static_cast<Task *>(this_instance)->__task_main();
} // Task::call_task_main()

/**
 *
 */
void Task::__task_main() {
  this->_active = false;
  this->setup();
  delay(1);

  this->_active = true;
  while (true) { // main loop
    this->loop();
    delay(1);
  } // main loop
  this->_active = false;
  vTaskDelete(NULL);
} // Task::task_main()
