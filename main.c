#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include "portyUart.h"
#include "uart.h"
#include "znaki.h"
#include "uartservice.h"
#include "kafelek.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PRZYCISK1 (BIT4&P4IN)
#define PRZYCISK2 (BIT5&P4IN)
#define PRZYCISK3 (BIT6&P4IN)
#define PRZYCISK4 (BIT7&P4IN) 

bool czyJestKafelek(int row, int col);

Kafelek *kafelki[32];
int indeksKaf = 0;

main (void) {
  //wylaczenie watchdoga
  WDTCTL = WDTPW + WDTHOLD;

  //inicjalizacja randa
  //time_t t;
  srand(1);

  //inicjalizacja przyciskow
  P4DIR &= ~BIT4;             // zainicjalizowanie pierwszego przycisku
  P4DIR &= ~BIT5;             // zainicjalizowanie drugiego przycisku
  P4DIR &= ~BIT6;             // zainicjalizowanie trzeciego przycisku
  P4DIR &= ~BIT7;             // zainicjalizowanie czwartego przycisku
  
  //inicjalizacja bibliotek
  InitPortsLcd();
  InitLCD();
  InitPortsUart();
  InitUart(1200);

  //zaladowanie znakow do tablicy
  CREATE_CHARACTER(lapanie,0);
  CREATE_CHARACTER(zlapany,1);
  CREATE_CHARACTER(kafelek,2);

  //inicjalizacja zegara
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

  menu();
  
  //nieskończona pętla, miejsce na nasz program
  while(1){
    
  }
}

//przerwania uart



//przerwania zegara
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void){
  if(rozpoczeta){
    int los = rand() % 7;
    switch(los){
    case 1: break;
    case 2: 
      if(!czyJestKafelek(1,16)){
        SET_CURSOR(1,16); Kafelek *nowy; nowy->row = 1; nowy->column = 16; nowyKafelek(*nowy); kafelki[indeksKaf++] = nowy;
      } break;
    case 3: 
      if(!czyJestKafelek(2,16)){
        SET_CURSOR(1,16); Kafelek nowy; nowy.row = 2; nowy.column = 16; nowyKafelek(nowy); *kafelki[indeksKaf++] = nowy;
      } break;
    case 4: 
      if(!czyJestKafelek(1,16)){
        SET_CURSOR(1,16); Kafelek *nowy1, *nowy2; nowy1->row = 1; nowy2->row = 2; 
        nowy1->column = 16; nowy2->column = 16; nowyKafelek(*nowy1); nowyKafelek(*nowy2); 
        kafelki[indeksKaf++] = nowy1; kafelki[indeksKaf++] = nowy2;
       } break;
    default: break;
    }
    przesunKafelki(*kafelki);
  }
}

bool czyJestKafelek(int row, int col){
  int dl = sizeof(kafelki)/sizeof(kafelki[0]);
  for(int i=0; i<dl; i++){
      if((kafelki[i]->row==row) && (kafelki[i]->column==col)){
        return true;
      }
    }
  return false;
}