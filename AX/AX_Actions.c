  // read excel file for more information on register. cf 7robot/ technique / test_feetech
//feetech base id is equal to 1 can be change  on register 5
#include "main.h"

// INIT ///
uint8_t start_init_AX = 0; //1 si start au debut, 0 sinon
uint8_t state_init = 0;
uint8_t done_init = 0;
uint32_t Timer_AX_init = 0;


//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  INIT  ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


void Init_AX_Actions_Loop(void){
    
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////  LOOP ACTIONS  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void AX_Actions_Loop(void){
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////////  AUTRES FONCTIONS  /////////////////////////////
//////////////////////////////////////////////////////////////////////////////


uint8_t Send_AX_Func(void) {
    u32 val32;
    u8 Id;
    u8 Reg;
    u16 Consigne;
    if (Get_Param_u32(&val32))
        return PARAM_ERROR_CODE;
    Id = val32;
    if (Get_Param_u32(&val32))
        return PARAM_ERROR_CODE;
    Reg = val32;
    if (Get_Param_u32(&val32))
        return PARAM_ERROR_CODE;
    Consigne = val32;

    Put_AX12(Id, Reg, Consigne, NULL);

    return 0;
}

uint8_t Get_AX_Func(void) {
    u32 val32;
    u8 Id;
    u8 Reg;
    if (Get_Param_u32(&val32))
        return PARAM_ERROR_CODE;
    Id = val32;
    if (Get_Param_u32(&val32))
        return PARAM_ERROR_CODE;
    Reg = val32;

    printf("Read AX12 %d, Reg %d\n", Id, Reg);

    val32 = 0;
    Get_AX12(Id, Reg, &val32, NULL);
    Wait_For_All_AX_Cmd();
    
    printf("%ld\r\n", val32);

    return 0;
}

//-------------------------------------------------------------------------------
// Fonctions Search ID return all id detected
//-------------------------------------------------------------------------------

static uint8_t start_ID=0;  //Static pour que les variables ne soient pas visibles par le fichier STS_Actions
static uint8_t etat_ID=0;
static uint8_t done_ID=0;
static uint16_t ID_return=0;
static uint16_t ID_test=0;

void AX_Search_ID_Loop(void){ 
    //scan all id return done when finish
    switch(etat_ID){
        case 0:
            if(start_ID==1){
                start_ID=0;
                etat_ID ++;
            }
        break;

        case 1:
            if (ID_test<255){
                Get_AX12(ID_test, AX_ID, &ID_return, &done_ID);
                //printf("ID test: %d\n", ID_test);
                etat_ID ++;
            } else {
                printf("done research \n");
                ID_test=0;
                ID_return=0;
                etat_ID=0;
            }

            break;
        case 2:
            if(done_ID){
                done_ID = 0;   
                if(ID_return != 0){                       
                        printf("ID return: %d\n",ID_return);
                }
                ID_test++;
                ID_return=0;
                etat_ID=1;
                //printf("ID test: %d\n", ID_test);
            }               
            break;
    }
        
}



uint8_t Test_ID_AX_Func (void){
    start_ID=1;
    return 0;
}

uint8_t Move_AX_Func (void){
    u32 val32;
    u8 Id;
    u32 pos;
    if (Get_Param_u32(&val32))
        return PARAM_ERROR_CODE;
    Id = val32;
    if (Get_Param_u32(&val32))
        return PARAM_ERROR_CODE;
    pos = val32;
    printf("Move AX12 %d, to pos %d\n", Id, pos);
    Put_AX12(Id, AX_GOAL_POSITION, pos, NULL);
    return 0;

}


uint8_t Get_Pos_AX_Func (void){
    u32 val32;
    u8 Id;
    if (Get_Param_u32(&val32)){
        return PARAM_ERROR_CODE;
    }
    Id = val32;

    val32 = 0;
    Get_AX12(Id, AX_PRESENT_POSITION, &val32, NULL);
    Wait_For_All_AX_Cmd();
    
    printf("%ld\r\n", val32);

    return 0;
}

uint8_t init_AX_Func (void){
    start_init_AX = 1;
    return 0;
}

uint8_t speed_AX_Func(void){
    u32 val32;
    u8 Id;
    u32 speed;
    if (Get_Param_u32(&val32))
        return PARAM_ERROR_CODE;
    Id = val32;
    if (Get_Param_u32(&val32))
        return PARAM_ERROR_CODE;
    speed = val32;
    Put_AX12(Id, AX_MOVING_SPEED, speed, NULL);
    return 0;

}


