
// Struktura przechowujqca wyniki z gry
typedef struct Score
{
    char name[2];
    int points;
} Score;

// modyfikuje wynik aktualnej gry
void zmienWynik(int num);

// funkcja pozwalająca wybrać literę, którą później zwraca
char readChar();

// funkcja wysylajaca tablice charow na plytke LCD
// nie widze zeby byla w kodzie i/lub uzywana takze wyjebie jak jutro nie bedzie bledow
// void sendCharsToLCD(char c[]);

// funkcja pozwalająca wybrać inicjały, które później zwraca w tablicy
char *podajInicjaly();

// funkcja sortujaca tablice wynikow
void sortScores(int n);

// funkcja tworzaca nowy rekord po rozegraniu gry
struct Score createScore(char name[], int points);

// funkcja dodajaca nowy rekord do tablicy wynikow
void addScore(struct Score newScore);

// funkcja zastąpiona SEND_NUMBER z lcd
// char* intToCharArray(int num);

// funkcja wypisujaca tablice wynikow do terminala
void printScores();

// funkcja wyswietlajaca na LCD zwyciestwo lub przegrana
// zmienie dzialanie troche
void endOfTheGame(int isWin, int result);

// najwazniejsze co tu jest tbh
void menu(int *rozpoczeta);

// funkcja kończąca rozgrywkę, wyświetla końcowy wynik i inicjały, przekazuje je do tabeli wyników i powraca do menu()
void koniecGry(int *rozpoczeta); // char inicjaly[2], int wynik
