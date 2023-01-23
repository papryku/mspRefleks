#define          LCD_Data           P2OUT
#define          _100us             100                //
#define          _10us              10                 //
#define          _WOWIE             2137
#define          E                  3                 //P2.3
#define          RS                 2                 //P2.2

#define		CLR_DISP		0x01    	// clear display
#define		CUR_HOME		0x02	        // return home
#define		ENTRY_INC		0x06            // entry mode increment
#define		ENTRY_INC_ROL		0x07            // entry mode increment with rol data
#define		ENTRY_DEC		0x04            // entry mode decrement
#define		ENTRY_DEC_ROL		0x05            // entry mode decrement witch rol dat
#define		DISP_OFF		0x08	        // all display off
#define		DISP_ON			0x0c	        // all display on
#define         CUR_OFF         0x0c            // 40us

#define         DATA_ROL_LEFT           0x18            // rol data left
#define         DATA_ROL_RIGHT          0x1c            // rol data right
#define         CUR_SHIFT_LEFT          0x10            // shift coursor left
#define         CUR_SHIFT_RIGHT         0x14            // shift coursor right

#define		DD_RAM_ADDR		0x80	        // set DD_RAM
#define		DD_RAM_ADDR2		0xc0	        // set CG_RAM
#define		DD_RAM_ADDR3		0x28	        //
#define		CG_RAM_ADDR		0x40	        //

void InitPortsLcd(void);
