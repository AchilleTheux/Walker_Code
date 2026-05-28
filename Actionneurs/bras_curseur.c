#include "main.h"


uint8_t state_curseur_descente = 0;
uint8_t start_curseur_descente=0;
uint8_t state_curseur_montee = 0;
uint8_t start_curseur_montee=0;

#define POS_UP 4095
#define POS_DOWN 3142




uint8_t curseur_Init(void){
    Put_Feetech(ID_BRAS_CURSEUR, STS_TORQUE_ENABLE, 1, NULL);
    Put_Feetech(ID_BRAS_CURSEUR, STS_GOAL_POSITION,POS_UP, NULL);
    return 0;
}
uint8_t curseur_free(void){
    Put_Feetech(ID_BRAS_CURSEUR, STS_TORQUE_ENABLE, 0, NULL);
    return 0;
}



void curseur_Loop(void){
    switch(state_curseur_descente){
        case 0 :
            break;
        case 1 :
                state_curseur_descente++;
                Put_Feetech(ID_BRAS_CURSEUR, STS_GOAL_POSITION,POS_DOWN, NULL); //compression cagette
            break;
        case 2:
                state_curseur_descente=0;
                printf("bras descendu ");
        
    }
    switch(state_curseur_montee){
        case 0 :
            break;
        case 1 :
                state_curseur_montee++;
                Put_Feetech(ID_BRAS_CURSEUR, STS_GOAL_POSITION, POS_UP, NULL); //compression cagette
            
            break;
        case 2:
                state_curseur_montee=0;
                printf("bras montee ");
        
    }
}


uint8_t action_montee_bras_curseur (void){
    state_curseur_montee = 1;
    return 0;
}

uint8_t action_descendre_bras_curseur (void){
    state_curseur_descente= 1;
    return 0;
}


