#include "TAD_MENU.h"

static unsigned char state_main_motor, state_option3;
static unsigned char start_system, esc_key, hash_less_3s;
static unsigned char option, hora_h, hora_l, minuts_h, minuts_l, enter;
static unsigned char someone;

static unsigned char actualitzar_lcd, quinLed, quinaIntensitat;
static unsigned char key;
static unsigned char state_setIntensity;


static unsigned char UID_CURRENT[5];
static unsigned char LEDS_CURRENT[6];
static unsigned char INDEX_CURRENT;

static unsigned char TimerHash;

void addHour(void);
unsigned char findLastLetterUID(unsigned char UID[5]);
void printLCD(void);
void handleEscKey(unsigned char received_char);
void handleMainMenu(void);
void handleIntensity(void);
void option3(unsigned char received_char);
void printHex(unsigned char value);
unsigned char checkUID(unsigned char *UID_detectat);
void showUID(unsigned char index);
void showConfigLeds(unsigned char index);
unsigned char mateixaTargeta(unsigned char UID[5]);

void MENU_Init(void) {
    state_main_motor = state_option3 = 
    actualitzar_lcd = 
    someone = esc_key = hash_less_3s = 
    hora_h = hora_l = minuts_h = minuts_l = key = 0;
    state_setIntensity = 0;
    start_system = 1;
    
    TI_NewTimer(&TimerHash);
}

void MENU_Motor(void) {
    unsigned char received_char = 0;
    
    SIO_Receive(&received_char);
        
    handleEscKey(received_char);
    handleIntensity();
    handleMainMenu();
    
    switch(state_main_motor) {
        case 0:            
            if(received_char == '1' || received_char == '2' || received_char == '3') {
                SIO_Send(received_char);
                option = received_char;
                received_char = 0;
                state_main_motor++;
                state_option3 = 0;
            }
            
            break;
            
        case 1:
            switch(option) {
                case '1':
                    if(someone) {
                        showUID(INDEX_CURRENT);
                    } else {
                        SIO_SendString(res_a_mostrar);
                    }
                    state_main_motor++;
                    break;
                case '2':
                    for(int i = 1; i < NUM_CONFIGS; i++) {
                        showUID((char)i);
                        showConfigLeds((char)i);
                        SIO_SendString("\r\n");
                    } 
                    state_main_motor++;
                    break;
                case '3':
                    option3(received_char);
                    break;
            }
            break;
        case 2:
            SIO_SendString("\r\n\r\nOpció: ");
            received_char = 0;
            state_main_motor = 0;
            break;
    }
}

void MENU_MotorHandleHash(void) {
    static unsigned char state_hash = 0;
    static unsigned char bug_timer = 0;
    
    switch(state_hash) {
        case 0:
            if(TM_IsKeyPressed()) {
                TI_ResetTics(TimerHash);
                state_hash++;
            }
            break;
        case 1:
            // BUG no em deixa comptar fins a 1500
            if(TI_GetTics(TimerHash) >= 500/*T_3S*/) {
                TI_ResetTics(TimerHash);
                bug_timer++;
            }
            if(!TM_IsKeyPressed()) {
                TM_GetTecla(&key);
                if (key == '#') {
                    state_hash++;
                } else {
                    state_hash--;
                }
            }
            break;
        case 2:
            if(bug_timer < 3) {
                hash_less_3s = 1;
                state_main_motor = 0; 
            } else {
                for(int i = 0; i < NUM_CONFIGS; i++) {
                    for (int j = 0; j < N_LEDS; j++) {
                        leds_saved[i][j] = 0;
                        LEDS_CURRENT[j] = 0;
                    }
                }  
                if (someone) actualitzar_lcd = 1;
            }
            state_hash = bug_timer = 0;
            break;
    }
}

void MENU_MotorHandleSetIntensity(void) {
    switch (state_setIntensity) {
        case 0:
            if (TM_IsKeyPressed()) {
                state_setIntensity++;
            }
            break;
        case 1:
            if (!TM_IsKeyPressed()) {
                state_setIntensity++;
            }
            break;
        case 2:
            TM_GetTecla(&key);
            if (someone && key >= '1' && key <= '6') {
                quinLed = key - '0' - 1;
                state_setIntensity++;
            } else {
                state_setIntensity = 0;
            }
            break;
        case 3:
            if (TM_IsKeyPressed()) {
                state_setIntensity++;
            }
            break;
        case 4:
            if (!TM_IsKeyPressed()) {
                state_setIntensity++;
            }
            break;
        case 5:
            TM_GetTecla(&key);
            if (someone && ((key >= '0' && key <= '9') || key == '*')) {
                quinaIntensitat = (key == '*') ? 10 : key - '0';
                leds_saved[INDEX_CURRENT][quinLed] = quinaIntensitat;
                LEDS_CURRENT[quinLed] = quinaIntensitat;
                LEDS_setIntensitat(quinLed, quinaIntensitat);
                state_setIntensity = 0;
                actualitzar_lcd = 1;
            }
            break;
    }
}


