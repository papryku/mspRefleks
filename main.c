#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include "portyUart.h"
#include "uartservice.h"
#include "gra.h"
#include <stdlib.h>
#include <string.h>

#ifndef PRZYCISKI
#define PRZYCISKI
#define PRZYCISK1 (BIT4 & P4IN)
#define PRZYCISK2 (BIT5 & P4IN)
#define PRZYCISK3 (BIT6 & P4IN)
#define PRZYCISK4 (BIT7 & P4IN)
#endif

int czyZlapany(int row);

// zmienna przyjmuje 1, gdy kafelek nie zostanie złapany, co spowoduje przegraną
int niezlapany = 0;

// tyle razy można za wcześnie wcisnąć przycisk, później przegrywa się grę
int zycia = 10;

// za złapany kafelek +10, za zbyt wcześnie wciśnięty przycisk -5
//int wynik = 0;
//zamiast tego uzywam globalny wynikAktualny z uartservice

// reprezentacja naszego ekranu LCD gry, która jest wypisywana co interwał na ekran
int tablicaLCD[2][16] = {
    {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

// spowolnienie TimerA
int licz = 0;

// powstrzymuje problemy z "przytrzymywaniem" przycisku, zezwala na jedno wciśnięcie przycisku na interwał
int przycisk1Niewcisniety = 1;
int przycisk2Niewcisniety = 1;

// reprezentacje liczbowe naszych znaków własnych
char lapanie[8] = {31, 17, 17, 17, 17, 17, 17, 31};
char kafek[8] = {4, 4, 4, 4, 4, 4, 4, 4};
char zlapany[8] = {31, 21, 21, 21, 21, 21, 21, 31};

void main(void)
{
  // wylaczenie watchdoga
  WDTCTL = WDTPW + WDTHOLD;

  // inicjalizacja przyciskow
  P4DIR &= ~BIT4; // zainicjalizowanie pierwszego przycisku
  P4DIR &= ~BIT5; // zainicjalizowanie drugiego przycisku
  P4DIR &= ~BIT6; // zainicjalizowanie trzeciego przycisku
  P4DIR &= ~BIT7; // zainicjalizowanie czwartego przycisku

  // inicjalizacja bibliotek
  InitPortsLcd();
  InitLCD();
  InitPortsUart();
  InitUart();

  // zaladowanie wlasnych znakow do CGRAMu
  CREATE_CHAR(0, lapanie);
  CREATE_CHAR(1, zlapany);
  CREATE_CHAR(2, kafek);

  SET_CURSOR(1, 1);

  // UartCharTransmit('x');

  // inicjalizacja zegara
  BCSCTL1 |= XTS; // ACLK = LFXT1 = HF XTAL 8MHz

  do
  {
    IFG1 &= ~OFIFG; // Czyszczenie flgi OSCFault
    for (int i = 0xFF; i > 0; i--)
      ;                              // odczekanie
  } while ((IFG1 & OFIFG) == OFIFG); // dop�ki OSCFault jest ci�gle ustawiona

  BCSCTL1 |= DIVA_1;        // ACLK=8 MHz/2=4 MHz
  BCSCTL2 |= SELM0 | SELM1; // MCLK= LFTX1 =ACLK

  // Timer_A ustawiamy na 500 kHz
  // a przerwanie generujemy co 100 ms
  TACTL = TASSEL_1 + MC_1 + ID_3; // Wybieram ACLK, ACLK/8=500kHz,tryb Up
  CCTL0 = CCIE;                   // w��czenie przerwa� od CCR0
  CCR0 = 500000;                  // podzielnik 5000: przerwanie co 10 ms
  //^tym sie trzeba pobawic jeszcze

  _EINT(); // w��czenie przerwa�

  menu();
  // rozpocznijGre(); niby jest to w menu

  // nieskończona pętla, miejsce na nasz program
  while (1)
  {
    if (!(PRZYCISK1) && przycisk1Niewcisniety)
    {
      przycisk1Niewcisniety = 0;
      if (czyZlapany(1))
      {
        SET_CURSOR(1, 1);
        SEND_CHAR(0);
        wynikAktualnej += 10;
      }
      else
      {
        wynikAktualnej -= 5;
        zycia--;
      }
    }
    if (!(PRZYCISK2) && przycisk2Niewcisniety)
    {
      przycisk2Niewcisniety = 0;
      if (czyZlapany(2))
      {
        SET_CURSOR(1, 2);
        SEND_CHAR(0);
        wynikAktualnej += 10;
      }
      else
      {
        wynikAktualnej -= 5;
        zycia--;
      }
    }

    // do testow zatrzymywanie

    else if (!(PRZYCISK3))
    {
      TACTL &= ~MC_1;
    }
    else if (!(PRZYCISK4))
    {
      TACTL |= MC_1;
    }

    if (zycia <= 0 || niezlapany)
    {
      niezlapany = 0;
      zycia = 10;
      koniecGry();
    }
  }
}

// tbh w trakcie się przekonam, czy to koniecznie
// może wystarczy same sprawdzanie czy w 16. kolumnie jest wolne xD
int czyZlapany(int row)
{
  if (tablicaLCD[row - 1][0] == 1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// losowanie lokalizacji kolejnego kafelka/kafelków lub ich braku
int los;

// przerwania zegara
#pragma vector = TIMERA0_VECTOR
__interrupt void Timer_A(void)
{
  int r;
  int c;

  // licznik do spowolnienia, z tym sie pobawimy troche
  licz++;
  if (licz == 8)
  {
    licz = licz % 8;

    if (1)
    {

      // wpierw operacje na pierwszych dwoch kolumnach
      for (r = 0; r < 2; r++)
      {
        // sprawdzenie, czy kafelek nie ucieknie
        if (tablicaLCD[r][0] == 1)
        {
          niezlapany = 1;
        }

        // ustawienie odpowiedniego znaku pola lapania(z kafelkiek lub bez)
        if (tablicaLCD[r][1] == 2)
        {
          tablicaLCD[r][0] = 1;
          tablicaLCD[r][1] = -1;
        }
        else
        {
          tablicaLCD[r][0] = 0;
        }
      }
      // potem przesuwa kafelki
      for (r = 0; r < 2; r++)
      {
        for (c = 2; c < 16; c++)
        {
          if (tablicaLCD[r][c] == 2)
          {
            tablicaLCD[r][c - 1] = 2;
            tablicaLCD[r][c] = -1;
          }
        }
      }
      los = rand() % 4;
      switch (los)
      {
      case 0:
        break;
      case 1:
        tablicaLCD[0][15] = 2;
        break;
      case 2:
        tablicaLCD[1][15] = 2;
        break;
      case 3:
        tablicaLCD[0][15] = 2;
        tablicaLCD[1][15] = 2;
        break;
      }
      // sprawdzenie, czy jakiś kafelek nie został niezłapany
      if (tablicaLCD[])

        // a na koniec
        przesunKafelki(tablicaLCD);
      przycisk1Niewcisniety = 1;
      przycisk2Niewcisniety = 1;
      // los++;
    }
  }
}
