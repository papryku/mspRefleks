void InitLCD();

void clearDisplay();

void SEND_CHAR (unsigned char c);

void SEND_CMD (unsigned char e);

void SEND_NUMBER(int num);

void MAKE_DEFINED_CHAR(unsigned char c);

void Delay(unsigned int a);

void Delayx100us(unsigned char b);

void DelayB(unsigned int b);

void SET_CURSOR(int column, int row);

void CREATE_CHAR(int addr, char *data);

void SEND_CHARS(char * napis);

void gotoSecondLine();
