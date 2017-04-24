/*
* =====================================================================================
*       Filename:  PCD8544.c
*
*    Description:  Driver to control the PCD8544 LCD display
*
*        Version:  1.0
*        Created:  22 April 2017 11:43
*
*         Author:  Olivier Pimi , pimium@gmail.com
*   Organization:  Pimium
* This Code has extra features
 * including a XY positioning function on Display
 * and a Line Draw function on Nokia 3310 LCD
 * It is modded from the original
 * http://playground.arduino.cc/Code/PCD8544
 * // Mods by Jim Park
 * // jim(^dOt^)buzz(^aT^)gmail(^dOt^)com
 * // hope it works for you
* =====================================================================================
*/
#include "PCD8544.h"
#include "inc/LPC11xx.h"

void set_NOKIA_RES(void) { PCD8544_GPIO->DATA |= SRES; }
void clear_NOKIA_RES(void) { PCD8544_GPIO->DATA &= ~SRES; }
void set_NOKIA_CLK(void) { PCD8544_GPIO->DATA |= SCLK; }
void clear_NOKIA_CLK(void) { PCD8544_GPIO->DATA &= ~SCLK; }
void set_NOKIA_DIN(void) { PCD8544_GPIO->DATA |= SDIN; }
void clear_NOKIA_DIN(void) { PCD8544_GPIO->DATA &= ~SDIN; }
void set_NOKIA_DnC(void) { PCD8544_GPIO->DATA |= SDnC; }
void clear_NOKIA_DnC(void) { PCD8544_GPIO->DATA &= ~SDnC; }
void set_NOKIA_CEN(void) { PCD8544_GPIO->DATA |= SCEN; }
void clear_NOKIA_CEN(void) { PCD8544_GPIO->DATA &= ~SCEN; }

void init_PCD8544(void) {
  int count_max = 1000;

  LPC_IOCON->SWCLK_PIO0_10 &= ~(0x7);
  LPC_IOCON->SWCLK_PIO0_10 |= 0x11;
  LPC_IOCON->R_PIO0_11 &= ~(0x7);
  LPC_IOCON->R_PIO0_11 |= 0x11;

  PCD8544_GPIO->DIR |= (SRES | SCLK | SCEN | SDIN | SDnC);

  clear_NOKIA_RES();
  for (int count = 0; count < count_max; count++)
    ;
  set_NOKIA_RES();

  LcdWrite(LCD_CMD, 0x21); // LCD Extended Commands.
  LcdWrite(LCD_CMD, 0xBf); // Set LCD Vop (Contrast). //B1
  LcdWrite(LCD_CMD, 0x04); // Set Temp coefficent. //0x04
  LcdWrite(LCD_CMD, 0x14); // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_CMD, 0x0C); // LCD in normal mode. 0x0d for inverse
  LcdWrite(LCD_C, 0x20);
  LcdWrite(LCD_C, 0x0C);
}
// gotoXY routine to position cursor
// x - range: 0 to 84
// y - range: 0 to 5

void gotoXY(unsigned int x, unsigned int y) {
  LcdWrite(0, 0x80 | x); // Column.
  LcdWrite(0, 0x40 | y); // Row.
}

void LcdWrite(unsigned int dc, unsigned int data) {
  int count_max = 10000;
  if (dc) {
    set_NOKIA_DnC();
  } else
    clear_NOKIA_DnC();

  clear_NOKIA_CEN();
  clear_NOKIA_CLK();
  if ((data & 0x80) == 0x80) {
    set_NOKIA_DIN();
  } else
    clear_NOKIA_DIN();

  for (int i = 0; i < 8; ++i) {
    for (int count = 0; count < count_max; count++)
      ;
    set_NOKIA_CLK();
    for (int count = 0; count < count_max; count++)
      ;
    clear_NOKIA_CLK();
    data = data << 1;
    if ((data & 0x80) == 0x80) {
      set_NOKIA_DIN();
    } else
      clear_NOKIA_DIN();
  }
  set_NOKIA_CEN();
  for (int count = 0; count < count_max; count++)
    ;
}

void LcdCharacter(unsigned int character) {
  LcdWrite(LCD_D, 0x00);
  for (int index = 0; index < 5; index++) {
    LcdWrite(LCD_D, ASCII[character - 0x20][index]);
  }
  LcdWrite(LCD_D, 0x00);
}

void drawLine(void) {
  unsigned char j;
  for (j = 0; j < 84; j++) // top
  {
    gotoXY(j, 0);
    LcdWrite(1, 0x01);
  }
  for (j = 0; j < 84; j++) // Bottom
  {
    gotoXY(j, 5);
    LcdWrite(1, 0x80);
  }

  for (j = 0; j < 6; j++) // Right
  {
    gotoXY(83, j);
    LcdWrite(1, 0xff);
  }
  for (j = 0; j < 6; j++) // Left
  {
    gotoXY(0, j);
    LcdWrite(1, 0xff);
  }
}
