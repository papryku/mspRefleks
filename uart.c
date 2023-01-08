#include <msp430x14x.h>
#include "uart.h"



void initUart( int Speed)
{ 
int i;
	BCSCTL1 |= XTS;                       // ACLK = LFXT1 = HF XTAL 8MHz
   do
	{
	IFG1 &= ~OFIFG;                       // Czyszczenie flgi OSCFault
		for (i = 0xFF; i > 0; i--);           // odczekanie 
	}
   while ((IFG1 & OFIFG));               // dopóki OSCFault jest ci¹gle ustawiona 
   BCSCTL2 |= SELM1+SELM0 ;              // MCLK =LFXT1 

 switch(Speed)
  {
  case 1200:{
   ME1 |= UTXE0 + URXE0;                 // W³¹czenie USART0 TXD/RXD
   UCTL0 |= CHAR;                        // 8-bitów
   UTCTL0 |= SSEL0;                      // UCLK = ACLK
   UBR00 = 0x0A;                         // 8MHz/Speed in Bauds
   UBR10 = 0x1A;                         //    
   UMCTL0 = 0x5B;                        // Modulation
   UCTL0 &= ~SWRST;                      // Inicjalizacja UARTA
   IE1 |= URXIE0;                        // W³¹czenie przerwañ od RX 
   break;
   }
  }
}

//---------------- wysy³anie znaku ---------
void UartCharTransmit(unsigned char znak)
{
 while ( (IFG1 & UTXIFG0)==0);          // gdy uk³¹d nie jest zajêty      
 TXBUF0=znak;                           // wysy³amy znak
}

//---------------- wysy³anie napisu ----------
void UartStringTransmit(char * napis)   
{
  while( *napis)
    UartCharTransmit(*napis ++);   
}
