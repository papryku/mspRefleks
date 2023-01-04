#include "msp430x14x.h"
#include "lcd.h"
#include "portyLcd.h"

#define PRZYCISK1 (BIT4&P4IN)
#define PRZYCISK2 (BIT5&P4IN)
#define PRZYCISK3 (BIT6&P4IN)
#define PRZYCISK4 (BIT7&P4IN) 

main (void) {
  InitPortsLcd();
  InitLCD();
}