void MENU_MotorActualitzarLCD(void) {
    if(actualitzar_lcd) {
        actualitzar_lcd = 0;
        printLCD();
    }
}

void MENU_addMinute(void) {
    actualitzar_lcd = 1;
    if (minuts_l == 9) {
        minuts_l = 0;
        if (minuts_h == 5) {
            minuts_h = 0;
            addHour();
        } else {
            minuts_h++;
        }
    } else {
        minuts_l++;
    }
}

void addHour(void) {
    if (hora_l == 9) {
        hora_l = 0;
        if (hora_h == 2) {
            hora_h = 0;
        } else {
            hora_h++;
        }
    } else if (hora_h == 2 && hora_l == 3) {  
        hora_h = 0;
        hora_l = 0;
    } else {
        hora_l++;
    }
}


void handleEscKey(unsigned char received_char) {
    if (received_char == ESC_KEY) {
        esc_key = 1;
        state_main_motor = 0;
        received_char = 0;
    }
}

void handleMainMenu(void) {
    if(start_system || esc_key || hash_less_3s) {
        start_system = esc_key = hash_less_3s = 0;
        //key = 0;
        SIO_SendString(menu_principal);
    }
}

void handleIntensity(void) {
    if(!someone) {
        LEDS_setIntensitat(0, 0);
        LEDS_setIntensitat(1, 0);
        LEDS_setIntensitat(2, 0);
        LEDS_setIntensitat(3, 0);
        LEDS_setIntensitat(4, 0);
        LEDS_setIntensitat(5, 0);
    } else {
        LEDS_setIntensitat(0, LEDS_CURRENT[0]);
        LEDS_setIntensitat(1, LEDS_CURRENT[1]);
        LEDS_setIntensitat(2, LEDS_CURRENT[2]);
        LEDS_setIntensitat(3, LEDS_CURRENT[3]);
        LEDS_setIntensitat(4, LEDS_CURRENT[4]);
        LEDS_setIntensitat(5, LEDS_CURRENT[5]);
    }
}

void option3(unsigned char received_char) {    
    switch(state_option3) {
        case 0:
            SIO_SendString(hora_sistema);
            state_option3++;
            break;
        case 1:
            if (received_char >= '0' && received_char <= '9') {
                hora_h = received_char - '0';
                SIO_Send(received_char);
                state_option3++;
            }
            break;
        case 2:
            if (received_char >= '0' && received_char <= '9') {
                hora_l = received_char - '0';
                SIO_Send(received_char);
                state_option3++;
            }
            break;
        case 3:
            SIO_SendString(separador_hora);
            state_option3++;
            break;
        case 4:
            if (received_char >= '0' && received_char <= '9') {
                minuts_h = received_char - '0';
                SIO_Send(received_char);
                state_option3++;
            }
            break;
        case 5:
            if (received_char >= '0' && received_char <= '9') {
                minuts_l = received_char - '0';
                SIO_Send(received_char);
                state_option3++;
            }
            break;
        case 6:
            SIO_SendString(enter_aplicar);
            state_option3++;
            break;
        case 7:
            if (received_char == ENTER) {
                SIO_SendString(canvis_aplicats);
                state_option3++;
            }
            break;
        case 8:
            if(someone) actualitzar_lcd = 1;
            state_main_motor = 2;
            state_option3 = 0;
            break;
    }
}

void MENU_SaveUID(unsigned char UID[5]) {    
    unsigned char index = checkUID(UID);
    if (index != 255) {
        SIO_SendString("\r\n\r\nTargeta detectada!");
        
        if (someone && mateixaTargeta(UID)) {
            someone = 0;
            SIO_SendString("\r\nUsuari surt.");
            LcClear();
        } else {
            SIO_SendString("\r\nNou usuari.");
            for (int i = 0; i < UID_LENGTH; i++) {
                UID_CURRENT[i] = uid_saved[index][i];
            }
            for (int i = 0; i < 6; i++) {
                LEDS_CURRENT[i] = leds_saved[index][i];
            }
            someone = 1;
            INDEX_CURRENT = index;
            
            for (int i = 0; i < 6; i++) {
                LEDS_setIntensitat((char) i, LEDS_CURRENT[i]);
            }
            
            actualitzar_lcd = 1;
        }

        showUID(INDEX_CURRENT);
        showConfigLeds(INDEX_CURRENT);
    } else {
        SIO_SendString("\r\n\r\nTargeta NO reconeguda!\r\n\tUID: ");
        
        for (int i = 0; i < UID_LENGTH; i++) {
            printHex(UID[i]);
            if (i < UID_LENGTH - 1) SIO_SendString("-");
        }
    }
    SIO_SendString("\r\n\r\nOpció: ");
}

