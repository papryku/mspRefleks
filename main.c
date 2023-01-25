#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include "menu.h"
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

int rozpoczeta = 0;

int czyZlapany(int row);

void resetTablicy();

// zmienna przyjmuje 1, gdy kafelek należy teraz złapać i wraca do 0 po złapaniu, a jeśli nie złapie się w porę to zmienia się w 2, co spowoduje przegraną
int stanLapania[2] = {0, 0};

// tyle razy można za wcześnie wcisnąć przycisk, później przegrywa się grę
int zycia = 10;

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

void main(void) {
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

    // zaladowanie wlasnych znakow do CGRAMu
    CREATE_CHAR(0, lapanie);
    CREATE_CHAR(1, zlapany);
    CREATE_CHAR(2, kafek);

    // powrót kursora do DDRAMu
    SET_CURSOR(1, 1);


    // inicjalizacja zegara
    BCSCTL1 |= XTS; // ACLK = LFXT1 = HF XTAL 8MHz

    do {
        IFG1 &= ~OFIFG; // Czyszczenie flgi OSCFault
        for (int i = 0xFF; i > 0; i--); // odczekanie
        // SEND_CHAR('X');
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
    TACTL &= ~MC_1;

    menu(&rozpoczeta);

    // nieskończona pętla, obsługa przycisków w trakcie gry
    while (1) {
        if (rozpoczeta) {
            if (!(PRZYCISK1) && przycisk1Niewcisniety) {
                przycisk1Niewcisniety = 0;
                if (czyZlapany(1)) {
                    SET_CURSOR(1, 1);
                    SEND_CHAR(0);
                    changeScore(10);
                    if (stanLapania[0] == 1) stanLapania[0] = 0;
                } else {
                    changeScore(-5);
                    zycia--;
                }
            }
            if (!(PRZYCISK2) && przycisk2Niewcisniety) {
                przycisk2Niewcisniety = 0;
                if (czyZlapany(2)) {
                    SET_CURSOR(1, 2);
                    SEND_CHAR(0);
                    changeScore(10);
                    if (stanLapania[1] == 1) stanLapania[1] = 0;
                } else {
                    changeScore(-5);
                    zycia--;
                }
            }

                // do testow zatrzymywanie
                //
            else if (!(PRZYCISK3)) {
                TACTL &= ~MC_1;
            } else if (!(PRZYCISK4)) {
                TACTL |= MC_1;
            }
            //    -----------------------
        }
    }
}

//sprawdza czy pole łapania w podanej linii ma w sobie kafelek
int czyZlapany(int row) {
    if (tablicaLCD[row - 1][0] == 1) {
        return 1;
    } else {
        return 0;
    }
}

// losowanie lokalizacji kolejnego kafelka/kafelków lub ich braku
int los;

// przerwania zegara
#pragma vector = TIMERA0_VECTOR

__interrupt void Timer_A(void) {
    int r, c;

    // licznik do spowolnienia, z tym sie pobawimy troche
    licz++;
    if (licz == 8) {
        licz = licz % 8;
        // wpierw operacje na pierwszych dwoch kolumnach
        for (r = 0; r < 2; r++) {
            // kafelek spierdolil, gejmower
            if (stanLapania[r] == 1) {
                stanLapania[r] = 2;
            }

            // ustawienie odpowiedniego znaku pola lapania(z kafelkiem lub bez)
            if (tablicaLCD[r][1] == 2) {
                tablicaLCD[r][0] = 1;
                tablicaLCD[r][1] = -1;
                stanLapania[r] = 1;
            } else {
                tablicaLCD[r][0] = 0;
            }
        }
        // potem przesuwa kafelki
        for (r = 0; r < 2; r++) {
            for (c = 2; c < 16; c++) {
                if (tablicaLCD[r][c] == 2) {
                    tablicaLCD[r][c - 1] = 2;
                    tablicaLCD[r][c] = -1;
                }
            }
        }
        los = rand() % 8;
        switch (los) // 4/8 razy pojedynczy kafelek, 1/8 razy podwojny, 3/8 razy nic
        {
            case 1:
            case 2:
                tablicaLCD[0][15] = 2;
                break;
            case 3:
            case 4:
                tablicaLCD[1][15] = 2;
                break;
            case 5:
                tablicaLCD[0][15] = 2;
                tablicaLCD[1][15] = 2;
                break;
            default:
                break;
        }

        // a na koniec
        moveTiles(tablicaLCD);
        przycisk1Niewcisniety = 1;
        przycisk2Niewcisniety = 1;

        // obsługa sytuacji kończących grę
        if (zycia <= 0 || stanLapania[0] == 2 || stanLapania[1] == 2) {
            stanLapania[0] = 0;
            stanLapania[1] = 0;
            zycia = 10;
            resetTablicy();
            TACTL &= ~MC_1;
            endOfGame(&rozpoczeta);
        }
    }
}

void resetTablicy() {
    int x, z;
    for (x = 0; x < 2; x++) {
        tablicaLCD[x][0] = 0;
        for (z = 1; z < 16; z++) {
            tablicaLCD[x][z] = -1;
        }
    }
}