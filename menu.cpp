#include "menu.h"

void startScreen()
{
  //initalize lcd screen
  LcdInitialise();
  LcdClear();
  LcdString(" Dla kochanej");
  LcdSetCursor(0, 1);
  LcdString("  Hani i Heli ");

  delay(5000);
  LcdClear();
  LcdSetCursor(0, 0);

}

void menuLogic(LCDMenu *menu)
{
  if (menu->menuPos > 4) {
    menu->menuPos = 2;
  }

  if (menu->menuPos < 2) {
    menu->menuPos = 4;
  }
}

void menuChangeItem(LCDMenu *menu, int value)
{
  switch (menu->menuPos) {
    case 2:
      menu->start_temp += value > 0 ? 1 : -1;
      if (menu->start_temp <= menu->stop_temp) {
        menu->start_temp = menu->stop_temp + 1;
      }
      break;
    case 3:
      menu->stop_temp += value > 0 ? 1 : -1;
      if (menu->start_temp <= menu->stop_temp) {
        menu->stop_temp = menu->start_temp - 1;
      }
      break;
    case 4:
      menu->mode += value > 0 ? 1 : -1;
      menu->mode = menu->mode % 3;
      break;
  }
}

void drawTemp(LCDMenu *menu) {
  char str_temp[] = "T              ";
  dtostrf(menu->temp, 4, 2, &str_temp[9]);
  LcdSetCursor(0, 0);
  LcdString(str_temp, menu->menuPos == 0);
}

void drawPump(LCDMenu *menu) {
  LcdSetCursor(0, 1);
  if (menu->pump_status) LcdString("Pompa      Zal", menu->menuPos == 1);
  else LcdString("Pompa      Wyl", menu->menuPos == 1);
}

void drawPumpStart(LCDMenu *menu) {
  char str_temp[] = "T zal.         ";
  dtostrf(menu->start_temp, 4, 1, &str_temp[10]);
  LcdSetCursor(0, 2);
  LcdString(str_temp, menu->menuPos == 2);
}

void drawPumpStop(LCDMenu *menu) {
  char str_temp[] = "T wyl.         ";
  dtostrf(menu->stop_temp, 4, 1, &str_temp[10]);
  LcdSetCursor(0, 3);
  LcdString(str_temp, menu->menuPos == 3);
}

void drawMode(LCDMenu *menu) {
  LcdSetCursor(0, 4);
  switch (menu->mode) {
    case 0:
      LcdString("Tryb    Wylacz", menu->menuPos == 4);
      break;
    case 1:
      LcdString("Tryb    Zalacz", menu->menuPos == 4);
      break;
    case 2:
      LcdString("Tryb      Auto", menu->menuPos == 4);
      break;
    default:
      LcdString("Tryb      Auto", menu->menuPos == 4);
      break;
  }

}

void drawMenu(LCDMenu *menu)
{
  menuLogic(menu);
  drawTemp(menu);
  drawPump(menu);
  drawPumpStart(menu);
  drawPumpStop(menu);
  drawMode(menu);
  LcdSetCursor(0, 0);
}

