#include "LCD.h"

#ifndef INCLUDE_menu_h
#define INCLUDE_menu_h

struct LCDMenu {
  float temp = 0.0;
  int start_temp = 0;
  int stop_temp = 0;
  bool pump_status = false;
  int mode = 0;
  int menuPos = 2;
};

void startScreen();
void drawMenu(LCDMenu *menu);
void menuChangeItem(LCDMenu *menu, int value);

#endif
