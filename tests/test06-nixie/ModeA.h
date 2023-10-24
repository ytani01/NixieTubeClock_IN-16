/**
 *
 */

#include "Mode.h"

class ModeA: public Mode {
 public:

  ModeA();

  virtual void enter();
  virtual void exit();
  virtual void loop();

  virtual void cbBtn(ButtonInfo_t *bi);
};
