/******************************************************************
 *****                                                        *****
 *****  Name: display.c                                       *****
 *****  modified version of Andreas Dannenberg                *****
 *****  Includes only display specific code.		      *****                                                        *****
 ******************************************************************/


#include "msp430x14x.h" //zamiennik dla msp430x14x.h, w sali uzywamy msp430x14x.h
#include "lcd.h"
#include "portyLcd.h"
#include "znaki.h"


#define bitset(var,bitno) ((var) |= 1 << (bitno))
#define bitclr(var,bitno) ((var) &= ~(1 << (bitno)))



void Delay (unsigned int a);
void Delayx100us(unsigned char b);
void _E(void);



void clearDisplay() {
    SEND_CMD(CLR_DISP);
    Delayx100us(10);
}

void gotoSecondLine() {
    SEND_CMD(DD_RAM_ADDR2);
}

//robocza wersja, gdybyśmy robili więcej niż 8 znaków to trzeba by było podmieniać znaki w trakcie gry
//ale mamy tylko 3 także ez
void loadCustomCharacters(){

}


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

void Delayx100us(unsigned char b)
{
  int j;
  for (j=0; j!=b; ++j) Delay (_100us);
}


void _E(void)
{
        bitset(P2OUT,E);		//toggle E for LCD
	Delay(_10us);
	bitclr(P2OUT,E);
}


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


void CREATE_CHAR(int addr, int *data){
  if (addr > 7 || addr < 0){
    return 0;
  }
  SEND_CMD(0x40 | addr << 3);
  SEND_CHAR(data);
}

//row 1 albo 2
//column on 1 do 16

void SET_CURSOR(int column, int row)
{
  SEND_CMD(0x80 | ((row - 1) << 6) | (column - 1));
}