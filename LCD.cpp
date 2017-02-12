#include "LCD.h"

void LcdWrite(unsigned char dc, unsigned char data)
{
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}


void LcdInitialise(void)
{
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  pinMode(PIN_LCD, OUTPUT);
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
  LcdWrite(LCD_C, 0x21 );  // LCD Extended Commands.
  LcdWrite(LCD_C, 0xBF );  // Set LCD Vop (Contrast).
  LcdWrite(LCD_C, 0x04 );  // Set Temp coefficent. //0x04
  LcdWrite(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_C, 0x20 );  // LCD Basic Commands
  LcdWrite(LCD_C, 0x0C );  // LCD in normal mode.
  digitalWrite(PIN_LCD, HIGH);
}

void LcdCharacter(unsigned char character, bool invert = false)
{
  for (int index = 0; index < 5; index++)
  {
    if (invert) {
      LcdWrite(LCD_D, ~(ASCII[character - 0x20][index]));
    } else {
      LcdWrite(LCD_D, ASCII[character - 0x20][index]);
    }
  }
  LcdWrite(LCD_D, invert ? 0xff : 0x00);
}

void LcdSetCursor(int column, int line)
{
  LcdWrite(LCD_C, 0x80 | column);
  LcdWrite(LCD_C, 0x40 | line);
}

void LcdClear(void)
{
  for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
  {
    LcdWrite(LCD_D, 0x00);
  }
}

void LcdString(const char *characters, bool invert = false)
{
  while (*characters)
  {
    LcdCharacter(*characters++, invert);
  }
}


