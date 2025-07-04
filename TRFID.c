#include "TRFID.h"

void MFRC522_Wr_1(unsigned char Address);
void MFRC522_Wr_2(unsigned char value);
void MFRC522_CRC_NOU(char *dataIn, char length, char *dataOut);
char MFRC522_ToCard_NOU(char command, char *sendData, char sendLen, char *backData, unsigned *backLen);

char* TagType;
unsigned char t;
static char state;
static char _status = 0;

unsigned char i;

void InitPortDirections() {
    DIR_MFRC522_SO  = 1; 
    DIR_MFRC522_SI  = 0; 
    DIR_MFRC522_SCK = 0; 
    DIR_MFRC522_CS  = 0; 
    DIR_MFRC522_RST = 0;
}

void delay_us(char howMany) {
    #define NUM_US 16
    char x;
    for (x = 0; x < howMany * NUM_US; x++) Nop();
}

void RFID_Init() {
    InitPortDirections();
    MFRC522_Init();
}

void MFRC522_Init() {
    MFRC522_CS = 1; 
    MFRC522_RST = 1;

    MFRC522_Reset();       
    MFRC522_Wr(TMODEREG, 0x8D);
    MFRC522_Wr(TPRESCALERREG, 0x3E);
    MFRC522_Wr(TRELOADREGL, 30);
    MFRC522_Wr(TRELOADREGH, 0); 
    MFRC522_Wr(TXAUTOREG, 0x40);
    MFRC522_Wr(MODEREG, 0x3D);

    MFRC522_AntennaOff();            
    MFRC522_AntennaOn();
}

void MFRC522_Clear_Bit(char addr, char mask) {
    unsigned char tmp = MFRC522_Rd(addr);
    MFRC522_Wr(addr, tmp & ~mask);
}

void MFRC522_Set_Bit(char addr, char mask) {
    unsigned char tmp = MFRC522_Rd(addr);
    MFRC522_Wr(addr, tmp | mask);
}

void MFRC522_Reset() {
    MFRC522_RST = 1;
    delay_us(1);
    MFRC522_RST = 0;
    delay_us(1);
    MFRC522_RST = 1;
    delay_us(1);
    MFRC522_Wr(COMMANDREG, PCD_RESETPHASE);
    delay_us(1);
}

void MFRC522_AntennaOn() {
    unsigned char stt = MFRC522_Rd(TXCONTROLREG);
    MFRC522_Set_Bit(TXCONTROLREG, 0x03);
}

void MFRC522_AntennaOff() {
    MFRC522_Clear_Bit(TXCONTROLREG, 0x03);
}

void MFRC522_CRC(char *dataIn, char length, char *dataOut) {
    static char step = 1;
    static char i = 0;
    static char n;

    switch (step) {
        case 1:
            MFRC522_Clear_Bit(DIVIRQREG, 0x04);
            MFRC522_Set_Bit(FIFOLEVELREG, 0x80);
            i = 0;
            step = 2;
            break;

        case 2:
            if (i < length) {
                MFRC522_Wr(FIFODATAREG, dataIn[i++]);
            } else {
                step = 3;
            }
            break;

        case 3:
            MFRC522_Wr(COMMANDREG, PCD_CALCCRC);
            step = 4;
            break;

        case 4:
            n = MFRC522_Rd(DIVIRQREG);
            if (n & 0x04) step = 5;
            break;

        case 5:
            dataOut[0] = MFRC522_Rd(CRCRESULTREGL);
            dataOut[1] = MFRC522_Rd(CRCRESULTREGM);
            step = 1;
            break;
    }
}

void MFRC522_CRC_NOU(char *dataIn, char length, char *dataOut) {
    static char step = 1;
    static char i = 0;
    static char n;
    static char wr_state = 0;

    switch (step) {
        case 1:
            MFRC522_Clear_Bit(DIVIRQREG, 0x04);
            MFRC522_Set_Bit(FIFOLEVELREG, 0x80);
            i = 0;
            step = 2;
            break;

        case 2:
            if (i < length) {
                if (wr_state == 0) {
                    MFRC522_Wr_1(FIFODATAREG);
                    wr_state = 1;
                } else {
                    MFRC522_Wr_2(dataIn[i++]);
                    wr_state = 0;
                }
            } else {
                step = 3;
            }
            break;

        case 3:
            if (wr_state == 0) {
                MFRC522_Wr_1(COMMANDREG);
                wr_state = 1;
            } else {
                MFRC522_Wr_2(PCD_CALCCRC);
                wr_state = 0;
                step = 4;
            }
            break;

        case 4:
            n = MFRC522_Rd(DIVIRQREG);
            if (n & 0x04) step = 5;
            break;

        case 5:
            dataOut[0] = MFRC522_Rd(CRCRESULTREGL);
            dataOut[1] = MFRC522_Rd(CRCRESULTREGM);
            step = 1;
            break;
    }
}

