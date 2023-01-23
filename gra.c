#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include "gra.h"

#ifndef PRZYCISKI
#define PRZYCISKI
#define PRZYCISK1 (BIT4 & P4IN)
#define PRZYCISK2 (BIT5 & P4IN)
#define PRZYCISK3 (BIT6 & P4IN)
#define PRZYCISK4 (BIT7 & P4IN)
#endif

int poprzedniaTablica[2][16] = {
    {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

// inicjaly chyba jednak niepotrzebne w tej sekcji tbh
void rozpocznijGre(char inicjaly[2])
{
  // pauza interrupta
  TACTL &= ~MC_1;

  SEND_CMD(CLR_DISP);
  SEND_CMD(CUR_OFF); // wylaczenie kursora
  SEND_CHAR(0);
  SEND_CMD(DD_RAM_ADDR2); // czyli SET_CURSOR(1,2)
  SEND_CHAR(0);

  for (int i = 3; i > 0; i--)
  {
    SET_CURSOR(8, 1);
    SEND_CHAR(48 + i);
    SEND_CHAR('.');
    SEND_CHAR('.');
    //_delay_cycles(1000);
  }

  SET_CURSOR(8, 1);
  SEND_CHAR(' ');
  SEND_CHAR(' ');
  SEND_CHAR(' ');

  TACTL |= MC_1;

  // return inicjaly?
  // juz powinny leciec sobie tyntyryn
}

void przesunKafelki(int tablicaLCD[][16])
{
  int r;
  int c;
  for (r = 0; r < 2; r++)
  {
    SET_CURSOR(1, r + 1);
    for (c = 0; c < 16; c++)
    {
      if (tablicaLCD[r][c] >= 0)
      {
        SEND_CHAR(tablicaLCD[r][c]);
      }
      else
      {
        SEND_CHAR(' ');
      }
    }
  }
}

// tablica 2x16 przechowujaca jaki znak i wyslac