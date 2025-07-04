#include <xc.h>

/*
 *  ---------------- RA ------------------
 * RA0 --> 
 * RA1 --> 
 * RA2 --> RS (LCD)
 * RA3 --> RW (LCD)
 * RA4 --> E (LCD)
 * RA5 --> D4 (LCD)
 * RA6 --> OSC
 * RA7 --> OSC

 * ---------------- RB ------------------
 * RB0 --> SCK (RC522)
 * RB1 --> SDA (RC522)
 * RB2 --> LED5
 * RB3 --> LED4
 * RB4 --> LED3
 * RB5 --> LED2
 * RB6 --> LED1
 * RB7 --> LED0

 * ---------------- RC ------------------
 * RC0 --> 
 * RC1 --> DEBUG
 * RC2 --> 
 * RC3 --> 
 * RC4 --> COL2
 * RC5 --> ROW2
 * RC6 --> TX
 * RC7 --> RX

 * ---------------- RD ------------------
 * RD0 --> ROW0
 * RD1 --> COL1
 * RD2 --> COL0
 * RD3 --> ROW3
 * RD4 --> ROW1
 * RD5 --> RST (RC522)
 * RD6 --> MISO (RC522)
 * RD7 --> MOSI (RC522)

 * ---------------- RE ------------------
 * RE0 --> D5 (LCD)
 * RE1 --> D6 (LCD)
 * RE2 --> D7 (LCD)
 * RE3 --> MCLR
 */

#pragma config OSC = HSPLL
#pragma config PBADEN = DIG
#pragma config MCLRE = ON
#pragma config DEBUG = OFF //CONFIG4Lbits.DEBUG = 0;
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF //CONFIG4Lbits.LVP = 0;

#include "TAD_TIMER.h"
#include "LcTLCD.h"
#include "TRFID.h"
#include "TAD_SIO.h"
#include "TAD_MENU.h"
#include "TAD_TECLATMATRIU.h"
#include "TAD_LEDS.h"

static unsigned int ticsCounter = 0;

void __interrupt(high_priority) RSI_High(void) {
    RSI_Timer0();
    
    ticsCounter++;
    
    if(ticsCounter == 30000) {
        ticsCounter = 0;
        MENU_addMinute();
    }
}

void Init_Interrupcions(void) {
    RCONbits.IPEN = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}
    
void main(void) {    
    
    // Inits
    Init_Interrupcions();
    TI_Init();
    SIO_Init();
    MENU_Init();
    TM_Init();
    RFID_Init();
    LEDS_Init();
    
    LcInit(2, 16);
    LcClear();
    LcCursorOn();
    
    TRISCbits.TRISC1 = 0;

    while (CERT) {
        LATCbits.LATC1 = !LATCbits.LATC1;
        
        MENU_MotorActualitzarLCD();
        MENU_MotorHandleHash();
        MENU_Motor();
        MENU_MotorHandleSetIntensity();
        TM_Motor();
        
        LEDS_Motor(0);
        LEDS_Motor(1);
        LEDS_Motor(2);
        LEDS_Motor(3);
        LEDS_Motor(4);
        LEDS_Motor(5);
        
        //motorRFIDcoop(); 
        motorRFIDcoop_NOU();
        
    }
}
