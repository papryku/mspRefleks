#include "msp430x14x.h" //jak patrze na bledy to ile wywala chyba jednak nie jest zbyt dobrze xDD
#include "portyLcd.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "uart.h"
#include "string.h"

int numberOfScores = 0;
int currentLetter = 'A';     // wartosc pierwszego znaku w tablicy ASCII
// Struktura przechowujqca wyniki z gry

struct Score {
    char name[2];
    int points;
};


char readChar() {
    currentLetter = 65;

    bool button1 = !(P4IN & BIT4);      // zdefiniowanie boola okreslajacego klikniecie przycisku pierwszego
    bool button2 = !(P4IN & BIT5);      // zdefiniowanie boola okreslajacego klikniecie przycisku drugiego
    bool button3 = !(P4IN & BIT6);      // zdefiniowanie boola okreslajacego klikniecie przycisku trzeciego

    if (button1) {
        Delayx100us(200);                           // opoznienie
        clearDisplay();                             // wyczyszczenie wyswietlacza

        if (currentLetter > 65) {                    // sprawdzenie czy obecna wartosc znaku ASCII nie będzie ponizej 'A'
            SEND_CHAR(--currentLetter);              // po jego dekrementacji
        }
    }

    if (button2) {
        Delayx100us(200);                           // opoznienie
        UartCharTransmit(currentLetter);             // wyslanie obecnej litery do terminmala
        return currentLetter;
    }

    if (button3) {
        Delayx100us(200);                           // opoznienie
        clearDisplay();                             // wyczyszczenie wyswietlacza
        if (currentLetter < 90) {                    // sprawdzenie czy obecna wartosc znaku ASCII nie będzie powyzej 'Z'
            SEND_CHAR(++currentLetter);              // po jego inkrementacji
        }
    }
}

char *podajInicjaly(){
    char name[2];
    name[0] = readChar();
    name[1] = readChar();
    return name;
}

// Tablica wynik�w
struct Score scores[10];

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Disable watchdog timer
    P4DIR &= ~BIT4;             // zainicjalizowanie pierwszego przycisku
    P4DIR &= ~BIT5;             // zainicjalizowanie drugiego przycisku
    P4DIR &= ~BIT6;             // zainicjalizowanie trzeciego przycisku
    P4DIR &= ~BIT7;             // zainicjalizowanie czwartego przycisku
    WDTCTL = WDTPW + WDTHOLD;   // wylaczenie WDT

    InitPortsLcd();             // inicjalizacja portow LCD
    InitLCD();                  // inicjalizacja LCD
    clearDisplay();             // czyszczenie wyswietlacza
    InitUart(1200);             // inicjalizacja UARTa predkosci transmisji 2400 b/s

    _EINT();                    // wylaczenie przerwan

    // Wysylanie tablicy wynikow do komputera
    int i;
    for (i = 0; i < 10; i++) {
        char buffer[64];
        SEND_CHAR(scores[i].name[0]);
        SEND_CHAR(scores[i].name[1]);
        SEND_CHAR()
        putc(buffer, "%s: %d points\n", scores[i].name, scores[i].points);
        int j;
        for (j = 0; j < strlen(buffer); j++) {
            while (!(UCA0IFG & UCTXIFG));
            UCA0TXBUF = buffer[j];
        }
    }

    return 0;
}

void sendCharsToTerminal(char c[]){
    for(int i = 0; i < sizeof(c); i++){
        SEND_CHAR(c[i]);
    }
}

void sortScores(struct Score scores[], int n) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        // Szukanie najwi?kszego elementu w tablicy
        int max_idx = i;
        for (j = i + 1; j < n; j++) {
            if (scores[j].points > scores[max_idx].points) {
                max_idx = j;
            }
        }

        // Zamiana miejscami najwi?kszego elementu z elementem na pozycji i
        struct Score temp = scores[i];
        scores[i] = scores[max_idx];
        scores[max_idx] = temp;
    }
}

void addScore(struct Score scores[], int arraySize, struct Score newScore) {
    int index = findEmptyIndex(scores[], arraySize);
    numberOfScores++;
    if (index <= 9) {
        scores[index] = newScore;
    }
    if (newScore.points > scores[9].points) {
        scores[9] = newScore
    }
    sortScores(scores, arraySize);
}

int findEmptyIndex(struct Score scores[], int arraySize) {
    for (int i = 0; i < arraySize; i++) {
        if (scores[i].points == 0) {
            return i;
        }
    }
    return 0;
}
void printScores(struct Score scores[], int n) {
    int i;
    char points_str[10];
    for (i = 1; i <= n; i++) {
        sendCharsToTerminal(scores[i].name);
        SEND_CHAR('.');
        SEND_CHAR(' ');
        points_str = (char)scores[i].points;
        sendCharsToTerminal(points_str[]);
    }
}

void endOfTheGame(int result){
    if(result){
        sendCharsToTerminal("Wygrales! ");
        //sendCharsToTerminal("Twoj wynik to: ");
        printScores(scores,numberOfScores);
    }
}



void sendScores(struct Score scores[], int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        int j;
        for (j = 0; j < strlen(scores[i].name); j++)
        {
            // Wys?anie znaku imienia gracza
            while (!(UCA0IFG & UCTXIFG));
            UCA0TXBUF = scores[i].name[j];
        }

        // Wys?anie znaku dwukropka i spacji
        while (!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = ':';
        while (!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = ' ';

        // Konwersja punkt�w na tekst i wyslanie ich do programu Putty
        char points_str[10];
        putc(points_str, "%d", scores[i].points);
        for (j = 0; j < strlen(points_str); j++)
        {
            while (!(UCA0IFG & UCTXIFG));
            UCA0TXBUF = points_str[j];
        }

        // Wys?anie znaku nowej linii
        while (!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = '\n';
    }
}
