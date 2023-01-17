//w tym pliku chce polaczyc biblioteki(te problematyczne) w jedno, bo nic sensowniejszego na błąd z pracowni nie mogłem wynaleźć...
#include "msp430x14x.h"
#include "lcd.h"
#include "portyLcd.h"
#include "znaki.h"
//#include "uartservice.c"
#include <stdbool.h>

// kafelek.h
typedef struct Kafelek {
    int row;
    int column;
} Kafelek;

// gra.c
int rozpoczeta = false;

//inicjaly chyba jednak niepotrzebne w tej sekcji tbh
void rozpocznijGre(){
    //pauza interrupta
    TACTL &= ~MC_1;

    SEND_CMD(CLR_DISP);
    SEND_CMD(CUR_OFF); //wylaczenie kursora
    SEND_CHAR(0);
    SEND_CMD(DD_RAM_ADDR2); //czyli SET_CURSOR(1,2)
    SEND_CHAR(0);

    for(int i = 3; i>0; i--){
        SET_CURSOR(8,1);
        SEND_CHAR(48+i);
        SEND_CHAR('.');
        SEND_CHAR('.');
    }

    //może return jakby byl problem z odpauzowaniem interrupta
    //odpauzowanie interrupta
    
    TACTL |= MC_1;
    rozpoczeta = true;
    //juz powinny leciec sobie tyntyryn
}


//struct gotowego kafelka z parametrami bedzie tworzony
//w przerwaniach zegara, poniewaz z poziomu namespace'a gra nie mamy
//dostepu do tablicy istniejacych kafelkow, w skrocie informacji czy pozycja zajeta bla bla bla
void nowyKafelek(Kafelek *nowy){
    SET_CURSOR(nowy->column, nowy->row);
    SEND_CHAR(1);
}

//
void przesunKafelki(Kafelek *kafelki){
    int dl = sizeof(kafelki)/sizeof(kafelki[0]);
    int stan1 = 0, stan2 = 1;

    //inna mozliwosc to bezposrednia edycja 30 pol DDRAMU z odpowiednim przesunieciem w lewo pomijajac pola lapania
    SEND_CMD(DATA_ROL_LEFT);
    //SET_CURSOR(1,2); SEND_CHAR(0);

    for(int i=0; i<dl; i++){
        kafelki[i].column--;
        if(kafelki[i].column==1){
            SET_CURSOR(1,kafelki[i].row);
            SEND_CHAR(2);
        }
    }
    if(!stan1) { SET_CURSOR(1,1); SEND_CHAR(0); }
    if(!stan2) { SET_CURSOR(1,2); SEND_CHAR(0); }

    //nie wiem jeszcze jak zareagowac na kafelek==0 i gdzie, chyba w timerze??
    //albo miec w powazaniu az nie okaze sie powodowac artefakty graficzne
}

// main.c

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