char MFRC522_ReadCardSerial(unsigned char *str) {
    char _status = MFRC522_AntiColl((char *)str);
    str[5] = 0;
    return (_status == MI_OK);
}

char MFRC522_ToCard(char command, char *sendData, char sendLen, char *backData, unsigned *backLen) {
    static char _status = MI_ERR;
    static char irqEn = 0x00;
    static char waitIRq = 0x00;
    static char lastBits;
    static char n;
    static unsigned char rd;
    static unsigned int j;
    static char state_aux = 0;
    static unsigned char tmp = 0x0;

    switch (state_aux) {
        case 0:
            switch (command) {
                case PCD_AUTHENT:
                    irqEn = 0x12;
                    waitIRq = 0x10;
                    break;
                case PCD_TRANSCEIVE:
                    irqEn = 0x77;
                    waitIRq = 0x30;
                    break;
                default:
                    break;
            }
            state_aux++;
            break;

        case 1:
            MFRC522_Wr(COMMIENREG, irqEn | 0x80);
            tmp = 0x0;
            state_aux++;
            break;

        case 2:
            tmp = MFRC522_Rd(COMMIRQREG);
            state_aux++;
            break;

        case 3:
            MFRC522_Wr(COMMIRQREG, tmp & ~0x80);
            tmp = 0x0;
            state_aux++;
            break;

        case 4:
            tmp = MFRC522_Rd(FIFOLEVELREG);
            state_aux++;
            break;

        case 5:
            MFRC522_Wr(FIFOLEVELREG, tmp | 0x80);
            state_aux++;
            break;

        case 6:
            MFRC522_Wr(COMMANDREG, PCD_IDLE);
            j = 0;
            state_aux++;
            break;

        case 7:
            if (j < sendLen) {
                MFRC522_Wr(FIFODATAREG, sendData[j++]);
            } else {
                state_aux++;
            }
            break;

        case 8:
            MFRC522_Wr(COMMANDREG, command);
            state_aux++;
            break;

        case 9:
            if (command == PCD_TRANSCEIVE) {
                tmp = 0x0;
                state_aux = 10;
            } else {
                state_aux = 12;
            }
            break;

        case 10:
            tmp = MFRC522_Rd(BITFRAMINGREG);
            state_aux++;
            break;

        case 11:
            MFRC522_Wr(BITFRAMINGREG, tmp | 0x80);
            state_aux++;
            break;

        case 12:
            j = 0xFFFF;
            state_aux++;
            break;

        case 13:
            n = MFRC522_Rd(COMMIRQREG);
            state_aux++;
            break;

        case 14:
            j--;
            if (j && !(n & 0x01) && !(n & waitIRq)) {
                state_aux--;
            } else {
                state_aux++;
                tmp = 0x0;
            }
            break;

        case 15:
            tmp = MFRC522_Rd(BITFRAMINGREG);
            state_aux++;
            break;

        case 16:
            MFRC522_Wr(BITFRAMINGREG, tmp & ~0x80);
            state_aux++;
            break;

        case 17:
            if (j != 0) state_aux++;
            else state_aux = 28;
            break;

        case 18:
            rd = MFRC522_Rd(ERRORREG) & 0x1B;
            state_aux++;
            break;

        case 19:
            if (!rd) {
                _status = MI_OK;
                state_aux++;
            } else {
                _status = MI_ERR;
                state_aux = 28;
            }
            break;

        case 20:
            if ((n & irqEn & 0x01)) {
                _status = MI_NOTAGERR;
            }
            state_aux++;
            break;

        case 21:
            if (command == PCD_TRANSCEIVE) state_aux++;
            else state_aux = 28;
            break;

        case 22:
            n = MFRC522_Rd(FIFOLEVELREG);
            state_aux++;
            break;

        case 23:
            lastBits = MFRC522_Rd(CONTROLREG) & 0x07;
            state_aux++;
            break;

        case 24:
            if (lastBits) *backLen = (n - 1) * 8 + lastBits;
            else *backLen = n * 8;
            if (n == 0) n = 1;
            else if (n > 16) n = 16;
            j = 0;
            state_aux++;
            break;

        case 25:
            if (j < n) {
                state_aux++;
            } else {
                backData[j] = 0;
                state_aux = 28;
            }
            break;

        case 26:
            backData[j++] = MFRC522_Rd(FIFODATAREG);
            state_aux = 25;
            break;

        case 28:
            irqEn = 0x00;
            waitIRq = 0x00;
            tmp = 0x0;
            state_aux = 0;
            state++;
            return _status;
    }
    return _status;
}

