#ifndef __NINJA_LAMP_TOP_MENU__
#define __NINJA_LAMP_TOP_MENU__

#include "lcdgfx.h"

class NinjaLAMPTopMenu {
  public:
    NinjaLAMPTopMenu();
    void setup(int pinMoveUp, int pinOk, int pinMoveDown); 
    int loop(int state);
  private:
    // The pins will be set in setup depending on the orientation.
    int pinMoveUp = 0;   // move up
    int pinOk = 0;        // enter
    int pinMoveDown = 0;  // move down
    void swapBtnFocus();
};
#endif /* __NINJA_LAMP_TOP_MENU__ */