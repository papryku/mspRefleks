#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include "gra.h"

#ifndef BUTTONS
#define BUTTONS
#define BUTTON1 (BIT4 & P4IN)
#define BUTTON2 (BIT5 & P4IN)
#define BUTTON3 (BIT6 & P4IN)
#define BUTTON4 (BIT7 & P4IN)
#endif

/**
 * @brief funkcja odpowiadajaca ustawieni kursorow i przygotowanie wyswietlacza do przeprowadzenia rozgrywki
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
 * @brief przesuwanie kafelka po wyswietlaczu
 * @param tableLCD tablica przechowujaca strukture wyswietlacza
 */
void moveTiles(int tableLCD[][16]) {
    int r;
    int c;
    for (r = 0; r < 2; r++) {
        SET_CURSOR(1, r + 1);
        for (c = 0; c < 16; c++) {
            if (tableLCD[r][c] >= 0) {
                SEND_CHAR(tableLCD[r][c]);
            } else {
                SEND_CHAR(' ');
            }
        }
    }
}