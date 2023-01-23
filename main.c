#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include "portyUart.h"
//#include "uartservice.h"
#include "gra.h"
#include <stdlib.h>
#include <string.h>

#ifndef PRZYCISKI
#define PRZYCISKI
#define PRZYCISK1 (BIT4&P4IN)
#define PRZYCISK2 (BIT5&P4IN)
#define PRZYCISK3 (BIT6&P4IN)
#define PRZYCISK4 (BIT7&P4IN)
#endif

int czyZlapany(int row);

int w=0;
int zycia = 10;
int wynik = 0; int tmp;
int rozpoczeta = 0;
int tablicaLCD[2][16] = {
  {0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  {0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};
int indeksKaf = 0;
int licz=0;
int przyciskNiewcisniety = 1;

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

  //inicjalizacja przyciskow
  P4DIR &= ~BIT4;             // zainicjalizowanie pierwszego przycisku
  P4DIR &= ~BIT5;             // zainicjalizowanie drugiego przycisku
  P4DIR &= ~BIT6;             // zainicjalizowanie trzeciego przycisku
  P4DIR &= ~BIT7;             // zainicjalizowanie czwartego przycisku
  
  //inicjalizacja bibliotek
  InitPortsLcd();
  InitLCD();
  InitPortsUart();
  InitUart();
  
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
  
  //TACTL ^= MC_1;
  
  //menu();
  
  rozpocznijGre(&rozpoczeta);
  
  //nieskończona pętla, miejsce na nasz program
  while(rozpoczeta){
  if(przyciskNiewcisniety){
    if(!(PRZYCISK1)){
      przyciskNiewcisniety=0;
      if(czyZlapany(1)){
        SET_CURSOR(1,1); SEND_CHAR(0);
        wynik+=10;
      }else{
        wynik-=5; zycia--;
      }
    }
    if(!(PRZYCISK2)){
      przyciskNiewcisniety=0;
      if(czyZlapany(2)){
        SET_CURSOR(1,2); SEND_CHAR(0);
        wynik+=10;
      }else{
        wynik-=5; zycia--;
      }
    }
    
    //do testow zatrzymywanie
    
    else if(!(PRZYCISK3)){
      TACTL &= ~MC_1;
    }
    else if(!(PRZYCISK4)){
      TACTL |= MC_1;
    }
    

    
    if(zycia<=0){
      char inicjaly[2] = {'P','U'};
      koniecGry(inicjaly, wynik, &rozpoczeta);
    }
  }   
  }
}


//tbh w trakcie się przekonam, czy to koniecznie
//może wystarczy same sprawdzanie czy w 16. kolumnie jest wolne xD
int czyZlapany(int row){
  if(tablicaLCD[row-1][0]==1){
    return 1;
  } else{
    return 0;
  }
}



int los;

//przerwania zegara
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  int r; int c;
  //licznik do spowolnienia, z tym sie pobawimy troche
  licz++;
  if(licz==8){
    licz=licz%8;
    if(rozpoczeta){
      //wpierw operacje na pierwszych dwoch kolumnach
      for(r=0; r<2; r++){
        if(tablicaLCD[r][1] == 2){
          tablicaLCD[r][0] = 1;
          tablicaLCD[r][1] = -1;
        }else{
          tablicaLCD[r][0] = 0;
        }
      }
      //potem przesuwa
      for(r=0; r<2; r++){
        for(c=2; c<16; c++){
          if(tablicaLCD[r][c]==2){
            tablicaLCD[r][c-1]=2;
            tablicaLCD[r][c]=-1;
          }
        }
      }
      los = rand() % 4;
      switch(los){
	            case 0: break;
		    case 1: tablicaLCD[0][15] = 2; break;
		    case 2: tablicaLCD[1][15] = 2; break;
		    case 3: tablicaLCD[0][15] = 2; tablicaLCD[1][15] = 2; break;
		 }
    
      //a na koniec
      przesunKafelki(tablicaLCD);
      przyciskNiewcisniety=1;
      //los++;
    }
  }
}
