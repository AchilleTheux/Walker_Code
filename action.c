#include "main.h"


/// TIMER MATCH ///
uint8_t Enable_Timer = 0; // Active ou non le Timer
uint8_t State_Timer_match = 1; // Possibilité de faire les actions
uint32_t Timer_start_match = 0; // Temps de match
uint32_t Timer_init=0;
extern uint8_t REG_AXSTS_ID ;
extern uint8_t REG_AXSTS_GOAL_POSITION;
extern uint8_t REG_AXSTS_PRESENT_POSITION;


uint8_t state_init_actionneur =0;

uint8_t start_init=1;

uint8_t done_cmd_feetech=0;
uint32_t pos_gap_g;
uint32_t pos_gap_d;

/// RETOURNE_N_LOOP//
uint32_t Timer_rotation = 0;

extern uint8_t state_retourne_n;
extern uint8_t vrai_done_barillet;
extern uint16_t sens;
extern u32 pos_barillet_future;
extern u32 nb_demi_section;

uint8_t state_retourne_n= 0;
uint8_t start_retourne_n=0;

uint8_t nombre_de_retounement  = 0;

void Action_init(void) {
    // Init des actions

}

void Action_Loop(void) {
}

uint8_t restart_match_Func (void){ //
    Enable_Timer = 0;
    State_Timer_match = 1;
    return 0;
}

uint8_t start_timer_match_Func (void){
    Enable_Timer = 1;
    Timer_start_match = Timer_ms1;
    State_Timer_match = 1;
    printf("Start Match\n");
    return 0;
}

void timer_match_Loop(void) {
    if (Enable_Timer) {
        if ((Timer_ms1 - Timer_start_match) > TIME_END_MATCH) { // 100s
            State_Timer_match = 0;
            Enable_Timer = 0;
            printf("End Match\n");
        }
    }
}


void Init_Actionneurs_Loop(void) {

    switch (state_init_actionneur)
    {
    case 0:
        if (start_init){
            state_init_actionneur = 1;
            start_init=0;
        }
        break;

    case 1: // init bras
        Bras_Demarrer_Init(&bras1);
        Bras_Demarrer_Init(&bras2);
        Timer_init = Timer_ms1;
        state_init_actionneur = 2;
        break;
    case 2:
        if ((Timer_ms1 - Timer_init) > 1000) {
            free_barillet();
            state_init_actionneur = 3;
        }
        break;
    case 3: // lire la position des serre-caisse (gauche_bas / droite_bas)
        done_cmd_feetech = 0;
        Get_Feetech(gauche_bas,STS_PRESENT_POSITION,&pos_gap_g,NULL);
        Get_Feetech(droite_bas,STS_PRESENT_POSITION,&pos_gap_d,&done_cmd_feetech);
        state_init_actionneur = 4;
        break;
    case 4:
        if(done_cmd_feetech){
            done_cmd_feetech=0;
            state_init_actionneur = 5;
        }
        break;
    case 5: // mettre les serre-caisse hors du chemin
        Put_Feetech(gauche_bas,STS_TORQUE_ENABLE,1,NULL);
        Put_Feetech(droite_bas,STS_TORQUE_ENABLE,1,NULL);
        if (pos_gap_g > POS_LB_GAP){
            Put_Feetech(gauche_bas,STS_GOAL_POSITION,POS_LB_DECOMPRES,NULL);
        }else{
            Put_Feetech(gauche_bas,STS_GOAL_POSITION,POS_LB_COMPRES,NULL);
        }
        if (pos_gap_d > POS_RB_GAP){
            Put_Feetech(droite_bas,STS_GOAL_POSITION,POS_RB_COMPRES,&done_cmd_feetech);
        }else{
            Put_Feetech(droite_bas,STS_GOAL_POSITION,POS_RB_DECOMPRES,&done_cmd_feetech);
        }
        Timer_init = Timer_ms1;
        state_init_actionneur = 6;
        break;
    case 6:
        if(done_cmd_feetech && (Timer_ms1 - Timer_init > 400)){
            done_cmd_feetech=0;
            state_init_actionneur = 7;
        }
        break;
    case 7: // sortir le retourneur (milieu_bas)
        Put_Feetech(milieu_bas,STS_TORQUE_ENABLE,1,NULL);
        Put_Feetech(milieu_bas, STS_GOAL_POSITION, POS_MILIEU_EXT_INIT, &done_cmd_feetech);
        Timer_init  = Timer_ms1;
        state_init_actionneur = 8;
        break;
    case 8:
        if(done_cmd_feetech && (Timer_ms1 - Timer_init > 1000)){
            done_cmd_feetech = 0;
            state_init_actionneur = 9;
        }
        break;
    case 9: // init barillet & guide
        vrai_done_barillet = 0;
        init_barillet();
        state_init_actionneur = 11;
        break;
    case 11:
        if (vrai_done_barillet && (etat_barillet == 0)) {
            done_cmd_feetech = 0;
            init_guide();
            curseur_Init();
            Retourne_Init();
            Put_Feetech(gauche_bas,STS_GOAL_POSITION,POS_LB_DECOMPRES,NULL);
            Put_Feetech(droite_bas,STS_GOAL_POSITION,POS_RB_DECOMPRES,&done_cmd_feetech);
            Timer_init = Timer_ms1;
            state_init_actionneur = 12;
        }
        break;
    case 12:
        if(done_cmd_feetech && (Timer_ms1 - Timer_init > 1000)){
            Put_Feetech(milieu_bas, STS_GOAL_POSITION, POS_MILIEU_INT, &done_cmd_feetech);
            Timer_init = Timer_ms1;
            state_init_actionneur = 13;
        }
        break;
    case 13:
        if(done_cmd_feetech && (Timer_ms1 - Timer_init > 1000)){
            done_cmd_feetech = 0;
            printf("init_actionneurs done\n");
            state_init_actionneur = 0;
        }
        break;
    default:
        state_init_actionneur=0;
        break;
        }

    }

    uint8_t init_actionneurs_Func(){
        start_init=1;
        return 0;
    }






    //et sortire en paralle le retourner caisse( à ce moment là il faut que les torque serre caisse soit à 0) )


    //monté les serre caisse



    //faire l'init du barillet


    // rentré

   //à n'importe qu'elle momment on peut placer les deux retourne caisse dans le bon endroit





