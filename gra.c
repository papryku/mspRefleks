#include "msp430x14x.h"
#include "lcd.h"
#include "portyLcd.h"
#include "znaki.h"
#include "kafelek.h"
#include <stdbool.h>

int rozpoczeta = false;

//inicjaly chyba jednak niepotrzebne w tej sekcji tbh
void rozpocznijGre(){
    //pauza interrupta
    TACTL &= ~MC_1;

    SEND_CMD(CLR_DISP);
    SEND_CMD(CUR_OFF); //wylaczenie kursora
    SEND_CHAR(0);
    SEND_CMD(DD_RAM_ADDR2); //czyli SET_CURSOR(1,2)
    SEND_CHAR(0);

    for(int i = 3; i>0; i--){
        SET_CURSOR(8,1);
        SEND_CHAR(48+i);
        SEND_CHAR('.');
        SEND_CHAR('.');
    }

    //może return jakby byl problem z odpauzowaniem interrupta
    //odpauzowanie interrupta
    
    TACTL |= MC_1;
    rozpoczeta = true;
    //juz powinny leciec sobie tyntyryn
}


//struct gotowego kafelka z parametrami bedzie tworzony
//w przerwaniach zegara, poniewaz z poziomu namespace'a gra nie mamy
//dostepu do tablicy istniejacych kafelkow, w skrocie informacji czy pozycja zajeta bla bla bla
void nowyKafelek(Kafelek *nowy){
    SET_CURSOR(nowy->column, nowy->row);
    SEND_CHAR(1);
}

//
void przesunKafelki(Kafelek *kafelki){
    int dl = sizeof(kafelki)/sizeof(kafelki[0]);
    int stan1 = 0, stan2 = 1;

    //inna mozliwosc to bezposrednia edycja 30 pol DDRAMU z odpowiednim przesunieciem w lewo pomijajac pola lapania
    SEND_CMD(DATA_ROL_LEFT);
    //SET_CURSOR(1,2); SEND_CHAR(0);

    for(int i=0; i<dl; i++){
        kafelki[i].column--;
        if(kafelki[i].column==1){
            SET_CURSOR(1,kafelki[i].row);
            SEND_CHAR(2);
        }
    }
    if(!stan1) { SET_CURSOR(1,1); SEND_CHAR(0); }
    if(!stan2) { SET_CURSOR(1,2); SEND_CHAR(0); }

    //nie wiem jeszcze jak zareagowac na kafelek==0 i gdzie, chyba w timerze??
    //albo miec w powazaniu az nie okaze sie powodowac artefakty graficzne
}
