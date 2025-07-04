#ifndef _TAD_TECLATMATRIU_H_
#define _TAD_TECLATMATRIU_H_

#include <xc.h>
#include "TAD_TIMER.h"
#include "TAD_SIO.h"

#define CERT    1
#define FALS    0

#define TRIS_COLUMN2 TRISCbits.TRISC4
#define TRIS_COLUMN1 TRISDbits.TRISD1
#define TRIS_COLUMN0 TRISDbits.TRISD2
#define TRIS_ROW3 TRISDbits.TRISD3
#define TRIS_ROW2 TRISCbits.TRISC5
#define TRIS_ROW1 TRISDbits.TRISD4
#define TRIS_ROW0 TRISDbits.TRISD0

#define COLUMN2 PORTCbits.RC4
#define COLUMN1 PORTDbits.RD1
#define COLUMN0 PORTDbits.RD2
#define ROW3 LATDbits.LATD3
#define ROW2 LATCbits.LATC5
#define ROW1 LATDbits.LATD4
#define ROW0 LATDbits.LATD0

const unsigned char T_REBOTS = 10;  // 10 cops 2ms(int de timer)

void TM_Init(void);
// Pre: --
// Post: Inicialitza el teclat matriu

void TM_Motor(void);
// Pre: --
// Post: Fa l'escombrat del teclat matriu per files

void TM_GetTecla(unsigned char *c);
// Pre: --
// Post: guarda el valor de la tecla polsada a c

unsigned char TM_IsKeyPressed(void);
// Pre: --
// Post: retorna 1 si s'esta polsant una tecla, 0 altrament


#endif