#include <msp430x14x.h>
#include "portsLcd.h"
#include "lcd.h"
#include "menu.h"
#include "game.h"
#include <stdio.h>
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

int numberOfScores = 0;  //!< zmienna przechowująca ilość wyników w tabeli wyników
int currentLetter = 'A'; //!< zmienna przechowująca aktualnie wybrany znak przez funkcję readChar()
int currentScore = 0;    //!< zmienna przechowująca wynik aktualnie przeprowadzanej gry
int clicks = 0;          //!< zmienna przechowująca ilość kliknięć(również nietrafionych) w aktualnej grze
int caught = 0;          //!< zmienna przechowująca ilość złapanych kafelków w aktualnej grze
char currentInitials[2]; //!< tablica przechowująca inicjały wybrane do aktualnej gry

/**
 * @brief modyfikuje wynik oraz składowe celności aktualnej gry
 * @param num przekazuje zdobyte punkty, przypadek 0 interpretowany jest jako zbyt wczesne wciśnięcie przycisku
 * @see currentScore
 * @see clicks
 * @see caught
 */
void changeScore(int num) {
    currentScore = currentScore + num;
    if (currentScore < 0) {
        currentScore = 0;
    }
    if (num > 0) {
        clicks++;
        caught++;
    } else {
        clicks++;
    }
}

/**
 * @brief funkcja pozwalająca wybrać literę
 * @brief przycisk pierwszy zmniejsza wartość litery(alfabetycznie w dół)
 * @brief przycisk drugi zwiększa wartość litery(alfabetycznie w górę)
 * @brief przycisk trzeci wybiera aktualną literę
 * @return wybrana litera
 * @see currentLetter
 */
char readChar() {
    gotoSecondLine();
    currentLetter = 65;
    SEND_CHAR('A');
    while (1) {

        if (!(BUTTON1)) //zmiejszenie aktualnej litery
        {
            DelayB(100);
            if (currentLetter > 65) {
                clearDisplay();
                SEND_CHARS("Podaj inicjaly:");
                gotoSecondLine();           // sprawdzenie czy obecna wartosc znaku ASCII nie będzie ponizej 'A'
                SEND_CHAR(--currentLetter); // po jego dekrementacji
            }
        }

        if (!(BUTTON3)) //zwrocenie atualnej litery
        {
            DelayB(100);
            clearDisplay();
            return currentLetter;
        }

        if (!(BUTTON2))  // zmieniejszenie aktualnej litery
        {
            DelayB(100);
            if (currentLetter < 90) {
                clearDisplay();
                SEND_CHARS("Podaj inicjaly:");
                gotoSecondLine();           // sprawdzenie czy obecna wartosc znaku ASCII nie będzie powyzej 'Z'
                SEND_CHAR(++currentLetter); // po jego inkrementacji
            }
        }
    }
}

/**
 * @brief funkcja pozwalająca wybrać inicjały wywołując dwukrotnie readChar()
 * @return tablica z wybranymi inicjałami
 * @see readChar()
 */
char *getInitials() {
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

// tablica wyników
struct Score scores[10]; //!< tablica przechowująca wyniki


/**
 * @brief funkcja sortująca tablice wyników
 * @param n ilość elementów w talicy wyników
 * @see scores
 */
void sortScores(int n) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        // Szukanie najwiekszego elementu w tablicy
        int max_idx = i;
        for (j = i + 1; j < n; j++) {
            if (scores[j].points > scores[max_idx].points) {
                max_idx = j;
            }
        }

        // Zamiana miejscami największego elementu z elementem na pozycji i
        struct Score temp = scores[i];
        scores[i] = scores[max_idx];
        scores[max_idx] = temp;
    }
}

/**
 * @brief funkcja tworząca nowy wynik po rozegraniu gry
 * @param name inicjały zawodnika
 * @param points zdobyte przez niego punkty
 * @return wynik z podanymi inicjałami i punktami
 * @see struct Score
 */
struct Score createScore(char name[], int points) {
    Score newScore;
    strcpy(newScore.name, name);
    newScore.points = points;
    return newScore;
}

/**
 * @brief funkcja dodająca nowy rekord do tablicy wyników i wywołująca sortowanie tablicy sortScores()
 * @param newScore nowy wynik
 * @see scores
 * @see sortScores(int n)
 */

void addScore(struct Score newScore) {
    if (numberOfScores <= 9) {
        scores[numberOfScores] = newScore;
        numberOfScores++;
    }
    if (newScore.points > scores[9].points) {
        scores[9] = newScore;
    }
    sortScores(numberOfScores);
}

/**
 * @brief funkcja wypisująca tablicę wyników
 * @brief przycisk 1 przechodzi do następnego wyniku
 * @brief przycisk 2 powraca do poprzedniego wyniku
 * @brief przycisk 4 powraca do menu()
 * @param gameStatus wskaźnik statusu rozgrywki, do przekazania spowrotem do menu(gameStatus)
 * @see gameStatus
 * @see scores
 * @see menu(int * gameStatus)
 */
