#include <msp430x14x.h>

/** @file */

void InitPortsLcd(void)
{
  
  P2SEL = 0;
  P2OUT = 0;
  P2DIR = ~BIT0;                                //only P2.0 is input
}