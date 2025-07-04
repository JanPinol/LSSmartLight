#ifndef _TAD_LEDS_H_
#define	_TAD_LEDS_H_

#include <xc.h>
#include "TAD_TIMER.h"

#define TRIS_LED0 TRISBbits.TRISB7
#define TRIS_LED1 TRISBbits.TRISB6
#define TRIS_LED2 TRISBbits.TRISB5
#define TRIS_LED3 TRISBbits.TRISB4
#define TRIS_LED4 TRISBbits.TRISB3
#define TRIS_LED5 TRISBbits.TRISB2

#define LED0 LATBbits.LATB7
#define LED1 LATBbits.LATB6
#define LED2 LATBbits.LATB5
#define LED3 LATBbits.LATB4
#define LED4 LATBbits.LATB3
#define LED5 LATBbits.LATB2

#define N_LEDS 6
#define MAX_PERIODE 10

void LEDS_Init(void);
// Pre: --
// Post: --

void LEDS_Motor(unsigned char quinLed);
// Pre: --
// Post: --

void LEDS_setIntensitat(unsigned char quinLed, unsigned char valor);
// Pre: --
// Post: estableix la intensitat de el led(quinLed)

#endif

