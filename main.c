#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include "menu.h"
#include "gra.h"
#include <stdlib.h>
#include <string.h>

#ifndef BUTTONS
#define BUTTONS
#define BUTTON1 (BIT4 & P4IN)
#define BUTTON2 (BIT5 & P4IN)
#define BUTTON3 (BIT6 & P4IN)
#define BUTTON4 (BIT7 & P4IN)
#endif

int gameStatus = 0;

int isCaught(int row);

void resetArray();

// zmienna przyjmuje 1, gdy kafelek należy teraz złapać i wraca do 0 po złapaniu, a jeśli nie złapie się w porę to zmienia się w 2, co spowoduje przegraną
int catchStatus[2] = {0, 0};

// tyle razy można za wcześnie wcisnąć przycisk, później przegrywa się grę
int lives = 10;

// reprezentacja naszego ekranu LCD gry, która jest wypisywana co interwał na ekran
int LCD_array[2][16] = {
        {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

// spowolnienie TimerA
int counter = 0;

// powstrzymuje problemy z "przytrzymywaniem" przycisku, zezwala na jedno wciśnięcie przycisku na interwał
int BUTTON1UNUSED = 1;
int BUTTON2UNUSED = 1;

// reprezentacje liczbowe naszych znaków własnych
char catching[8] = {31, 17, 17, 17, 17, 17, 17, 31};
char tile[8] = {4, 4, 4, 4, 4, 4, 4, 4};
char caught[8] = {31, 21, 21, 21, 21, 21, 21, 31};

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
    CREATE_CHAR(0, catching);
    CREATE_CHAR(1, caught);
    CREATE_CHAR(2, tile);

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

    menu(&gameStatus);

    // nieskończona pętla, obsługa przycisków w trakcie gry
    while (1) {
        if (gameStatus) {
            if (!(BUTTON1) && BUTTON1UNUSED) {
                BUTTON1UNUSED = 0;
                if (isCaught(1)) {
                    SET_CURSOR(1, 1);
                    SEND_CHAR(0);
                    changeScore(10);
                    if (catchStatus[0] == 1) catchStatus[0] = 0;
                } else {
                    changeScore(-5);
                    lives--;
                }
            }
            if (!(BUTTON2) && BUTTON2UNUSED) {
                BUTTON2UNUSED = 0;
                if (isCaught(2)) {
                    SET_CURSOR(1, 2);
                    SEND_CHAR(0);
                    changeScore(10);
                    if (catchStatus[1] == 1) catchStatus[1] = 0;
                } else {
                    changeScore(-5);
                    lives--;
                }
            }

                // do testow zatrzymywanie
                //
            else if (!(BUTTON3)) {
                TACTL &= ~MC_1;
            } else if (!(BUTTON4)) {
                TACTL |= MC_1;
            }
            //    -----------------------
        }
    }
}

//sprawdza czy pole łapania w podanej linii ma w sobie kafelek
int isCaught(int row) {
    if (LCD_array[row - 1][0] == 1) {
        return 1;
    } else {
        return 0;
    }
}

// przerwania zegara
#pragma vector = TIMERA0_VECTOR

__interrupt void Timer_A(void) {
    int r, c;

    // licznik do spowolnienia, z tym sie pobawimy troche
    counter++;
    if (counter == 8) {
        counter = counter % 8;
        // wpierw operacje na pierwszych dwoch kolumnach
        for (r = 0; r < 2; r++) {
            // kafelek spierdolil, gejmower
            if (catchStatus[r] == 1) {
                catchStatus[r] = 2;
            }

            // ustawienie odpowiedniego znaku pola lapania(z kafelkiem lub bez)
            if (LCD_array[r][1] == 2) {
                LCD_array[r][0] = 1;
                LCD_array[r][1] = -1;
                catchStatus[r] = 1;
            } else {
                LCD_array[r][0] = 0;
            }
        }
        // potem przesuwa kafelki
        for (r = 0; r < 2; r++) {
            for (c = 2; c < 16; c++) {
                if (LCD_array[r][c] == 2) {
                    LCD_array[r][c - 1] = 2;
                    LCD_array[r][c] = -1;
                }
            }
        }
        switch (rand() % 8) // 4/8 razy pojedynczy kafelek, 1/8 razy podwojny, 3/8 razy nic
        {
            case 1:
            case 2:
                LCD_array[0][15] = 2;
                break;
            case 3:
            case 4:
                LCD_array[1][15] = 2;
                break;
            case 5:
                LCD_array[0][15] = 2;
                LCD_array[1][15] = 2;
                break;
            default:
                break;
        }

        // a na koniec
        moveTiles(LCD_array);
        BUTTON1UNUSED = 1;
        BUTTON2UNUSED = 1;

        // obsługa sytuacji kończących grę
        if (lives <= 0 || catchStatus[0] == 2 || catchStatus[1] == 2) {
            catchStatus[0] = 0;
            catchStatus[1] = 0;
            lives = 10;
            resetArray();
            TACTL &= ~MC_1;
            endOfGame(&gameStatus);
        }
    }
}

void resetArray() {
    int x, z;
    for (x = 0; x < 2; x++) {
        LCD_array[x][0] = 0;
        for (z = 1; z < 16; z++) {
            LCD_array[x][z] = -1;
        }
    }
}