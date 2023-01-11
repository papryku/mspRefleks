#include "msp430x14x.h"
#include "lcd.h"
#include "portyLcd.h"
#include "znaki.h"
#include "kafelek.h"

void rozpocznijGre(char* inicjaly){
    SEND_CMD(CLR_DISP);
    SEND_CMD(CUR_OFF); //wylaczenie kursora
    SEND_CHAR(0);
    SEND_CMD(DD_RAM_ADDR2); //czyli SET_CURSOR(1,2)
    SEND_CHAR(0);
}

//struct gotowego kafelka z parametrami bedzie tworzony
//w przerwaniach zegara, poniewaz z poziomu namespace'a gra nie mamy
//dostepu do tablicy istniejacych kafelkow, w skrocie informacji czy pozycja zajeta bla bla bla
void nowyKafelek(Kafelek nowy){
    SET_CURSOR(nowy.column, nowy.row);
    SEND_CHAR(1);
}

//znak lapania w obu liniach po lewej
//SEND_CHAR(0);
//SEND_CMD(DD_RAM_ADDR2);
//odliczanie??

