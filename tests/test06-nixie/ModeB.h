/**
 *
 */

#include "Mode.h"

class ModeB: public Mode {
 public:
  virtual void enter();
  virtual void exit();
  virtual void loop();

  virtual void cbBtn(ButtonInfo_t *bi);
};
