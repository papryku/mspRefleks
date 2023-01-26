#include <msp430x14x.h>
#include "lcd.h"
#include "portsLcd.h"
#include "menu.h"
#include "game.h"
#include <stdlib.h>
#include <string.h>

/** @file */

#ifndef BUTTONS

#define BUTTONS //!< flaga powstrzymująca program od wielokrotnego definiowania przycisków
#define BUTTON1 (BIT4 & P4IN) //!< definicja pierwszego przycisku od lewej
#define BUTTON2 (BIT5 & P4IN) //!< definicja drugiego przycisku od lewej
#define BUTTON3 (BIT6 & P4IN) //!< definicja trzeciego przycisku od lewej
#define BUTTON4 (BIT7 & P4IN) //!< definicja czwartego przycisku od lewej

#endif

/**
 * @brief zmienna przechowuje informację, czy gra aktualnie trwa, czy nie
 * @brief wynosi 0, gdy gra nie jest w toku (menu, tabela wyników, odliczanie, ekran porażki)
 * @brief wynosi 1, gdy gra jest w toku
 */
int gameStatus = 0;

/**
 * @brief funkcja sprawdza, czy w polu łapania danego rzędu znajduje się kafelek 
 * @param row rząd, który ma sprawdzić 
 * @retval 0 jeśli nie ma tam kafelka
 * @retval 1 jeśli jest tak kafelek
 */
int isCaught(int row);

/**
 * @brief funkcja przywraca wartość początkową tablicy LCD_array, czyli pola łapania(0) w pierwszej kolumnie i reszta puste znaki(-1)
 * @see LCD_array
 */
void resetArray();

/**
 * @brief zmienna jest tablicą przechowującą status obu rzędów LCD
 * @brief wynosi 0, gdy kafelka nie trzeba łapać lub wprost po złapaniu
 * @brief wynosi 1, gdy kafelek należy teraz złapać 
 * @brief wynosi 2, gdy kafelka nie złapie się w porę, co powoduje przegraną
 */
int catchStatus[2] = {0, 0};

// tyle razy można za wcześnie wcisnąć przycisk, później przegrywa się grę
/**
 * @brief zmienna przechowuje pozostałą dozwoloną ilość nietrafionych kliknięć (żyć)
 * @brief po osiągnięciu 0 powoduje przegraną, przy rozpoczęciu gry wraca do 10
 */
int lives = 10;

/**
 * @brief reprezentacja naszego ekranu gry, która później jest wypisywana co interwał na ekran
 * @brief 0 to pole łapania, 1 to pole łapania z kafelkiem w środku, 2 to kafelek, -1 to pusta komórka
 */
int LCD_array[2][16] = {
        {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

/**
 * @brief licznik spowalniający wykonywanie operacji w przerwaniach TimerA
 */
int counter = 0;

/**
 * @brief powstrzymuje problemy z "przytrzymywaniem" pierwszego przycisku, zezwala na jedno wciśnięcie owego przycisku na interwał
 * @brief 1 oznacza, że można wcisnąć, 0 blokuje wciskanie
 */
int BUTTON1UNUSED = 1;
/**
 * @brief powstrzymuje problemy z "przytrzymywaniem" drugiego przycisku, zezwala na jedno wciśnięcie owego przycisku na interwał
 * @brief 1 oznacza, że można wcisnąć, 0 blokuje wciskanie
 */
int BUTTON2UNUSED = 1;

char catching[8] = {31, 17, 17, 17, 17, 17, 17, 31}; //!<reprezentacja liczbowa znaku własnego pola łapania
char tile[8] = {4, 4, 4, 4, 4, 4, 4, 4};//!<reprezentacja liczbowa znaku własnego kafelka
char caught[8] = {31, 21, 21, 21, 21, 21, 21, 31};//!<reprezentacja liczbowa znaku własnego pola łapania z kafelkiem w środku

/**
 * @brief główny program, wpierw inicjalizuje używane układy, następnie wywołuje menu(), później trwa nieskończona pętla z obsługą przycisków 
 * @brief
 * @brief najpierw wyłączany jest watchdog, następnie inicjalizowane są przyciski, kolejno inicjalizowany jest LCD
 * @brief później tworzone są własne znaki i inicjalizowany jest TimerA po czym wywoływane jest menu() 
 * @see InitPortsLcd()
 * @see InitLCD()
 * @see menu(int *gameStatus)
 */
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
    TACTL = TASSEL_1 + MC_1 + ID_3; 
    CCTL0 = CCIE;                   
    CCR0 = 500000;                  

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
                    changeScore(0);
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
                    changeScore(0);
                    lives--;
                }
            }

                // do testow, pauzowanie
                //
            // else if (!(BUTTON3)) {
            //     TACTL &= ~MC_1;
            // } else if (!(BUTTON4)) {
            //     TACTL |= MC_1;
            // }
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

/**
 * @brief instrukcje wykonywane przy przerwaniach spowodowanych TimerA
 * @brief wpierw sprawdzenie catchStatus dla obu rzędów, następnie ustawienie odpowiedniego znaku pola łapania(z kafelkiem lub bez),
 * @brief następnie przesunięcie wszystkich pozostałych kafelków w tablicy, generowanie nowych i wywolanie moveTiles(),
 * @brief na koniec przywrócenie BUTTONxUNUSED do wartości 1 i obsługa sytuacji kończących grę
 * @brief przy lives==0 lub catchStatus[x]==2 kończy grę resetując stan zmiennych i wywołując endOfGame()
 *  
 * @see catchStatus
 * @see LCD_array
 * @see moveTiles(int LCD_array[][16])
 * @see BUTTON1UNUSED
 * @see BUTTON2UNUSED
 * @see lives
 * @see resetArray()
 * @see endOfGame(int * gameStatus)
 */
__interrupt void Timer_A(void) {
    counter++; //licznik do spowolnienia
    if (counter == 8) {
        int r, c;
        counter = counter % 8;
        // wpierw operacje na pierwszych dwoch kolumnach
        for (r = 0; r < 2; r++) {
            // kafelek uciekł, gejmower
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
        // potem przesuwa kafelki w tablicy
        for (r = 0; r < 2; r++) {
            for (c = 2; c < 16; c++) {
                if (LCD_array[r][c] == 2) {
                    LCD_array[r][c - 1] = 2;
                    LCD_array[r][c] = -1;
                }
            }
        }
        // potem generuje(bądź nie) nowy kafelek lub kafelki
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

        // a na koniec wypisuje tablicę na LCD
        moveTiles(LCD_array);

        // przywraca możliwość wciskania przycisków
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