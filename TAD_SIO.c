#include "TAD_SIO.h"

void SIO_Init(void) {
    // Configurar pins TRISC6 y TRISC7 com entrades (EUSART)
    TRISCbits.TRISC6 = 1;  // TX entrada
    TRISCbits.TRISC7 = 1;  // RX entrada

    //SYNC = 0, BRG16 = 0, BRGH = 0
    // Actual Rate (K) = 9.615
    // Error = 0.16
    // SPBRG value = 64
    
    // Configuración de baud rate
    SPBRG = 64;
    
    BAUDCONbits.BRG16 = 0;
    TXSTAbits.BRGH = 0;
    
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
}

unsigned char SIO_Send (unsigned char character) {
    if (TXSTAbits.TRMT) {
        TXREG = character;
        return CERT;
    }
    return FALS;
}

unsigned char SIO_Receive(unsigned char* character) {
    if (PIR1bits.RCIF) {
        *character = RCREG;
        return CERT;
    }
    return FALS;
}


void SIO_SendString(const char *str) {
    while(*str) {
        if (SIO_Send(*str)) {
            str++;
        }
    }
}

