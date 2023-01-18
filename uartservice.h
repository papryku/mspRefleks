//macie zmienne w uartservice.c, nie można ich przekazywać do maina w ten sposób także header dorabiam (nie pytajcie, też nic nie wiem)
#include "msp430x14x.h" //jak patrze na bledy to ile wywala chyba jednak nie jest zbyt dobrze xDD
#include "portyLcd.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uart.h"
#include <string.h>
#include "math.h"

int numberOfScores = 0;
int currentLetter = 'A'; // wartosc pierwszego znaku w tablicy ASCII
// Struktura przechowujqca wyniki z gry

struct Score {
    char name[2];
    int points;
};

char readChar();
//metoda wysylajaca tablice charow na terminal
//wywala jakis blad


//metoda wysylajaca tablice charow na plytke LCD
void sendCharsToLCD(char* c[]);

char *podajInicjaly();

// Tablica wynikow
// struct Score scores[10]; 

// int main(void); tutaj nawet nie chce tlumaczyc

//metoda sortujaca tablice wynikow
void sortScores(struct Score scores[], int n);

//metoda tworzaca nowy rekord po rozegraniu gry
struct Score createScore(char name[], int points);

//metoda dodajaca nowy rekord do tablicy wynikow
void addScore(struct Score scores[], int arraySize, struct Score newScore);

char* intToCharArray(int num);

//metoda wypisujaca tablice wynikow do terminala
void printScores(struct Score scores[], int n);

//metoda wyswietlajaca na LCD zwyciestwo lub przegrana
void endOfTheGame(bool isWin, int result);
void menu();