char MFRC522_ToCard_NOU(char command, char *sendData, char sendLen, char *backData, unsigned *backLen) {
    static char _status = MI_ERR;
    static char irqEn = 0x00;
    static char waitIRq = 0x00;
    static char lastBits;
    static char n;
    static unsigned char rd;
    static unsigned int j;
    static char state_aux = 0;
    static unsigned char tmp = 0x0;
    static char wr_state = 0;

    switch (state_aux) {
        case 0:
            switch (command) {
                case PCD_AUTHENT:
                    irqEn = 0x12;
                    waitIRq = 0x10;
                    break;
                case PCD_TRANSCEIVE:
                    irqEn = 0x77;
                    waitIRq = 0x30;
                    break;
            }
            state_aux++;
            break;

        case 1:
            if (wr_state == 0) {
                MFRC522_Wr_1(COMMIENREG);
                wr_state = 1;
            } else {
                MFRC522_Wr_2(irqEn | 0x80);
                wr_state = 0;
                tmp = 0;
                state_aux++;
            }
            break;

        case 2:
            tmp = MFRC522_Rd(COMMIRQREG);
            state_aux++;
            break;

        case 3:
            if (wr_state == 0) {
                MFRC522_Wr_1(COMMIRQREG);
                wr_state = 1;
            } else {
                MFRC522_Wr_2(tmp & ~0x80);
                wr_state = 0;
                tmp = 0;
                state_aux++;
            }
            break;

        case 4:
            tmp = MFRC522_Rd(FIFOLEVELREG);
            state_aux++;
            break;

        case 5:
            if (wr_state == 0) {
                MFRC522_Wr_1(FIFOLEVELREG);
                wr_state = 1;
            } else {
                MFRC522_Wr_2(tmp | 0x80);
                wr_state = 0;
                state_aux++;
            }
            break;

        case 6:
            if (wr_state == 0) {
                MFRC522_Wr_1(COMMANDREG);
                wr_state = 1;
            } else {
                MFRC522_Wr_2(PCD_IDLE);
                wr_state = 0;
                j = 0;
                state_aux++;
            }
            break;

        case 7:
            if (j < sendLen) {
                if (wr_state == 0) {
                    MFRC522_Wr_1(FIFODATAREG);
                    wr_state = 1;
                } else {
                    MFRC522_Wr_2(sendData[j++]);
                    wr_state = 0;
                }
            } else {
                state_aux++;
            }
            break;

        case 8:
            if (wr_state == 0) {
                MFRC522_Wr_1(COMMANDREG);
                wr_state = 1;
            } else {
                MFRC522_Wr_2(command);
                wr_state = 0;
                state_aux++;
            }
            break;

        case 9:
            state_aux = (command == PCD_TRANSCEIVE) ? 10 : 12;
            break;

        case 10:
            tmp = MFRC522_Rd(BITFRAMINGREG);
            state_aux++;
            break;

        case 11:
            if (wr_state == 0) {
                MFRC522_Wr_1(BITFRAMINGREG);
                wr_state = 1;
            } else {
                MFRC522_Wr_2(tmp | 0x80);
                wr_state = 0;
                state_aux++;
            }
            break;

        case 12:
            j = 0xFFFF;
            state_aux++;
            break;

        case 13:
            n = MFRC522_Rd(COMMIRQREG);
            state_aux++;
            break;

        case 14:
            j--;
            if (j && !(n & 0x01) && !(n & waitIRq)) {
                state_aux--;
            } else {
                tmp = 0;
                state_aux++;
            }
            break;

        case 15:
            tmp = MFRC522_Rd(BITFRAMINGREG);
            state_aux++;
            break;

        case 16:
            if (wr_state == 0) {
                MFRC522_Wr_1(BITFRAMINGREG);
                wr_state = 1;
            } else {
                MFRC522_Wr_2(tmp & ~0x80);
                wr_state = 0;
                state_aux++;
            }
            break;

        case 17:
            state_aux = (j != 0) ? 18 : 28;
            break;

        case 18:
            rd = MFRC522_Rd(ERRORREG) & 0x1B;
            state_aux++;
            break;

        case 19:
            if (!rd) {
                _status = MI_OK;
                state_aux++;
            } else {
                _status = MI_ERR;
                state_aux = 28;
            }
            break;

        case 20:
            if ((n & irqEn & 0x01)) {
                _status = MI_NOTAGERR;
            }
            state_aux++;
            break;

        case 21:
            state_aux = (command == PCD_TRANSCEIVE) ? 22 : 28;
            break;

        case 22:
            n = MFRC522_Rd(FIFOLEVELREG);
            state_aux++;
            break;

        case 23:
            lastBits = MFRC522_Rd(CONTROLREG) & 0x07;
            state_aux++;
            break;

        case 24:
            *backLen = (lastBits) ? (n - 1) * 8 + lastBits : n * 8;
            if (n == 0) n = 1;
            else if (n > 16) n = 16;
            j = 0;
            state_aux++;
            break;

        case 25:
            state_aux = (j < n) ? 26 : 28;
            if (j >= n) backData[j] = 0;
            break;

        case 26:
            backData[j++] = MFRC522_Rd(FIFODATAREG);
            state_aux = 25;
            break;

        case 28:
            irqEn = waitIRq = tmp = 0;
            state_aux = 0;
            state++;
            return _status;
    }

    return _status;
}

