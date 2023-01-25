/** @brief Struktura przechowujqca wyniki z gry
 */
typedef struct Score {
    char name[2]; /**< inicjały gracza */
    int points;   /**< punkty gracza*/
} Score;


/// @cond NIE
void changeScore(int num);

char readChar();

char *getInitials();

void sortScores(int n);

struct Score createScore(char name[], int points);

void addScore(struct Score newScore);

void printScores(int *rozpoczeta);

void menu(int *rozpoczeta);

void endOfGame(int *rozpoczeta);
/// @endcond