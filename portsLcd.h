/** @file */

#define          LCD_Data           P2OUT       //!< definicja DDRAMu
#define          _100us             300         //!< definicja czasu do odczekania
#define          _10us              30          //!< definicja czasu do odczekania
#define          _WOWIE             2137        //!< definicja czasu do odczekania
#define          E                  3           //!< definicja portu E, P2.3
#define          RS                 2           //!< definicja portu RS, P2.2

#define		CLR_DISP		    0x01    	    //!< instrukcja czyszcząca ekran
#define		CUR_HOME		    0x02	        //!< instrukcja wracająca do 1 kolumny 1 rzędu
#define		DISP_OFF		    0x08	        //!< instrukcja wyłączenia wyświetlacza
#define		DISP_ON			    0x0c	        //!< instrukcja włączenia wyświetlacza
#define     CUR_OFF             0x0c            //!< instrukcja wyłączenia kursora

#define     DATA_ROL_LEFT       0x18            //!< instrukcja przesunięcia danych DDRAM LCD w lewo
#define     DATA_ROL_RIGHT      0x1c            //!< instrukcja przesunięcia danych DDRAM LCD w prawo
#define     CUR_SHIFT_LEFT      0x10            //!< instrukcja przesunięcia kursora w lewo
#define     CUR_SHIFT_RIGHT     0x14            //!< instrukcja przesunięcia kursora w prawo

#define		DD_RAM_ADDR		    0x80	        //!< instrukcja ustawienia kursora w pierwszej linii
#define		DD_RAM_ADDR2		0xc0	        //!< instrukcja ustawienia kursora w drugiej linii
#define		CG_RAM_ADDR		    0x40	        //!< instrukcja ustawienia kursora na początku CGRAMu

/// @cond NIE
void InitPortsLcd(void);
/// @endcond