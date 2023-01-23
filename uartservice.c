#include <msp430x14x.h> //jak patrze na bledy to ile wywala chyba jednak nie jest zbyt dobrze xDD
#include "portyLcd.h"
#include "lcd.h"
#include "uart.h"
#include "uartservice.h"

#ifndef PRZYCISKI
#define PRZYCISKI
#define PRZYCISK1 (BIT4&P4IN)
#define PRZYCISK2 (BIT5&P4IN)
#define PRZYCISK3 (BIT6&P4IN)
#define PRZYCISK4 (BIT7&P4IN)
#endif

//jeśli potrzebne to będzie w operacjach poza biblioteką, to należy zdefiniować jedynie w pliku nagłówkowym i tu zostawić ustawienie wartości bez deklarowania jako int
int numberOfScores = 0;
int currentLetter = 'A'; // wartosc pierwszego znaku w tablicy ASCII

// pobierane jest z uartservice.h
// Struktura przechowujqca wyniki z gry
// struct Score {
// char name[2];
// int points;
// };

char readChar()
{
    gotoSecondLine();
    currentLetter = 65;
    SEND_CHAR('A');
    while(1)
    {
        DelayB(100); //git jest
        if (!(PRZYCISK1))
        {
            if (currentLetter > 65)
            {
                clearDisplay();
                SEND_CHARS("Podaj Inicjaly");
                gotoSecondLine();// sprawdzenie czy obecna wartosc znaku ASCII nie będzie ponizej 'A'
                SEND_CHAR(--currentLetter); // po jego dekrementacji
            }
        }

        if (!(PRZYCISK3))
        {
            clearDisplay();
            return currentLetter;
        }

        if (!(PRZYCISK2))
        {
            if (currentLetter < 90)
            {
                clearDisplay();
                SEND_CHARS("Podaj Inicjaly");
                gotoSecondLine();// sprawdzenie czy obecna wartosc znaku ASCII nie będzie powyzej 'Z'
                SEND_CHAR(++currentLetter); // po jego inkrementacji
            }
        }
    }
}

char *podajInicjaly()
{
    char name[2];
    name[0] = readChar(); //uzytkownik wybiera pierwsza litere swojego inicjalu
    SEND_CHARS("Podaj inicjaly:");
    gotoSecondLine();
    SEND_CHAR(name[0]);
    name[1] = readChar();
    SEND_CHARS("Podaj inicjaly:");
    gotoSecondLine();
    SEND_CHAR(name[0]);
    SEND_CHAR(name[1]);
//uzytkownik wybiera druga litere swojego inicjalu
    SEND_CHARS("utworzono inicjaly");
    return name;
}

// Tablica wynikow
struct Score scores[10];

/*
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
}*/


//metoda sortujaca tablice wynikow
void sortScores(struct Score scores[], int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
    {
// Szukanie najwi?kszego elementu w tablicy
        int max_idx = i;
        for (j = i + 1; j < n; j++)
        {
            if (scores[j].points > scores[max_idx].points)
            {
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
struct Score createScore(char name[], int points)
{
    struct Score newScore;
    strcpy(newScore.name, name);
    newScore.points = points;
    addScore(scores, numberOfScores, newScore);
    return newScore;
}

//metoda dodajaca nowy rekord do tablicy wynikow
void addScore(struct Score scores[], int arraySize, struct Score newScore)
{
    if (numberOfScores <= 9)
    {
        scores[numberOfScores] = newScore;
        numberOfScores++;
    }
    if (newScore.points > scores[9].points)
    {
        scores[9] = newScore;
    }
// sortScores(scores, arraySize);
}

char* intToCharArray(int num)
{
    static char arr[10];
    return arr;
}

//metoda wypisujaca tablice wynikow do terminala
void printScores(struct Score scores[], int n)
{
    for (int i = 0; i <= n - 1; i++)
    {
//bledy
        SEND_CHARS(scores[i].name);
        SEND_CHAR(':');
//bledy
        char *points_str = intToCharArray(scores[i].points);
        SEND_CHARS(points_str);
        gotoSecondLine();

        SEND_CHARS(scores[i + 1].name);
        SEND_CHAR(':');
//bledy
        char *points_str2 = intToCharArray(scores[i + 1].points);
        SEND_CHARS(points_str2);
        clearDisplay();
        Delayx100us(1000);
    }
}

//metoda wyswietlajaca na LCD zwyciestwo lub przegrana
void endOfTheGame(int isWin, int result)
{
    if(isWin)
    {
        char send[32] ="Wygrales! ";
        sendCharsToLCD(send);
    }
    else
    {
        char send2[32] ="Przegrales! ";
        sendCharsToLCD(send2);
    }
    printScores(scores,numberOfScores);
}

void menu()
{
    int b = 0;
    SEND_CHARS("1.Nowa gra.");
    gotoSecondLine();
    SEND_CHARS("2.Wyswietl wyniki.");
    while(b==0)
    {
        if (!(PRZYCISK1))
        {
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
            b=1;
        }

        if (!(PRZYCISK2))
        {
            Delayx100us(200); // opoznienie
            clearDisplay(); // wyczyszczenie wyswietlacza
//drukowanie wynikow
            printScores(scores,numberOfScores);
            b=1;
        }

    }
}
