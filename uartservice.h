//zmienna globalna, która będzie modyfikowana z poziomu main.c, a następnie używana w uartservice.h
//flagowanie chyba niepotrzebne, ale nie zaszkodzi
#ifndef WYNIK
#define WYNIK
int wynikAktualnej;
#endif

// Struktura przechowujqca wyniki z gry
typedef struct Score {
    char name[2];
    int points;
} Score;


char readChar();
//metoda wysylajaca tablice charow na terminal

//metoda wysylajaca tablice charow na plytke LCD
void sendCharsToLCD(char c[]);

char *podajInicjaly();

//metoda sortujaca tablice wynikow
void sortScores(int n);

//metoda tworzaca nowy rekord po rozegraniu gry
struct Score createScore(char name[], int points);

//metoda dodajaca nowy rekord do tablicy wynikow
void addScore(struct Score newScore);

char* intToCharArray(int num);

//metoda wypisujaca tablice wynikow do terminala
void printScores();

//metoda wyswietlajaca na LCD zwyciestwo lub przegrana
//zmienie dzialanie troche
void endOfTheGame(int isWin, int result);

//najwazniejsze co tu jest
void menu();

//przerzucam swoje z gry.c zeby nie bawic sie w rzucanie wskaznikiem do inicjalow
void koniecGry(char inicjaly[2], int wynik);