#include <msp430x14x.h>
#include "portyUart.h"

void InitPortsUart(void)
{
 P3SEL |= Tx + Rx;
 P3DIR |= Tx;
 P3DIR &= ~ Rx;
}