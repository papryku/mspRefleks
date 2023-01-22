#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include "portyUart.h"
//#include "uartservice.h"
#include "kafelek.h"
#include "gra.h"

#ifndef PRZYCISK1
#define PRZYCISK1 (BIT4&P4IN)
#endif

#ifndef PRZYCISK2
#define PRZYCISK2 (BIT5&P4IN)
#endif

#ifndef PRZYCISK3
#define PRZYCISK3 (BIT6&P4IN)
#endif

#ifndef PRZYCISK4
#define PRZYCISK4 (BIT7&P4IN)
#endif

int czyJestKafelek(int row, int col);

int w=0;
int rozpoczeta = 0;
Kafelek kafelki[32];
int indeksKaf = 0;
int licz=0;

char lapanie[8] = {
	31,//0b11111,
	17,//0b10001,
	17,//0b10001,
	17,//0b10001,
	17,//0b10001,
	17,//0b10001,
	17,//0b10001,
	31,//0b11111
};

char kafek[8] = {
	4,//0b00000,
	4,//0b00000,
	4,//0b00100,
	4,//0b00100,
	4,//0b00100,
	4,//0b00100,
	4,//0b00000,
	4,//0b00000
};

char zlapany[8] = {
	31,//0b11111,
	21,//0b10001,
	21,//0b10101,
	21,//0b10101,
	21,//0b10101,
	21,//0b10101,
	21,//0b10001,
	31,//0b11111
};




void main (void) {
  //wylaczenie watchdoga
  WDTCTL = WDTPW + WDTHOLD;

  //inicjalizacja randa
  //time_t t;
  //srand(1);

  //inicjalizacja przyciskow
  P4DIR &= ~BIT4;             // zainicjalizowanie pierwszego przycisku
  P4DIR &= ~BIT5;             // zainicjalizowanie drugiego przycisku
  P4DIR &= ~BIT6;             // zainicjalizowanie trzeciego przycisku
  P4DIR &= ~BIT7;             // zainicjalizowanie czwartego przycisku
  
  //inicjal izacja bibliotek
  InitPortsLcd();
  InitLCD();
//  InitPortsUart();
//  InitUart(1200);
  
  //zaladowanie znakow do tablicy
  CREATE_CHAR(0,lapanie);
  CREATE_CHAR(1,zlapany);
  CREATE_CHAR(2,kafek);
  
  SET_CURSOR(1,1);
  //SEND_CHAR(0);
  //SEND_CHAR(1);
  //SEND_CHAR(2);

 // UartCharTransmit('x');
  //inicjalizacja zegara
  BCSCTL1 |= XTS; // ACLK = LFXT1 = HF XTAL 8MHz

  do 
    {
    IFG1 &= ~OFIFG; // Czyszczenie flgi OSCFault
    for (int i = 0xFF; i > 0; i--); // odczekanie
    SEND_CHAR(2);
    }
    while ((IFG1 & OFIFG) == OFIFG); // dop�ki OSCFault jest ci�gle ustawiona   
  
  BCSCTL1 |= DIVA_1; // ACLK=8 MHz/2=4 MHz
  BCSCTL2 |= SELM0 | SELM1; // MCLK= LFTX1 =ACLK
  
  // Timer_A ustawiamy na 500 kHz
  // a przerwanie generujemy co 100 ms
  TACTL = TASSEL_1 + MC_1 +ID_3; // Wybieram ACLK, ACLK/8=500kHz,tryb Up
  CCTL0 = CCIE; // w��czenie przerwa� od CCR0
  CCR0=500000; // podzielnik 5000: przerwanie co 10 ms
  
  _EINT(); // w��czenie przerwa�
  
  SEND_CHARS("zaladowane");
  SET_CURSOR(1,2);
  SEND_CHARS("dzialo");
  
  //TACTL ^= MC_1;
  
  rozpocznijGre(&rozpoczeta);
  
  
  //nieskończona pętla, miejsce na nasz program
  while(1){
   //SEND_CHAR(1);
  }
}

int czyJestKafelek(int row, int col){
  int dl = sizeof(kafelki)/sizeof(kafelki[0]);
  for(int i=0; i<dl; i++){
      if((kafelki[i].row==row) && (kafelki[i].column==col)){
        return 1;
      }
    }
  return 0;
}

//przerwania uart


//przerwania zegara
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  licz++;
  if(licz==5){
    if(!w){
      licz=licz%5;
       if(rozpoczeta){
          int los = 2; //rand
          switch(los){
          case 1: break;
          case 2: 
            if(!czyJestKafelek(1,16)){
              Kafelek nowy; nowy.row = 1; nowy.column = 16; nowyKafelek(&nowy); kafelki[indeksKaf++] = nowy;
            } break;
          case 3: 
            if(!czyJestKafelek(2,16)){
              Kafelek nowy; nowy.row = 2; nowy.column = 16; nowyKafelek(&nowy); kafelki[indeksKaf++] = nowy;
            } break;
          case 4: 
            if(!czyJestKafelek(1,16) && !czyJestKafelek(2,16)){
              Kafelek nowy1, nowy2; nowy1.row = 1; nowy2.row = 2; 
              nowy1.column = 16; nowy2.column = 16; nowyKafelek(&nowy1); nowyKafelek(&nowy2); 
              kafelki[indeksKaf++] = nowy1; kafelki[indeksKaf++] = nowy2;
             } break;
          default: break;
          }
          przesunKafelki(kafelki,&w);
        }
    }
  }
}
