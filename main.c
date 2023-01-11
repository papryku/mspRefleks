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


  //nieskończona pętla, miejsce na nasz program
  while(1){

  }
}

//przerwania uart



//przerwania zegara