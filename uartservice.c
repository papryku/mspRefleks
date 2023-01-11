#include "msp430x14x.h" //jak patrze na bledy to ile wywala chyba jednak nie jest zbyt dobrze xDD
#include "portyLcd.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "uart.h"

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

// Tablica wynikow
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

struct Score createScore(char name[], int points) {
    struct Score newScore;
    strcpy(newScore.name, name);
    newScore.points = points;
    return newScore;
}

void addScore(struct Score scores[], int arraySize, struct Score newScore) {
    if (numberOfScores <= 9) {
        scores[numberOfScores] = newScore;
        numberOfScores++;
    }
    if (newScore.points > scores[9].points) {
        scores[9] = newScore;
    }
    sortScores(scores, arraySize);
}

void printScores(struct Score scores[], int n) {
    int i;
    char points_str[10];
    for (i = 1; i <= n; i++) {
        sendCharsToTerminal(scores[i].name);
        SEND_CHAR('.');
        SEND_CHAR(' ');
        sprintf(points_str,"%ld", scores[i].points);
        sendCharsToTerminal(points_str);
    }
}

void endOfTheGame(bool isWin, int result){
    if(isWin){
        sendCharsToTerminal("Wygrales! ");
    } else {
        sendCharsToTerminal("Przegrales! ");
    }
    printScores(scores,numberOfScores);
}
void menu() {
    bool button1 = !(P4IN & BIT4);      // zdefiniowanie boola okreslajacego klikniecie przycisku pierwszego
    bool button2 = !(P4IN & BIT5);      // zdefiniowanie boola okreslajacego klikniecie przycisku drugiego
    bool button3 = !(P4IN & BIT6);      // zdefiniowanie boola okreslajacego klikniecie przycisku trzeciego
    bool button4 = !(P4IN & BIT7);      // zdefiniowanie boola okreslajacego klikniecie przycisku czwartego


    if (button1) {
        Delayx100us(200);                           // opoznienie
        clearDisplay();                             // wyczyszczenie wyswietlacza
        // start new game
    }

    if (button2) {
        Delayx100us(200);                           // opoznienie
        clearDisplay();                             // wyczyszczenie wyswietlacza
        //drukowanie wynikow
        printScores(scores,numberOfScores);
    }

    if (button3) {
        Delayx100us(200);                           // opoznienie
        clearDisplay();                             // wyczyszczenie wyswietlacza
        //wyjscie z gry
    }
}
