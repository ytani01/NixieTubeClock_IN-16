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

  Nta->set_string("        ");
#if 0
  for (int i = 0; i < NIXIE_NUM_N; i++) {
    for (int e=0; e < NIXIE_NUM_DIGIT_N; e++) {
      NtaNumEl(i,e).set_brightness_to_max();
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
  
  log_i("uxTaskGetStackHighWaterMark = %d", uxTaskGetStackHighWaterMark(NULL));
#endif
} // Task_NixieTubeArray::setup()

/**
 *
 */
void Task_NixieTubeArray::loop() {
#if 1
  static uint32_t prev_ms = 0;
  static uint32_t prev_heap_size = 0;
  static uint32_t prev_stack_size = 0;

  uint32_t ms = millis();
  uint32_t heap_size = esp_get_free_heap_size() / 1000;
  uint32_t stack_size = uxTaskGetStackHighWaterMark(NULL);

  if ( ms - prev_ms >= 30 * 1000 ||
       heap_size / 10 * 10 != prev_heap_size / 10 * 10 ||
       stack_size != prev_stack_size ) {

    log_i("===== heap_size = %uk, stack_size = %u",
          heap_size, stack_size);

    prev_ms = ms;
    prev_heap_size = heap_size;
    prev_stack_size = stack_size;
  }
#endif

  if ( ! enable_update ) {
    return;
  }

  unsigned long cur_ms = millis();
  
  Nta->display(cur_ms);

  delayMicroseconds(1);
} // Task_NixieTubeArray::loop()
