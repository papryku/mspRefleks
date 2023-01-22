//macie zmienne w uartservice.c, nie można ich przekazywać do maina w ten sposób także header dorabiam (nie pytajcie, też nic nie wiem)
//nie wiem nawet czy dobrze, szczerze kija w tym momencie umiem zrobic bez dostepu do IARa salowego

// int numberOfScores = 0;
// int currentLetter = 'A'; // wartosc pierwszego znaku w tablicy ASCII
// ^ to przyklad tego, co nie powinno byc includowane

// int numberOfScores;
// int currentLetter;
// ^ to przyklad samej deklaracji, która może być przekazywana, wtedy stanie się zmienną globalną
// można użyć jeśli te dwie zmienne mają być używane poza funkcjami z biblioteki uartservice

// Struktura przechowujqca wyniki z gry

struct Score {
    char name[2];
    int points;
};

char readChar();
//metoda wysylajaca tablice charow na terminal
//wywala jakis blad


//metoda wysylajaca tablice charow na plytke LCD
void sendCharsToLCD(char c[]);

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
void endOfTheGame(int isWin, int result);
void menu();

