#include <msp430x14x.h>

/** @file */

/**
 * @brief funkcja inicjalizuje port 2.0 układu MSP430f149 do współpracy z EasyWeb2
 */
void InitPortsLcd(void)
{
  
  P2SEL = 0;
  P2OUT = 0;
  P2DIR = ~BIT0;                                //only P2.0 is input
}