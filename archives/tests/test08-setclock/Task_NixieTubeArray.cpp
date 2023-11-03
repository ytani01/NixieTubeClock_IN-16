/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Task_NixieTubeArray.h"

/** static variables
 *
 */
int Task_NixieTubeArray::INIT_VAL[] = {0, 0, 0, 0, 0, 0};

/** constructor
 *
 */
Task_NixieTubeArray::Task_NixieTubeArray(NixieTubeArray *nta,
                                         brightness_t brightness,
                                         int *init_val,
                                         uint32_t stack_size,
                                         UBaseType_t priority,
                                         UBaseType_t core)
  : Task(__CLASS_NAME__, stack_size, priority, core) {

  this->_nta = nta;
  this->_nta->set_brightness(brightness);

  if ( init_val == NULL ) {
    this->init_val = (int *)Task_NixieTubeArray::INIT_VAL;
  } else {
    this->init_val = init_val;
  }

} // Task_NixieTubeArray::Task_NixieTubeArray()

/**
 * 
 */
void Task_NixieTubeArray::setup() {
  log_d("%s", this->conf.name.c_str());

  for (int i = 0; i < NIXIE_NUM_N; i++) {
    for (int e=0; e < NIXIE_NUM_DIGIT_N; e++) {
      if ( this->init_val[i] == e ) {
        NtaNumEl(i,e).set_brightness_to_max();
      } else {
        NtaNumEl(i,e).set_brightness(0);
      }
    } // for(e)
  } // for(i)

  for (int i=0; i < NIXIE_COLON_N; i++) {
    Nta->colon[i].element[0].set_brightness_to_max();
  } // for(i)
  
} // Task_NixieTubeArray::setup()

/**
 *
 */
void Task_NixieTubeArray::loop() {
  if ( ! enable_update ) {
    return;
  }

  unsigned long cur_ms = millis();
  
  Nta->display(cur_ms);

  delayMicroseconds(1);
} // Task_NixieTubeArray::loop()