unsigned char MFRC522_Rd(unsigned char Address) {
    unsigned int i, ucAddr;
    unsigned int ucResult = 0;

    MFRC522_SCK = 0;
    MFRC522_CS = 0;
    ucAddr = ((Address << 1) & 0x7E) | 0x80;

    for (i = 8; i > 0; i--) {
        MFRC522_SI = ((ucAddr & 0x80) == 0x80);
        MFRC522_SCK = 1;
        ucAddr <<= 1;
        MFRC522_SCK = 0;
    }

    for (i = 8; i > 0; i--) {
        MFRC522_SCK = 1;
        ucResult <<= 1;
        ucResult |= MFRC522_SO;
        MFRC522_SCK = 0;
    }

    MFRC522_CS = 1;
    MFRC522_SCK = 1;
    return ucResult;
}

void MFRC522_Wr(unsigned char Address, unsigned char value) {
    unsigned char i, ucAddr;

    MFRC522_SCK = 0;
    MFRC522_CS = 0;
    ucAddr = ((Address << 1) & 0x7E);

    for (i = 8; i > 0; i--) {
        MFRC522_SI = ((ucAddr & 0x80) == 0x80);
        MFRC522_SCK = 1;
        ucAddr <<= 1;
        delay_us(3);
        MFRC522_SCK = 0;
        delay_us(3);
    }

    for (i = 8; i > 0; i--) {
        MFRC522_SI = ((value & 0x80) == 0x80);
        MFRC522_SCK = 1;
        value <<= 1;
        delay_us(3);
        MFRC522_SCK = 0;
        delay_us(3);
    }

    MFRC522_CS = 1;
    MFRC522_SCK = 1;
}

void MFRC522_Wr_1(unsigned char Address) {
    static unsigned char i, ucAddr;

    MFRC522_SCK = 0;
    MFRC522_CS = 0;
    ucAddr = ((Address << 1) & 0x7E);

    for (i = 8; i > 0; i--) {
        MFRC522_SI = ((ucAddr & 0x80) == 0x80);
        MFRC522_SCK = 1;
        ucAddr <<= 1;
        delay_us(3);
        MFRC522_SCK = 0;
        delay_us(3);
    }
}

