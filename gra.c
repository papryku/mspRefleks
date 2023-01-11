#include "msp430x14x.h"
#include "lcd.h"
#include "portyLcd.h"
#include "znaki.h"

void rozpocznijGre(char* inicjaly){
    SEND_CMD(CLR_DISP);
    SEND_CMD(CUR_OFF); //wylaczenie kursora
    SEND_CHAR(znak pola);
    SEND_CMD(DD_RAM_ADDR2);
    SEND_CHAR(znak pola);
}

//znak lapania w obu liniach po lewej
//SEND_CHAR(0);
//SEND_CMD(DD_RAM_ADDR2);
//odliczanie??