uint8_t retourne_n_Loop(void) {

        switch (state_retourne_n) {
            case 0:
            if ( start_retourne_n){

                state_retourne_n++;
            }
            break;

            case 1:
                if (nombre_de_retounement<20) {
                    printf("  tour %d \n",nombre_de_retounement );
                    state_retourne_n++;
                }
                else{
                    state_retourne_n=0;
                    start_retourne_n=0;
                    nombre_de_retounement=0;
                }
                printf("%i",vrai_done_barillet);
                Timer_rotation = Timer_ms1;
                break;

            case 2:
                if(Timer_ms1 - Timer_rotation > 2000){ {
                    state_retourne_gauche = 1;
                    state_retourne_n++;
                    printf(" tourne_gauche \n");
                }
                break;

            case 3:
                if (state_retourne_gauche == 0) {
                    state_retourne_droite = 1;
                    printf(" tourne_droit \n");
                    state_retourne_n++;
                }
                break;

            case 4:
                if ((state_retourne_droite == 0) && (state_retourne_gauche == 0)) {
                    nb_demi_section = 1;
                    nb_demi_section %= 6;
                    if (!nb_demi_section) {
                        printf("Done !\n");
                        return 0;
                    }
                    if (nb_demi_section > 3) {
                        nb_demi_section = 6 - nb_demi_section;
                        sens = 1024;
                        pos_barillet_future -= nb_demi_section;
                    } else {
                        sens = 0;
                        pos_barillet_future += nb_demi_section;
                    }
                    dist_total = nb_demi_section * SECTION_ROTATION;
                    nb_demi_section = 2 * nb_demi_section - 1;
                    start_barillet = 1;


                    vrai_done_barillet = 0;
                    printf(" tourne_barillet \n");
                    state_retourne_n++;
                }
                break;

            case 5:
                nombre_de_retounement++;
                state_retourne_n = 1;
                break;

    }
    return 0;
}
}

uint8_t retourne_n(void){
    start_retourne_n=1;
    return 0;
}
