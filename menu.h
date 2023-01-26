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

void printScores(int *gameStatus);

void menu(int *gameStatus);

void endOfGame(int *gameStatus);
/// @endcond