/**
 *
 */

#include "Mode.h"

class ModeB: public Mode {
 public:
  virtual bool enter();
  virtual void loop();

  virtual void cbBtn(ButtonInfo_t *bi);
};