void printScores(int *gameStatus) {
    //BRAK OBSLUGI WYJATKU Z BRAKIEM JAKICHKOLWIEK WYNIKOW numberOfScores==0
    int i = 0;
    //for (int i = 0; i < numberOfScores; i++)
    while (1) //wyjście tylko za pomocą przycisku 4, możliwość porzuszania w górę i dół, możliwość pauzy
    {
        // bledy
        clearDisplay();
        SEND_NUMBER(i + 1);
        SEND_CHARS(". miejsce");
        gotoSecondLine();
        //wypisanie inicjałów na aktualnej pozycji
        SEND_CHAR(scores[i].name[0]);
        SEND_CHAR(scores[i].name[1]);
        SEND_CHARS(" : ");
        //wypisanie punktow na aktualnej pozycji
        SEND_NUMBER(scores[i].points);


        //delay wyłapujące w miarę dokładnie przycisk
        //chyba
        for (int j = 0; j < 500; j++) { //DelayB(500);
            if (!(BUTTON1)) { //przejscie do kolejnego wyniku
                if (i < numberOfScores) {
                    i++;
                } else { i = numberOfScores--; }
                DelayB(50);
                break;
            } else if (!(BUTTON2)) { //powrot do poprzedniego wyniku
                if (i > 0) {
                    i--;
                } else { i = 0; }
                DelayB(50);
                break;
            } else if (!(BUTTON4)) { //powrot do menu
                menu(gameStatus);
                return;
            }
            DelayB(1);
        }
        i++;
        if (i >= numberOfScores) { i = 0; }
    }
}

/**
 * @brief funkcja wyswietlajaca na LCD możliwe akcje, które moze wykonać zawodnik 
 * @brief przycisk 1 rozpoczyna nową grę
 * @brief przycisk 2 wyświetla tabelę wyników
 * @param gameStatus wskaźnik statusu rozgrywki, przy rozpoczęciu gry zmieniany jest na 1
 */

void menu(int *gameStatus) {
    // wylaczenie watchdoga
    WDTCTL = WDTPW + WDTHOLD;
    SEND_CMD(CLR_DISP);
    SEND_CHARS("1.Nowa gra.");
    gotoSecondLine();
    SEND_CHARS("2.Pokaz wyniki.");
    DelayB(100);
    while (1) {

        if (!(BUTTON1))   // rozpoczecie gry
        {
            DelayB(100);    // opoznienie
            clearDisplay(); // wyczyszczenie wyswietlacza
            // start new game
            SEND_CHARS("Podaj inicjaly:");

            char *inicjaly = getInitials();
            strcpy(currentInitials, inicjaly);
            SEND_CMD(CLR_DISP);
            // rozpoczyna gre (gra.c)
            startGame();
            *gameStatus = 1;
            return;
        }

        if (!(BUTTON2))   // wyswietlenie wyników
        {
            DelayB(100);    // opoznienie
            clearDisplay(); // wyczyszczenie wyswietlacza
            printScores(gameStatus); // drukowanie wyników
            return;
        }
    }
}


/**
 * @brief funkcja kończąca rozgrywkę, wylicza i wyświetla końcowy wynik oraz inicjały, przekazuje je do tabeli wyników i powraca do menu()
 * @param gameStatus wskaźnik statusu rozgrywki, do przekazania spowrotem do menu(gameStatus)
 * @see menu(int * gameStatus)
 * @see createScore(char name[], int points)
 * @see addScore(struct Score newScore)
 */
void endOfGame(int *gameStatus) {
    // wylaczenie watchdoga
    WDTCTL = WDTPW + WDTHOLD;
    TACTL &= ~MC_1;
    *gameStatus = 0;

    //int zeby potem nie castowac wypisujac
    //ustawienie accuracy i jako iloczyn zlapanych kafelkow i iloraz klikniec
    int accuracy = 100.0 * caught / clicks;
    int endScore = (float) currentScore * (float) accuracy / 100.0;

    SEND_CMD(CLR_DISP);
    SET_CURSOR(4, 1);
    SEND_CHARS("PRZEGRYWASZ");

    //wypisanie inicjałów aktualnego gracza
    SET_CURSOR(2, 2);
    SEND_CHAR(currentInitials[0]);
    SEND_CHAR(currentInitials[1]);

    SEND_CHARS(", WYNIK:");
    if (currentScore == 0) {
        SEND_CHAR('0');
    } else {
        //wyslanie wyniku koncowego
        SEND_NUMBER(endScore);
    }
    //utworzenie nowego wyniku w strukturze Score i dodanie go do tablicy wyników
    if (currentScore == 0) {
        struct Score sc = createScore(currentInitials, endScore);
        addScore(sc);
    } else {
        struct Score sc = createScore(currentInitials, currentScore);
        addScore(sc);
    }
    currentScore = 0; clicks = 0; caught = 0;

    DelayB(200);
    menu(gameStatus); //tutaj zamiast kontynuować to program sie konczy..
}
