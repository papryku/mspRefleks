#include <msp430x14x.h>
#include "portyLcd.h"
#include "lcd.h"
#include "menu.h"
#include "gra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PRZYCISKI
#define PRZYCISKI
#define PRZYCISK1 (BIT4 & P4IN)
#define PRZYCISK2 (BIT5 & P4IN)
#define PRZYCISK3 (BIT6 & P4IN)
#define PRZYCISK4 (BIT7 & P4IN)
#endif


int numberOfScores = 0;
int currentLetter = 'A'; // wartosc pierwszego znaku w tablicy ASCII
int wynikAktualnej = 0; int iloscKlikniec = 0; int iloscZlapanych = 0;
char inicjalyAktualnej[2];

/**
 * @brief modyfikuje wynik aktualnej gry
 * @param num przekazuje nowy wynik
 */
void changeScore(int num)
{
    wynikAktualnej = wynikAktualnej + num;
    if (wynikAktualnej < 0)
    {
        wynikAktualnej = 0;
    }
    if(num>0){ //próba implementacji accuracy zeby nam mnie punktow ujebal, zamiast odejmowania jest zmniejszany
        iloscKlikniec++; iloscZlapanych++;
    }else{
        iloscKlikniec++;
    }
}
/**
 * @brief funkcja pozwalająca wybrać literę
 * @return wybrana litere
 */
char readChar()
{
    gotoSecondLine();
    currentLetter = 65;
    SEND_CHAR('A');
    while (1)
    {

        if (!(PRZYCISK1)) //zmiejszenie aktualnej litery
        {
            DelayB(100);
            if (currentLetter > 65)
            {
                clearDisplay();
                SEND_CHARS("Podaj inicjaly:");
                gotoSecondLine();           // sprawdzenie czy obecna wartosc znaku ASCII nie będzie ponizej 'A'
                SEND_CHAR(--currentLetter); // po jego dekrementacji
            }
        }

        if (!(PRZYCISK3)) //zwrocenie atualnej litery
        {
            DelayB(100);
            clearDisplay();
            return currentLetter;
        }

        if (!(PRZYCISK2))  // zmieniejszenie aktualnej litery
        {
            DelayB(100);
            if (currentLetter < 90)
            {
                clearDisplay();
                SEND_CHARS("Podaj inicjaly:");
                gotoSecondLine();           // sprawdzenie czy obecna wartosc znaku ASCII nie będzie powyzej 'Z'
                SEND_CHAR(++currentLetter); // po jego inkrementacji
            }
        }
    }
}
/**
 * @brief funkcja pozwalająca wybrać inicjały, które później zwraca w tablicy
 * @return tablica inicjalow
 */
char *getInitials()
{
    char name[2];
    name[0] = readChar(); // uzytkownik wybiera pierwsza litere swojego inicjalu
    SEND_CHARS("Podaj inicjaly:");
    DelayB(50);
    gotoSecondLine();
    SEND_CHAR(name[0]);
    name[1] = readChar(); // uzytkownik wybiera druga litere swojego inicjalu
    SEND_CHARS("Podaj inicjaly:");
    gotoSecondLine();
    clearDisplay();
    SEND_CHARS("Utworzono gracza");
    gotoSecondLine();
    SEND_CHAR(name[0]);
    SEND_CHAR(name[1]);

    DelayB(250);
    return name;
}

// tablica wynikow
struct Score scores[10];


/**
 * @brief funkcja sortujaca tablice wynikow
 * @param n ilosc elementow w talicy wynikow
 */
