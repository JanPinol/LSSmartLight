#include "TAD_LEDS.h"

static unsigned char state[N_LEDS];
static unsigned char intensitat[N_LEDS];
static unsigned char TimerLed[N_LEDS];

void LEDS_Init(void) {
    TRIS_LED0 = TRIS_LED1 = TRIS_LED2 = TRIS_LED3 = TRIS_LED4 = TRIS_LED5 = 0;
    LED0 = LED1 = LED2 = LED3 = LED4 = LED5 = 0;

    for(int i = 0; i < N_LEDS; i++) {
        state[i] = intensitat[i] = 0;
        TI_NewTimer(&TimerLed[i]);
    }
}

void LEDS_setState(unsigned char quinLed, unsigned char estat) {
    switch (quinLed) {
        case 0: LED0 = estat; break;
        case 1: LED1 = estat; break;
        case 2: LED2 = estat; break;
        case 3: LED3 = estat; break;
        case 4: LED4 = estat; break;
        case 5: LED5 = estat; break;
    }
}

void LEDS_Motor(unsigned char quinLed) {
 
    if (intensitat[quinLed] == 0) {  
        LEDS_setState(quinLed, 0);
        state[quinLed] = 0;
        return;
    }

    if (intensitat[quinLed] == 10) {  
        LEDS_setState(quinLed, 1);
        state[quinLed] = 0;
        return;
    }

    switch(state[quinLed]) {
        case 0:  // LED encès
            if (TI_GetTics(TimerLed[quinLed]) >= intensitat[quinLed]) { 
                LEDS_setState(quinLed, 0);
                TI_ResetTics(TimerLed[quinLed]);
                state[quinLed]++;
            }
            break;

        case 1:  // LED apagat
            if (TI_GetTics(TimerLed[quinLed]) >= (MAX_PERIODE - intensitat[quinLed])) { 
                LEDS_setState(quinLed, 1);
                TI_ResetTics(TimerLed[quinLed]);
                state[quinLed]--;
            }
            break;
    }
}

void LEDS_setIntensitat(unsigned char quinLed, unsigned char valor) {
    intensitat[quinLed] = valor;
}
