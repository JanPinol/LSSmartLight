#ifndef _TAD_MENU_H_
#define _TAD_MENU_H_

#include <xc.h>
#include "TAD_SIO.h"
#include "TAD_TECLATMATRIU.h"
#include "TAD_LEDS.h"
#include "LcTLCD.h"
#include "TAD_TIMER.h"

#define CERT 1
#define FALS 0
#define ESC_KEY 0x1B
#define ENTER 0x0D
#define T_3S 1500

#define NUM_CONFIGS 6   // Número màxim de configuracions guardades
#define UID_LENGTH 5    // Longitud d'un UID

// Array on cada fila és un UID guardat
const unsigned char uid_saved[NUM_CONFIGS][UID_LENGTH] = {
    {0x00, 0x00, 0x00, 0x00, 0x00},
    {0xE3, 0xC2, 0x26, 0xDB, 0xDC},
    {0x03, 0x6D, 0xD2, 0xD9, 0x65},
    {0x3D, 0xE6, 0x97, 0x3F, 0x73},
    {0xCC, 0xD3, 0x96, 0x3F, 0xB6},
    {0x88, 0x05, 0x6A, 0xCA, 0x2D}
    // ...
};

// Configuració de LEDs associada a cada UID
static unsigned char leds_saved[NUM_CONFIGS][6] = {
    {0, 0, 0, 0, 0, 0},
    {1, 2, 3, 4, 5, 6},
    {10, 10, 10, 10, 10, 10},
    {1, 1, 1, 1, 1, 1},
    {0, 2, 4, 6, 8, 10},
    {0, 2, 4, 6, 8, 10}
    // ...
};



const char menu_principal[] = 
    "\033[2J\033[H"
    "\r\n----------------"
    "\r\nMenú principal"
    "\r\n----------------"
    "\r\nTria una opció:"
    "\r\n\t1. Qui hi ha a la sala?"
    "\r\n\t2. Mostrar configuracions"
    "\r\n\t3. Modificar hora del sistema"
    "\r\nOpció: ";

const char res_a_mostrar[] = "\r\nRes a mostrar";
const char hora_sistema[] = "\r\nHora del sistema: ";
const char separador_hora[] = ":";
const char enter_aplicar[] = "\r\nEnter per aplicar... ";
const char canvis_aplicats[] = "\r\nCanvis aplicats.";

void MENU_Init(void);

void MENU_Motor(void);
void MENU_MotorHandleHash(void);
void MENU_MotorHandleSetIntensity(void);
void MENU_MotorActualitzarLCD(void);

void MENU_SaveUID(unsigned char UID[5]);
// Pre: cal que l'UID sigui valid
// Post: guarda el UID

void MENU_addMinute(void);

#endif
