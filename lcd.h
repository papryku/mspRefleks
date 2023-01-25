/******************************************************************
 *****                                                        *****
 *****  Name: lcd.c                                       *****
 *****  modified version of Andreas Dannenberg                *****
 *****  Includes only display specific code.		      *****     
 ******************************************************************/


void InitLCD();

/**
 * @brief funkcja służąca do czyszczenia wyświetlacza LCD
 * 
 */
void clearDisplay();


void SEND_CHAR (unsigned char c);


void SEND_CMD (unsigned char e);

/**
 * @brief funkcja wypisująca liczbę na ekran od najmniej znaczącej do najbardziej wysyłając cyfry do funkcji SEND_CHAR
 * 
 * @param num liczba całkowita do wypisania
 */
void SEND_NUMBER(int num);

void MAKE_DEFINED_CHAR(unsigned char c);

/**
 * @brief funkcja opóźniająca program o podaną ilość taktów
 * 
 * @param a ilość taktów
 */
void Delay(unsigned int a);

/**
 * @brief funkcja opóźniająca program o 300 taktów * podany parametr
 * 
 * @param b ilość 300etek taktów
 */
void Delayx100us(unsigned char b);

/**
 * @brief funkcja opóźniająca program o 2137 taktów * podany parametr
 * 
 * @param b ilość 2137etek taktów
 */
void DelayB(unsigned int b);

void SET_CURSOR(int column, int row);

void CREATE_CHAR(int addr, char *data);

/**
 * @brief funkcja przyjmująca tablicę charów i wypisująca ją po koleji przez funkcję SEND_CHAR
 * 
 * @param napis tablica charów do wypisania
 */
void SEND_CHARS(char * napis);



/**
 * @brief funkcja służąca do przerzucenia kursora LCD na drugą linię
 * 
 */
void gotoSecondLine();