void printHex(unsigned char value) {
    const char hex_chars[] = "0123456789ABCDEF";
    char hex_out[3];
    
    hex_out[0] = hex_chars[(value >> 4) & 0x0F];
    hex_out[1] = hex_chars[value & 0x0F];
    hex_out[2] = '\0';

    SIO_SendString(hex_out);
}

unsigned char checkUID(unsigned char *UID_detectat) {
    for (int i = 0; i < NUM_CONFIGS; i++) {
        int match = 1;
        for (int j = 0; j < UID_LENGTH; j++) {
            if (UID_detectat[j] != uid_saved[i][j]) {
                match = 0;
                break;
            }
        }

        if (match) {
            return (char) i;
        }
    }
    return 255;
}

void showUID(unsigned char index) {
    SIO_SendString("\r\n\tUID: ");
    printHex(uid_saved[index][0]);
    SIO_SendString("-");
    printHex(uid_saved[index][1]);
    SIO_SendString("-");
    printHex(uid_saved[index][2]);
    SIO_SendString("-");
    printHex(uid_saved[index][3]);
    SIO_SendString("-");
    printHex(uid_saved[index][4]);
}

void showConfigLeds(unsigned char index) {
    char buffer[4];

    SIO_SendString("\r\n\t");
    
    for (unsigned int i = 0; i < N_LEDS; i++) {
        if (i > 0) {
            SIO_SendString(" - ");
        }

        buffer[0] = 'L';
        buffer[1] = (char) i + '0';
        buffer[2] = ':';
        buffer[3] = '\0';

        SIO_SendString(buffer);

        buffer[0] = (leds_saved[index][i] == 10) ? 'A' : (leds_saved[index][i] + '0');
        buffer[1] = '\0';

        SIO_SendString(buffer);
    }
}

unsigned char mateixaTargeta(unsigned char UID[5]) {
    for (int i = 0; i < UID_LENGTH; i++) {
        if (UID[i] != uid_saved[INDEX_CURRENT][i]) {
            return FALS;
        }
    }
    return CERT;
}

unsigned char findLastLetterUID(unsigned char UID[5]) {
    for (int i = 4; i >= 0; i--) {
        unsigned char highNibble = (UID[i] >> 4) & 0x0F;
        unsigned char lowNibble = UID[i] & 0x0F;

        if (lowNibble >= 10) {
            return 'A' + (lowNibble - 10);
        }

        if (highNibble >= 10) {
            return 'A' + (highNibble - 10);
        }
    }
    return '0';
}

void printLCD(void) {
    //LcCursorOn();

    LcClear(); 

    LcGotoXY(0, 0);
    LcPutChar(findLastLetterUID(UID_CURRENT)); LcPutChar(' ');
    LcPutChar(hora_h + '0'); LcPutChar(hora_l + '0');
    LcPutChar(':');
    LcPutChar(minuts_h + '0'); LcPutChar(minuts_l + '0');
    LcPutChar(' ');
    LcPutChar('1'); LcPutChar('-'); 
    LcPutChar((LEDS_CURRENT[0] == 10) ? 'A' : (LEDS_CURRENT[0] + '0'));
    LcPutChar(' ');
    LcPutChar('2'); LcPutChar('-'); 
    LcPutChar((LEDS_CURRENT[1] == 10) ? 'A' : (LEDS_CURRENT[1] + '0'));

    LcGotoXY(0, 1);
    LcPutChar('3'); LcPutChar('-'); 
    LcPutChar((LEDS_CURRENT[2] == 10) ? 'A' : (LEDS_CURRENT[2] + '0'));
    LcPutChar(' ');
    LcPutChar('4'); LcPutChar('-'); 
    LcPutChar((LEDS_CURRENT[3] == 10) ? 'A' : (LEDS_CURRENT[3] + '0'));
    LcPutChar(' ');
    LcPutChar('5'); LcPutChar('-'); 
    LcPutChar((LEDS_CURRENT[4] == 10) ? 'A' : (LEDS_CURRENT[4] + '0'));
    LcPutChar(' ');
    LcPutChar('6'); LcPutChar('-'); 
    LcPutChar((LEDS_CURRENT[5] == 10) ? 'A' : (LEDS_CURRENT[5] + '0'));
}