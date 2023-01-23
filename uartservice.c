#include <msp430x14x.h> 
#include "portyLcd.h"
#include "lcd.h"
#include "uart.h"
#include "uartservice.h"
#include "gra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int wynikAktualnej = 0;
char inicjalyAktualnej[2];

char readChar()
{
    gotoSecondLine();
    currentLetter = 65;
    SEND_CHAR('A');
    while(1)
    {

        if (!(PRZYCISK1))
        {
            DelayB(100);
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
            DelayB(100);
            clearDisplay();
            return currentLetter;
        }

        if (!(PRZYCISK2))
        {
            DelayB(100);
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
    DelayB(50);
    gotoSecondLine();
    SEND_CHAR(name[0]);
    name[1] = readChar();
    SEND_CHARS("Podaj inicjaly:");
    gotoSecondLine();
    clearDisplay();
    SEND_CHARS("Utworzono gracza");
    gotoSecondLine();
    SEND_CHAR(name[0]);
    SEND_CHAR(name[1]);

    DelayB(10000);
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
void sortScores(int n)
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
    Score newScore;
    strcpy(newScore.name, name);
    newScore.points = points;
    return newScore;
}

//metoda dodajaca nowy rekord do tablicy wynikow
void addScore(struct Score newScore)
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
//metoda wypisujaca tablice wynikow do terminala
void printScores()
{
    for (int i = 0; i <= numberOfScores-1; i++)
    {
//bledy
      clearDisplay();
        SEND_CHARS(scores[i].name);
        SEND_CHAR(':');
//bledy
        SEND_NUMBERS(scores[i].points);
        gotoSecondLine();

        /*SEND_CHARS(scores[i + 1].name);
        SEND_CHAR(':');
//bledy
        char *points_str2 = intToCharArray(scores[i + 1].points);
        SEND_CHARS(points_str2);
        clearDisplay();
        DelayB(10000);*/
    }
}

//metoda wyswietlajaca na LCD zwyciestwo lub przegrana\
//deprecated
void endOfTheGame(int isWin, int result) //nie wiem co z tym resultem
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
    printScores(scores);
}

void menu()
{
  //Score patryk;
  //patryk.name[0] = 'P';
  //patryk.name[1] = 'U';
  //patryk.points = 5;
  //numberOfScores++;
  //addScore( patryk);
  //addScore(createScore("ZM", 15));
 // printScores();
    int b = 0;
    SEND_CHARS("1.Nowa gra.");
    gotoSecondLine();
    SEND_CHARS("2.Wyswietl wyniki.");
    while(b==0) //podejscie z returnem w niesk. petli imo jest bardziej naturalne, ale to tez git
    {
        if (!(PRZYCISK1))
        {
            Delayx100us(200); // opoznienie
            clearDisplay(); // wyczyszczenie wyswietlacza
// start new game
            SEND_CHARS("Podaj inicjaly");

    //w skrócie chcę mieć globalne inicjały i wynik w uartservice, które będą zmieniały się co rozpoczęcie gry
    //potem w każdej potrzebnej funkcji będzie można wykorzystać te wartości zamiast się bawić we wskaźniki
            //char *inicjaly = podajInicjaly();
            strcpy(inicjalyAktualnej, podajInicjaly()); //oby to dzialalo
            
            //rozpoczyna gre (gra.c)
            rozpocznijGre();

            int wynik = 0;//wynik z skadstam
            struct Score sc = createScore(inicjalyAktualnej, wynikAktualnej);
            addScore(sc);
            //clearDisplay();
//endOfTheGame(wygrales, wynik);
            b=1;
        }

        if (!(PRZYCISK2))
        {
            DelayB(100); // opoznienie
            clearDisplay(); // wyczyszczenie wyswietlacza
//drukowanie wynikow
            printScores(scores);
            b=1;
        }

    }
}


//przerzucam swoje z gry.c zeby nie bawic sie w rzucanie wskaznikiem do inicjalow
void koniecGry(){
    //pauza timerA
    TACTL &= ~MC_1;
    SEND_CMD(CLR_DISP);
    SET_CURSOR(4,1);
    SEND_CHARS("PRZEGRALES");
    SET_CURSOR(2,2);
    SEND_CHARS(inicjalyAktualnej);
    SEND_CHARS(", WYNIK:");
    SEND_NUMBER(wynikAktualnej);

    DelayB(200);
    //dopoki nie jest wcisniety
    while(1){
      if(!(PRZYCISK1)||!(PRZYCISK2)||!(PRZYCISK3)||!(PRZYCISK4)){
        SEND_CMD(CLR_DISP);
        //mam nadzieje, ze to juz zapewni tak naprawde mozliwosc ponownego zagrania
        menu();
      }
    }
}