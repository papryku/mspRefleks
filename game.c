#include <msp430x14x.h>
#include "lcd.h"
#include "portsLcd.h"
#include "game.h"

/** @file */

#ifndef BUTTONS

#define BUTTONS //!< flaga powstrzymująca program od wielokrotnego definiowania przycisków
#define BUTTON1 (BIT4 & P4IN) //!< definicja pierwszego przycisku od lewej
#define BUTTON2 (BIT5 & P4IN) //!< definicja drugiego przycisku od lewej
#define BUTTON3 (BIT6 & P4IN) //!< definicja trzeciego przycisku od lewej
#define BUTTON4 (BIT7 & P4IN) //!< definicja czwartego przycisku od lewej

#endif

/**
 * @brief funkcja odpowiadająca za przygotowanie wyświetlacza do gry, odliczanie i odpauzowanie TimerA
 */
void startGame() {
    // pauza interrupta
    //TACTL &= ~MC_1;
    SEND_CMD(CLR_DISP);
    SEND_CMD(CUR_OFF); // wylaczenie kursora
    SEND_CHAR(0);
    SEND_CMD(DD_RAM_ADDR2); // czyli SET_CURSOR(1,2)
    SEND_CHAR(0);

    for (int i = 3; i > 0; i--) {
        SET_CURSOR(8, 1);
        SEND_CHAR(48 + i);
        SEND_CHAR('.');
        SEND_CHAR('.');
        DelayB(100);
    }

    SET_CURSOR(8, 1);
    SEND_CHAR(' ');
    SEND_CHAR(' ');
    SEND_CHAR(' ');

    TACTL |= MC_1;

    // juz powinny leciec sobie tyntyryn
}

/**
 * @brief wypisuje podaną jako parametr tablicę na LCD
 * @param LCD_array tablica przechowująca reprezentację ekranu gry
 */
void moveTiles(int LCD_array[][16]) {
    int r;
    int c;
    for (r = 0; r < 2; r++) {
        SET_CURSOR(1, r + 1);
        for (c = 0; c < 16; c++) {
            if (LCD_array[r][c] >= 0) {
                SEND_CHAR(LCD_array[r][c]);
            } else {
                SEND_CHAR(' ');
            }
        }
    }
}