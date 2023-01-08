#include <msp430.h>
#include <stdio.h>
#include <string.h>

// Struktura przechowuj?ca wyniki z gry
struct Score {
    char name[32];
    int points;
};

// Tablica wyników
struct Score scores[10];

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Disable watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

    // Konfiguracja UART
    P1SEL0 |= BIT0 | BIT1;      // P1.0 i P1.1 jako z??cza UCA0TXD i UCA0RXD
    P1SEL1 &= ~(BIT0 | BIT1);

    UCA0CTLW0 = UCSSEL__SMCLK;  // Ustawienie SMCLK jako ?ród?a sygna?u zegarowego
    UCA0BRW = 6;                // Baud rate = 9600
    UCA0MCTLW = UCOS16 | UCBRF_1 | 0xD600;  // Modulacja UCBRSx = 1, UCBRFx = 0, UCOS16 = 1

    UCA0CTLW0 &= ~UCSWRST;     // Inicjalizacja UART



    // Wysy?anie tablicy wyników do komputera
    int i;
    for (i = 0; i < 10; i++) {
        char buffer[64];
        sprintf(buffer, "%s: %d points\n", scores[i].name, scores[i].points);
        int j;
        for (j = 0; j < strlen(buffer); j++) {
            while (!(UCA0IFG & UCTXIFG));
            UCA0TXBUF = buffer[j];
        }
    }

    return 0;
}


void sortScores(struct Score scores[], int n) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        // Szukanie najwi?kszego elementu w tablicy
        int max_idx = i;
        for (j = i + 1; j < n; j++) {
            if (scores[j].points > scores[max_idx].points) {
                max_idx = j;
            }
        }

        // Zamiana miejscami najwi?kszego elementu z elementem na pozycji i
        struct Score temp = scores[i];
        scores[i] = scores[max_idx];
        scores[max_idx] = temp;
    }
}

void addScore(struct Score scores[], int arraySize, struct Score newScore) {
    int index = findEmptyIndex(scores[], arraySize);
    if (index <= 9) {
        scores[index] = newScore;
    }
    if (newScore.points > scores[9].points) {
        scores[9] = newScore
    }
    sortScores(scores, arraySize);
}

int findEmptyIndex(struct Score scores[], int arraySize) {
    for (int i = 0; i < arraySize; i++) {
        if (scores[i].points == 0) {
            return i;
        }
    }
    return 0;
}

void printScores(struct Score scores[], int n) {
    int i;
    for (i = 1; i <= n; i++) {
        printf("%d. %s: %d points\n", i, scores[i].name, scores[i].points);
    }
}