void sortScores(int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
    {
        // Szukanie najwiekszego elementu w tablicy
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

/**
 * @brief funkcja tworzaca nowy rekord po rozegraniu gry
 * @param name inicjaly zawodnika
 * @param points zdobyte przez niego punkty
 * @return ilosc elementow w tablicy wynikow
 */
struct Score createScore(char name[], int points)
{
    Score newScore;
    strcpy(newScore.name, name);
    newScore.points = points;
    return newScore;
}

/**
 * @brief metoda dodajaca nowy rekord do tablicy wynikow
 * @param newScore nowy wynik
 * @return rekord nowego wyniku
 */

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
    sortScores(numberOfScores);
}

/**
 * @brief metoda wypisujaca tablice wynikow rosnaco
 * @param rozpoczeta status rozgrywki
 */
void printScores(int *rozpoczeta)
{
    int i = 0;
    //for (int i = 0; i < numberOfScores; i++)
    while(1) //wyjście tylko za pomocą przycisku 4, możliwość porzuszania w górę i dół, możliwość pauzy
    {
        // bledy
        clearDisplay();
        SEND_NUMBER(i + 1);
        SEND_CHARS(". miejsce");
        gotoSecondLine();
        //wypisanie inicjalow na aktualnej pozycji
        SEND_CHAR(scores[i].name[0]);
        SEND_CHAR(scores[i].name[1]);
        SEND_CHARS(" : ");
        //wypisanie punktow na aktualnej pozycji
        SEND_NUMBER(scores[i].points);


        //delay wyłapujące w miarę dokładnie przycisk
        //chyba
        for(int j = 0; j < 500; j++){ //DelayB(500);
            if(!(PRZYCISK1)){ //przejscie do kolejnego wyniku
                if(i < numberOfScores){
                    i++;
                } else { i = numberOfScores--; }
                DelayB(50);
                break;
            }
            else if(!(PRZYCISK2)){ //powrot do poprzedniego wyniku
                if(i > 0){
                    i--;
                } else { i = 0; }
                DelayB(50);
                break;
            }
            else if(!(PRZYCISK4)){ //powrot do menu
                menu(rozpoczeta);
                return;
            }
            DelayB(1);
        } i++;
        if(i>=numberOfScores) { i = 0; }
    }
}

/**
 * @brief metoda wyswietlajaca na plytce mozliwe akcje jakie, ktore moze wykonac zawodnik moga za pomoca przyciskow zadecydowac czy chca rozpoczac nowa gre lub wyswielisc dotychczasowe wyniki
 * @param rozpoczeta status rozgrywki
 */

void menu(int *rozpoczeta)
{
    // wylaczenie watchdoga
    WDTCTL = WDTPW + WDTHOLD;
    SEND_CMD(CLR_DISP);
    SEND_CHARS("1.Nowa gra.");
    gotoSecondLine();
    SEND_CHARS("2.Pokaz wyniki.");
    DelayB(100);
    while (1)
    {

        if (!(PRZYCISK1))   // rozpoczecie gry
        {
            DelayB(100);    // opoznienie
            clearDisplay(); // wyczyszczenie wyswietlacza
            // start new game
            SEND_CHARS("Podaj inicjaly:");

            char *inicjaly = getInitials();
            strcpy(inicjalyAktualnej, inicjaly);
            SEND_CMD(CLR_DISP);
            // rozpoczyna gre (gra.c)
            startGame();
            *rozpoczeta = 1;
            return;
        }

        if (!(PRZYCISK2))   // wyswietlenie wynikow
        {
            DelayB(100);    // opoznienie
            clearDisplay(); // wyczyszczenie wyswietlacza
            printScores(rozpoczeta); // drukowanie wynikow
            return;
        }
    }
}


/**
 * @brief funkcja kończąca rozgrywkę, wyświetla końcowy wynik i inicjały, przekazuje je do tabeli wyników i powraca do menu()
 * @param rozpoczeta status rozgrywki
 */
void endOfGame(int *rozpoczeta)
{
    // wylaczenie watchdoga
    WDTCTL = WDTPW + WDTHOLD;
    TACTL &= ~MC_1;
    *rozpoczeta = 0;

    //int zeby potem nie castowac wypisujac
    //ustawienie celnosci jako iloczyn zlapanych kafelkow i iloraz klikniec
    int celnosc = 100.0 * iloscZlapanych / iloscKlikniec;
    int koncowyWynik = (float)wynikAktualnej * (float)celnosc / 100.0;

    SEND_CMD(CLR_DISP);
    if (koncowyWynik == 0){
        SET_CURSOR(1,1);
        SEND_CHARS("PRZEGRYWASZ");
    }else{
        SET_CURSOR(4, 1);
        SEND_CHARS("PRZEGRYWASZ");
    }
    SET_CURSOR(2, 2);
    //wypisanie inicjalow aktualnego gracza
    SEND_CHAR(inicjalyAktualnej[0]);
    SEND_CHAR(inicjalyAktualnej[1]);
    SEND_CHARS(", WYNIK:");
    if(wynikAktualnej==0){
        SEND_CHAR('0');
    }else{
        //wyslanie wyniku koncowego
        SEND_NUMBER(koncowyWynik);
    }
    //utworzenie nowego wyniku w strukturze Score i dodanie go do tablicy wynikow
    if(wynikAktualnej==0){
        struct Score sc = createScore(inicjalyAktualnej, koncowyWynik); addScore(sc);
    }else{
        struct Score sc = createScore(inicjalyAktualnej, wynikAktualnej); addScore(sc);
    }
    wynikAktualnej = 0;
    menu(rozpoczeta);
}
