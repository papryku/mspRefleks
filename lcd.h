void InitLCD(void);
void clearDisplay();
void SEND_CHAR (unsigned char c);
void SEND_CMD (unsigned char e);
void MAKE_DEFINED_CHAR(unsigned char c);
void Delayx100us(unsigned char b);
void SET_CURSOR(int column, int row);
void CREATE_CHAR(int addr, char *data);
