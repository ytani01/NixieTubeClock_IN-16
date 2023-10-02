/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Task_NixieTubeArray.h"

/** constructor
 *
 */
Task_NixieTubeArray::Task_NixieTubeArray(NixieTubeArray *nta)
  : Task(String(Task_NixieTubeArray::name)) {

  this->_nta = nta;  
} // Task_NixieTubeArray::Task_NixieTubeArray()

/**
 * 
 */
void Task_NixieTubeArray::setup() {
  log_i("%s", this->conf.name);

  Nta->brightness = BRIGHTNESS_RESOLUTION / 4;

  int init_val[NIXIE_NUM_N] = {0, 1, 2, 3, 4, 5};
  for (int i = 0; i < NIXIE_NUM_N; i++) {
    for (int e=0; e < NIXIE_NUM_DIGIT_N; e++) {
      if ( init_val[i] == e ) {
        //Nta->num[i].element[e].set_brightness(Nta->brightness);
        NtaNumEl(i,e).set_brightness(Nta->brightness);
      } else {
        //Nta->num[i].element[e].set_brightness(0);
        NtaNumEl(i,e).set_brightness(0);
      }
    } // for(e)
  } // for(i)

  for (int i=0; i < NIXIE_COLON_N; i++) {
    Nta->colon[i].element[0].set_brightness(Nta->brightness);
  } // for(i)
  
} // Task_NixieTubeArray::setup()

/**
 *
 */
void Task_NixieTubeArray::loop() {
  //log_i("%s", this->conf.name);
  unsigned long cur_ms = millis();
  
  Nta->display(cur_ms);

  delayMicroseconds(1);
} // Task_NixieTubeArray::loop()
