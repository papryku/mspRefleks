#include "msp430x14x.h" //jak patrze na bledy to ile wywala chyba jednak nie jest zbyt dobrze xDD
#include "portyLcd.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uart.h"
#include <string.h>
#include "math.h"
#include "uartservice.h"

int numberOfScores = 0;
int currentLetter = 'A'; // wartosc pierwszego znaku w tablicy ASCII

// pobierane jest z uartservice.h
// Struktura przechowujqca wyniki z gry
// struct Score {
//     char name[2];
//     int points;
// };

char readChar() {
    currentLetter = 65;
    bool button1 = !(P4IN & BIT4); // zdefiniowanie boola okreslajacego klikniecie przycisku pierwszego
    bool button2 = !(P4IN & BIT5); // zdefiniowanie boola okreslajacego klikniecie przycisku drugiego
    bool button3 = !(P4IN & BIT6); // zdefiniowanie boola okreslajacego klikniecie przycisku trzeciego

    if (button1) {
        Delayx100us(200); // opoznienie
        clearDisplay(); // wyczyszczenie wyswietlacza

        if (currentLetter > 65) { // sprawdzenie czy obecna wartosc znaku ASCII nie będzie ponizej 'A'
            SEND_CHAR(--currentLetter); // po jego dekrementacji
        }
    }

    if (button2) {
        Delayx100us(200); // opoznienie
        return currentLetter;
    }

    if (button3) {
        Delayx100us(200); // opoznienie
        clearDisplay(); // wyczyszczenie wyswietlacza
        if (currentLetter < 90) { // sprawdzenie czy obecna wartosc znaku ASCII nie będzie powyzej 'Z'
            SEND_CHAR(++currentLetter); // po jego inkrementacji
        }
    }
}
//metoda wysylajaca tablice charow na terminal
//wywala jakis blad


//metoda wysylajaca tablice charow na plytke LCD
void sendCharsToLCD(char* c[]){
    for(int i = 0; i < sizeof(c); i++){
        SEND_CHAR(*c[i]); //wyslanie chara na LCD
    }
}

char *podajInicjaly(){
    char name[2];
    name[0] = readChar(); //uzytkownik wybiera pierwsza litere swojego inicjalu
    name[1] = readChar(); //uzytkownik wybiera druga litere swojego inicjalu
    char send[32]= "utworzono inicjaly";
    UartStringTransmit(send);
    return name;
}

// Tablica wynikow
struct Score scores[10];

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Disable watchdog timer
    P4DIR &= ~BIT4; // zainicjalizowanie pierwszego przycisku
    P4DIR &= ~BIT5; // zainicjalizowanie drugiego przycisku
    P4DIR &= ~BIT6; // zainicjalizowanie trzeciego przycisku
    P4DIR &= ~BIT7; // zainicjalizowanie czwartego przycisku
    WDTCTL = WDTPW + WDTHOLD; // wylaczenie WDT

    InitPortsLcd(); // inicjalizacja portow LCD
    InitLCD(); // inicjalizacja LCD
    clearDisplay(); // czyszczenie wyswietlacza
    InitUart(1200); // inicjalizacja UARTa predkosci transmisji 2400 b/s

    _EINT(); // wylaczenie przerwan
}


//metoda sortujaca tablice wynikow
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

//metoda tworzaca nowy rekord po rozegraniu gry
struct Score createScore(char name[], int points) {
    struct Score newScore;
    strcpy(newScore.name, name);
    newScore.points = points;
    return newScore;
}

//metoda dodajaca nowy rekord do tablicy wynikow
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

char* intToCharArray(int num) {
    static char arr[10];
    sprintf(arr, "%d", num);
    return arr;
}

//metoda wypisujaca tablice wynikow do terminala
void printScores(struct Score scores[], int n) {
    int i;
    for (i = 1; i <= n; i++) {
//bledy
        UartStringTransmit(scores[i].name);
        UartCharTransmit('.');
        UartCharTransmit(' ');
//bledy
        char *points_str = intToCharArray(scores[i].points);
        UartStringTransmit(points_str);
    }
}

//metoda wyswietlajaca na LCD zwyciestwo lub przegrana
void endOfTheGame(bool isWin, int result){
    if(isWin){
        char send[32] ="Wygrales! ";
        sendCharsToLCD(send);
    } else {
        char send2[32] ="Przegrales! ";
        sendCharsToLCD(send2);
    }
    printScores(scores,numberOfScores);
}

void menu() {
    bool button1 = !(P4IN & BIT4); // zdefiniowanie boola okreslajacego klikniecie przycisku pierwszego
    bool button2 = !(P4IN & BIT5); // zdefiniowanie boola okreslajacego klikniecie przycisku drugiego
    bool button3 = !(P4IN & BIT6); // zdefiniowanie boola okreslajacego klikniecie przycisku trzeciego
    bool button4 = !(P4IN & BIT7); // zdefiniowanie boola okreslajacego klikniecie przycisku czwartego
    char nowaGra[32] = "1. Nowa gra.";
    char wyswietl[32] = "2. Wyswietl wyniki.";
    char zakoncz[32] = "3. Zakoncz gre.";
    UartStringTransmit(nowaGra);
    UartStringTransmit(wyswietl);
    UartStringTransmit(zakoncz);

    if (button1) {
        Delayx100us(200); // opoznienie
        clearDisplay(); // wyczyszczenie wyswietlacza
// start new game
        SEND_CHARS("Podaj inicjaly");
        char *inicjaly = podajInicjaly();
        int wynik = 0;//wynik z skadstam
        struct Score sc = createScore(inicjaly, wynik);
        addScore(scores, numberOfScores, sc);
        clearDisplay();
        //endOfTheGame(wygrales, wynik);
    }

    if (button2) {
        Delayx100us(200); // opoznienie
        clearDisplay(); // wyczyszczenie wyswietlacza
//drukowanie wynikow
        printScores(scores,numberOfScores);
    }

    if (button3) {
        Delayx100us(200); // opoznienie
        clearDisplay(); // wyczyszczenie wyswietlacza
        char send[32] = "Koniec gry!";
        UartStringTransmit(send);
//wyjscie z gry
    }
}