void MFRC522_Wr_2(unsigned char value) {
    static unsigned char i;

    for (i = 8; i > 0; i--) {
        MFRC522_SI = ((value & 0x80) == 0x80);
        MFRC522_SCK = 1;
        value <<= 1;
        delay_us(3);
        MFRC522_SCK = 0;
        delay_us(3);
    }

    MFRC522_CS = 1;
    MFRC522_SCK = 1;
}


void motorRFIDcoop(void) {
    static unsigned backBits;
    static char buff[4];
    static unsigned char UID[5];
    static unsigned unLen;
    char i;
    char serNumCheck = 0;

    switch (state) {
        case 0:
            MFRC522_Wr(BITFRAMINGREG, 0x07);
            TagType[0] = PICC_REQIDL;
            state++;
            break;

        case 1:
            _status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
            break;

        case 2:
            if ((_status != MI_OK) || (backBits != 0x10)) {
                state = 0;
            } else {
                state++;
            }
            break;

        case 3:
            MFRC522_Wr(BITFRAMINGREG, 0x00);
            UID[0] = PICC_ANTICOLL;
            UID[1] = 0x20;
            MFRC522_Clear_Bit(STATUS2REG, 0x08);
            state++;
            break;

        case 4:
            _status = MFRC522_ToCard(PCD_TRANSCEIVE, UID, 2, UID, &unLen);
            break;

        case 5:
            if (_status == MI_OK) {
                for (i = 0; i < 4; i++) serNumCheck ^= UID[i];
                if (serNumCheck != UID[4]) _status = MI_ERR;
            }
            UID[5] = 0;

            if (_status == MI_OK) state++;
            else state = 7;
            break;

        case 6:
            MENU_SaveUID(UID);
            state++;
            break;

        case 7:
            buff[0] = PICC_HALT;
            buff[1] = 0;
            MFRC522_CRC(buff, 2, &buff[2]);
            MFRC522_Clear_Bit(STATUS2REG, 0x80);
            state++;
            break;

        case 8:
            _status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
            break;

        case 9:
            MFRC522_Clear_Bit(STATUS2REG, 0x08);
            state = 0;
            break;
    }
}

void motorRFIDcoop_NOU(void) {
    static unsigned backBits;
    static char buff[4];
    static unsigned char UID[5];
    static unsigned unLen;
    char i;
    char serNumCheck = 0;
    static char wr_state = 0;

    switch (state) {
        case 0:
            if (wr_state == 0) {
                MFRC522_Wr_1(BITFRAMINGREG);
                wr_state = 1;
            } else {
                MFRC522_Wr_2(0x07);
                wr_state = 0;
                TagType[0] = PICC_REQIDL;
                state++;
            }
            break;

        case 1:
            _status = MFRC522_ToCard_NOU(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
            break;

        case 2:
            if ((_status != MI_OK) || (backBits != 0x10)) {
                state = 0;
            } else {
                state++;
            }
            break;

        case 3:
            if (wr_state == 0) {
                MFRC522_Wr_1(BITFRAMINGREG);
                wr_state = 1;
            } else {
                MFRC522_Wr_2(0x00);
                wr_state = 0;
                UID[0] = PICC_ANTICOLL;
                UID[1] = 0x20;
                MFRC522_Clear_Bit(STATUS2REG, 0x08);
                state++;
            }
            break;

        case 4:
            _status = MFRC522_ToCard_NOU(PCD_TRANSCEIVE, UID, 2, UID, &unLen);
            break;

        case 5:
            if (_status == MI_OK) {
                for (i = 0; i < 4; i++) serNumCheck ^= UID[i];
                if (serNumCheck != UID[4]) _status = MI_ERR;
            }
            UID[5] = 0;

            if (_status == MI_OK) state++;
            else state = 7;
            break;

        case 6:
            MENU_SaveUID(UID);
            state++;
            break;

        case 7:
            buff[0] = PICC_HALT;
            buff[1] = 0;
            MFRC522_CRC_NOU(buff, 2, &buff[2]);
            MFRC522_Clear_Bit(STATUS2REG, 0x80);
            state++;
            break;

        case 8:
            _status = MFRC522_ToCard_NOU(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
            break;

        case 9:
            MFRC522_Clear_Bit(STATUS2REG, 0x08);
            state = 0;
            break;
    }
}


