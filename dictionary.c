/*
 //Interface 0

unsigned char TI_NewTimer(unsigned char *TimerHandle) ;
	// Post: Retorna TI_CERT en cas que s'hagi creat adequadament un nou timer, i TI_FALS en cas contrati.
    // Posa a *TimerHandle l'identificador de timer assignat, necessari per usar les funcions TI_GetTics i TI_ResetTics.

void TI_ResetTics (unsigned char TimerHandle);
	// Pre: Handle ha estat retornat per Ti_NewTimer.
	// Post: Engega la temporització associada a 'TimerHandle', guardant la referencia temporal en el moment de la crida.

unsigned long TI_GetTics (unsigned char TimerHandle);
	// Pre: Handle ha estat retornat per TI_NewGetTimer.
	// Post: Retorna els tics transcorreguts des de la crida a TI_ResetTics per al mateix TimerHandle.

//Interface 1

unsigned char SIO_Send(unsigned char character);
// Pre: character ha de ser un caracter valid en ASCII.
// Post: retorna 1 si s'ha enviat el byte, 0 altrament

unsigned char SIO_Receive(unsigned char* characters);
// Pre: --
// Post: retorna el byte rebut, 0 altrament

void SIO_SendString(const char *str);
// Pre: --
// Post: envia el string per la SIO//Interface 2

void TM_GetTecla(unsigned char *c);
// Pre: --
// Post: guarda el valor de la tecla polsada a c

char TM_HashPressed(void);
// Pre: --
// Post: retorna 1 si sha polsat el hash, 0 altrament

unsigned char TM_IsKeyPressed(void);
// Pre: --
// Post: retorna 1 si s'esta polsant una tecla, 0 altrament

//Interface 3

void LEDS_setIntensitat(unsigned char quinLed, unsigned char valor);
// Pre: --
// Post: estableix la intensitat de el led(quinLed)

//Interface 4

unsigned char TI_NewTimer(unsigned char *TimerHandle) ;
	// Post: Retorna TI_CERT en cas que s'hagi creat adequadament un nou timer, i TI_FALS en cas contrati.
    // Posa a *TimerHandle l'identificador de timer assignat, necessari per usar les funcions TI_GetTics i TI_ResetTics.

void TI_ResetTics (unsigned char TimerHandle);
	// Pre: Handle ha estat retornat per Ti_NewTimer.
	// Post: Engega la temporització associada a 'TimerHandle', guardant la referencia temporal en el moment de la crida.

unsigned long TI_GetTics (unsigned char TimerHandle);
	// Pre: Handle ha estat retornat per TI_NewGetTimer.
	// Post: Retorna els tics transcorreguts des de la crida a TI_ResetTics per al mateix TimerHandle.

//Interface 5

unsigned char SIO_Send(unsigned char character);
// Pre: character ha de ser un caracter valid en ASCII.
// Post: retorna 1 si s'ha enviat el byte, 0 altrament

unsigned char SIO_Receive(unsigned char* characters);
// Pre: --
// Post: retorna el byte rebut, 0 altrament

void SIO_SendString(const char *str);
// Pre: --
// Post: envia el string per la SIO//Interface 6

unsigned char TI_NewTimer(unsigned char *TimerHandle) ;
	// Post: Retorna TI_CERT en cas que s'hagi creat adequadament un nou timer, i TI_FALS en cas contrati.
    // Posa a *TimerHandle l'identificador de timer assignat, necessari per usar les funcions TI_GetTics i TI_ResetTics.

void TI_ResetTics (unsigned char TimerHandle);
	// Pre: Handle ha estat retornat per Ti_NewTimer.
	// Post: Engega la temporització associada a 'TimerHandle', guardant la referencia temporal en el moment de la crida.

unsigned long TI_GetTics (unsigned char TimerHandle);
	// Pre: Handle ha estat retornat per TI_NewGetTimer.
	// Post: Retorna els tics transcorreguts des de la crida a TI_ResetTics per al mateix TimerHandle.

//Interface 7

void MENU_SaveUID(unsigned char UID[5]);
// Pre: cal que l'UID sigui valid
// Post: guarda el UID//Interface 8

unsigned char SIO_Send(unsigned char character);
// Pre: character ha de ser un caracter valid en ASCII.
// Post: retorna 1 si s'ha enviat el byte, 0 altrament

unsigned char SIO_Receive(unsigned char* characters);
// Pre: --
// Post: retorna el byte rebut, 0 altrament

void SIO_SendString(const char *str);
// Pre: --
// Post: envia el string per la SIO//Interface 9

void LcClear(void);
// Post: Clears the display and sets the cursor to its previous state. 
// Post: The next order can last up to 1.6ms. 

void LcCursorOn(void);
// Post: Turn on the cursor
// Post: The next order can last up to 40us. 

//void LcCursorOff(void);
// Post: Turns off the cursor
// Post: The next order can last up to 40us. 

void LcGotoXY(char Column, char Row);
// Pre : Column between 0 and 39, row between 0 and 3. 
// Post: Sets the cursor to those coordinates. 
// Post: The next order can last up to 40us. 

void LcPutChar(char c);
// Post: Paints the char in the actual cursor position and increments 
// its position. If the column gets to 39 it returns to 0.
// The row of the LCD is increased when this happens until the second
// row and then it is reset back to row 0 if it has 2 rows total. 
// If the LCD has 4 rows it will reset back to row 0 when it
// reaches row 4 and the columns will go till 39 before reseting to 0.
// The one row LCDs returns to 0 when a column gets to 39. 
// The row is never increased. 
	// The char is written

//void LcPutString(char *s);
// Post: Paints the string from the actual cursor position. 
// The coordinate criteria is the same as the LcPutChar. 
// Post: Can last up to 40us per char of a routine output.


 
 */