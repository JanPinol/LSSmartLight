#include "TAD_TECLATMATRIU.h"

const char TECLAT_MATRIU[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

static unsigned char state;
unsigned char key, hash, row_selected, col_selected, index_row;

static unsigned char TimerRebots;

void TM_Init(void) {
    TI_NewTimer(&TimerRebots);
    
    TRIS_COLUMN0 = TRIS_COLUMN1 = TRIS_COLUMN2 = 1;     // COLUMNS INPUT
    TRIS_ROW0 = TRIS_ROW1 = TRIS_ROW2 = TRIS_ROW3 = 0;  // ROWS OUTPUT
    ROW0 = ROW1 = ROW2 = ROW3 = 0;
    index_row = state = hash = 0;
}

void TM_Motor(void) {
    switch(state) {
        case 0:
            ROW1 = ROW2 = ROW3 = 0;
            ROW0 = 1;
            row_selected = 0;
            state = 4;
            break;
            
        case 1:
            ROW0 = ROW2 = ROW3 = 0;
            ROW1 = 1;
            row_selected = 1;
            state = 4;
            break;
        
        case 2:
            ROW0 = ROW1 = ROW3 = 0;
            ROW2 = 1;
            row_selected = 2;
            state = 4;
            break;
            
        case 3:
            ROW0 = ROW1 = ROW2 = 0;
            ROW3 = 1;
            row_selected = 3;
            state = 4;
            break;
            
    case 4:
        if (COLUMN0 || COLUMN1 || COLUMN2) {
            TI_ResetTics(TimerRebots);
            state++;
        } else {
            index_row++;
            if (index_row >= 4) {
                index_row = 0;
            }
            state = index_row;
            
        }
        break;


        case 5:
            if (TI_GetTics(TimerRebots) >= T_REBOTS) {
                if (COLUMN0 || COLUMN1 || COLUMN2) { 
                    state++;
                } else {
                    state--;
                }
            }
            break;

        case 6:
            col_selected = 255;

            if (COLUMN0) {
                col_selected = 0;
            } else if (COLUMN1) {
                col_selected = 1;
            } else if (COLUMN2) {
                col_selected = 2;
            }

            if (col_selected != 255) {
                state++;
            } else {
                state = 4;
            }
            break;
            
        case 7:
            key = TECLAT_MATRIU[row_selected][col_selected];
            //SIO_Send(TECLAT_MATRIU[row_selected][col_selected]);
            //SIO_Send(key);
            if (!TM_IsKeyPressed()) state = 0;
            break;
    }
}

void TM_GetTecla(unsigned char *c) {
    *c = key;
    //key = 0;
}

unsigned char TM_IsKeyPressed(void) {
    return (COLUMN0 || COLUMN1 || COLUMN2);
}


