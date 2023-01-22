#include "msp430x14x.h"
#include "lcd.h"
#include "portyLcd.h"
#include "kafelek.h"
#include "gra.h"

//inicjaly chyba jednak niepotrzebne w tej sekcji tbh
void rozpocznijGre(int *rozpoczeta){
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
        Delayx100us(100000);
    }
    
    SET_CURSOR(8,1);
    SEND_CHAR(' ');
    SEND_CHAR(' ');
    SEND_CHAR(' ');

    //może return jakby byl problem z odpauzowaniem interrupta
    //odpauzowanie interrupta
    
    TACTL |= MC_1;
    *rozpoczeta = 1;
    //juz powinny leciec sobie tyntyryn
}


//struct gotowego kafelka z parametrami bedzie tworzony
//w przerwaniach zegara, poniewaz z poziomu namespace'a gra nie mamy
//dostepu do tablicy istniejacych kafelkow, w skrocie informacji czy pozycja zajeta bla bla bla
void nowyKafelek(Kafelek *nowy){
    SET_CURSOR(nowy->column, nowy->row);
    SEND_CHAR(2);
}

//
void przesunKafelki(Kafelek *kafelki, int *wykonuje){
    *wykonuje = 1;
    int dl = sizeof(kafelki)/sizeof(kafelki[0]);
    int stan[2] = {1,1};

    //inna mozliwosc to bezposrednia edycja 30 pol DDRAMU z odpowiednim przesunieciem w lewo pomijajac pola lapania
    SET_CURSOR(1,1); SEND_CHAR(' ');
    SET_CURSOR(2,1); SEND_CHAR(' ');
    
    SEND_CMD(DATA_ROL_LEFT);

    for(int i=0; i<dl; i++){
        kafelki[i].column--;
        if(kafelki[i].column==1){
            SET_CURSOR(1,kafelki[i].row);
            SEND_CHAR(1);
            stan[kafelki[i].row+1]=0;
        }
    }
    if(!stan[0]) { SET_CURSOR(1,1); SEND_CHAR(0); }
    if(!stan[1]) { SET_CURSOR(1,2); SEND_CHAR(0); }
    
    *wykonuje = 0;
    //nie wiem jeszcze jak zareagowac na kafelek==0 i gdzie, chyba w timerze??
    //albo miec w powazaniu az nie okaze sie powodowac artefakty graficzne
}


//tablica 2x16 przechowujaca jaki znak i wyslac