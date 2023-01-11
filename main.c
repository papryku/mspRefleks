#include "msp430x14x.h"
#include "lcd.h"
#include "portyLcd.h"
#include "portyUart.h"
#include "uart.h"
#include "znaki.h"

#define PRZYCISK1 (BIT4&P4IN)
#define PRZYCISK2 (BIT5&P4IN)
#define PRZYCISK3 (BIT6&P4IN)
#define PRZYCISK4 (BIT7&P4IN) 

main (void) {
  //wylaczenie watchdoga

  //inicjalizacja bibliotek
  InitPortsLcd();
  InitLCD();
  InitPortsUart();
  InitUart(1200);

  //zaladowanie znakow do tablicy
  CREATE_CHARACTER(lapanie,0);
  CREATE_CHARACTER(zlapany,1);
  CREATE_CHARACTER(kafelek,2);

  //ustawienie zegara
  BCSCTL1 |= XTS;
  do
  {
    IFG1 &= ~OFIFG; // zerowanie flagi OSCFault
    for (int i = 0xFF; i > 0; i--); // odczekujemy 50 μs
  }
  while ((IFG1 & OFIFG) == OFIFG);
  BCSCTL1 |= DIVA_1;
  BCSCTL2 |= SELM0 | SELM1;
  TACTL = TASSEL_1 + MC_1 + ID_3;
  CCTL0 = CCIE;
  CCR0=50000;
  _EINT();

  //nieskończona pętla, miejsce na nasz program
  while(1){

  }
}

//przerwania uart



//przerwania zegara
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void){

}
