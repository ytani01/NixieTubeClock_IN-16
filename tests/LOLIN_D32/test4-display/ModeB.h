/**
 *
 */

#include "Mode.h"

class ModeB: public Mode {
 public:
  virtual void loop();

  virtual void cbBtn(ButtonInfo_t *bi);
};
