#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"

/** @file */

/** @brief ustawia podany bit liczby na 1
 *  @param var liczba do modyfikacji
 *  @param bitno numer bitu do modyfikacji 
 */
#define bitset(var,bitno) ((var) |= 1 << (bitno))

/** @brief ustawia podany bit liczby na 0
 *  @param var liczba do modyfikacji
 *  @param bitno numer bitu do modyfikacji 
 */
#define bitclr(var,bitno) ((var) &= ~(1 << (bitno)))

void Delay (unsigned int a);
void DelayB (unsigned int b);
void Delayx100us(unsigned char b);
void _E(void);

/**
 * @brief funkcja wypisująca po kolei zawartość tablicy charów przez funkcję SEND_CHAR
 * 
 * @param napis tablica charów do wypisania 
 */
void SEND_CHARS(char * napis)
{
    while( *napis)
        SEND_CHAR(*napis ++);
}

/**
 * @brief funkcja wypisująca liczbę na ekran od najmniej znaczącej do najbardziej wysyłając cyfry do funkcji SEND_CHAR
 * 
 * @param num liczba do wypisania
 */
void SEND_NUMBER(int num){
    int arr[10];
    int i=0;
    int j,r;
    while(num != 0){
      r = num%10;
      arr[i] = r;
      i++;

      num = num/10;
    }
    for(j=i-1; j>=0; j--){
        SEND_CHAR(arr[j]+48);
    }
}

/**
 * @brief funkcja służąca do czyszczenia LCD
 * 
 */
void clearDisplay() {
    SEND_CMD(CLR_DISP);
    Delayx100us(10);
}

/**
 * @brief funkcja służąca do przerzucenia kursora LCD na drugą linię
 * 
 */
void gotoSecondLine() {
    SEND_CMD(DD_RAM_ADDR2);
}

/**
 * @brief funkcja opóźniająca program o podaną ilość taktów
 * 
 * @param a ilość taktów
 */
void Delay (unsigned int a)
{
    int k;
    for (k=0 ; k != a; ++k) {
        _NOP();
        _NOP();
        _NOP();
        _NOP();
    }
}

/**
 * @brief funkcja opóźniająca program o 300 taktów * podany parametr
 * 
 * @param b ilość 300etek taktów
 */
void Delayx100us(unsigned char b)
{
    int j;
    for (j=0; j!=b; ++j) Delay (_100us);
}

/**
 * @brief funkcja opóźniająca program o 2137 taktów * podany parametr
 * 
 * @param b ilość 2137emek taktów
 */
void DelayB(unsigned int b)
{
    int j;
    for (j=0; j!=b; ++j) Delay (_WOWIE);
}

/**
 * @brief funkcja służąca do przełączania wartości podanej na pin E LCD
 * 
 */
void _E(void)
{
    bitset(P2OUT,E);		//toggle E for LCD
    Delay(_10us);
    bitclr(P2OUT,E);
}

/**
 * @brief funkcja służąca do wysłania pojedyńczego znaku na LCD
 * 
 * @param d znak do wysłania
 */
void SEND_CHAR (unsigned char d)
{
    int temp;
    Delayx100us(5);                 //.5ms
    temp = d & 0xf0;		//get upper nibble
    LCD_Data &= 0x0f;
    LCD_Data |= temp;
    bitset(P2OUT,RS);     	        //set LCD to data mode
    _E();                           //toggle E for LCD
    temp = d & 0x0f;
    temp = temp << 4;               //get down nibble
    LCD_Data &= 0x0f;
    LCD_Data |= temp;
    bitset(P2OUT,RS);   	        //set LCD to data mode
    _E();                           //toggle E for LCD
}

/**
 * @brief funkcja służąca do wysłania komendy do LCD
 * 
 * @param e kod instrukcji komendy
 */
void SEND_CMD (unsigned char e)
{
    int temp;
    Delayx100us(10);                //10ms
    temp = e & 0xf0;		//get upper nibble
    LCD_Data &= 0x0f;
    LCD_Data |= temp;               //send CMD to LCD
    bitclr(P2OUT,RS);     	        //set LCD to CMD mode
    _E();                           //toggle E for LCD
    temp = e & 0x0f;
    temp = temp << 4;               //get down nibble
    LCD_Data &= 0x0f;
    LCD_Data |= temp;
    bitclr(P2OUT,RS);   	        //set LCD to CMD mode
    _E();                           //toggle E for LCD
}

/**
 * @brief funkcja służąca do inicjalizacji LCD
 * 
 */
void InitLCD(void)
{
    bitclr(P2OUT,RS);
    Delayx100us(250);                   //Delay 100ms
    Delayx100us(250);
    Delayx100us(250);
    Delayx100us(250);
    LCD_Data |= BIT4 | BIT5;            //D7-D4 = 0011
    LCD_Data &= ~BIT6 & ~BIT7;
    _E();                               //toggle E for LCD
    Delayx100us(100);                   //10ms
    _E();                               //toggle E for LCD
    Delayx100us(100);                   //10ms
    _E();                               //toggle E for LCD
    Delayx100us(100);                   //10ms
    LCD_Data &= ~BIT4;
    _E();                               //toggle E for LCD

    SEND_CMD(DISP_ON);
    SEND_CMD(CLR_DISP);
    Delayx100us(250);
    Delayx100us(250);
    Delayx100us(250);
    Delayx100us(250);
}

/**
 * @brief funkcja służąca do tworzenia niestandardowych znaków
 * 
 * @param addr miejsce w pamięci CGRAM, które ma zająć znak <0,7>
 * @param data tablica zawierająca dane niestandardowego znaku
 */
void CREATE_CHAR(int addr, char *data){
    SEND_CMD(0x40+addr*8);
    SEND_CHARS(data);
}

/**
 * @brief funkcja służąca do ustawienia kursora LCD na pozycję podaną w parametrach
 * 
 * @param column kolumna, w której będzie ustawiony kursor <1,16>
 * @param row rząd, w którym będzie ustawiony kursor <1,2>
 */
void SET_CURSOR(int column, int row)
{
    SEND_CMD(0x80 | ((row - 1) << 6) | (column - 1));
}
