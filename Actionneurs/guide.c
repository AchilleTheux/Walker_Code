#include "main.h"

u8 IDS_FEETECH_POUSSE[2] = { 40 ,41};

uint32_t Timer_bras = 0;


u8 start_bras=0;
u8 state_bras=0;
uint8_t done_guide_tx = 0;
uint8_t done_guide_poll = 0;
const char *guide_action_name = "idle";
uint8_t guide_track_left = 0;
uint8_t guide_track_right = 0;
uint16_t guide_target_left = 0;
uint16_t guide_target_right = 0;
uint16_t guide_pos_left = 0;
uint16_t guide_pos_right = 0;
uint32_t guide_move_start_ms = 0;

static uint8_t guide_output_code_from_sts(uint8_t sts_status) {
    if (sts_status == STS_STATUS_OK) return 0;
    return sts_status;
}

static void print_guide_status(const char *action_name, uint8_t output_code) {
    printf("guide %s %u\n", action_name, output_code);
}

static uint16_t guide_abs_diff_u16(uint16_t a, uint16_t b) {
    return (a > b) ? (a - b) : (b - a);
}

static void guide_prepare_motion(const char *action_name,
                                 uint8_t track_left,
                                 uint16_t target_left,
                                 uint8_t track_right,
                                 uint16_t target_right) {
    guide_action_name = action_name;
    guide_track_left = track_left;
    guide_track_right = track_right;
    guide_target_left = target_left;
    guide_target_right = target_right;
    done_guide_tx = 0;
    done_guide_poll = 0;
    guide_move_start_ms = Timer_ms1;
}

static void guide_finish_action(uint8_t output_code) {
    print_guide_status(guide_action_name, output_code);
    state_bras = 0;
    guide_track_left = 0;
    guide_track_right = 0;
    done_guide_tx = 0;
    done_guide_poll = 0;
}


uint8_t guide_Loop(void) {
    switch (state_bras) {
        case 0: // État de repos : on attend un ordre
            if (start_bras != 0) {
                state_bras = start_bras; // On change d'état vers l'action demandée
                start_bras = 0;          
            }
            break;

        case bras_gauche_open:
            guide_prepare_motion("openbrasg", 1, POS_GAUCHE_OPEN, 0, 0);
            Put_Feetech(IDS_FEETECH_POUSSE[0], STS_GOAL_POSITION, POS_GAUCHE_OPEN, &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            break;

        case bras_droite_open:
            guide_prepare_motion("openbrasd", 0, 0, 1, POS_DROITE_OPEN);
            Put_Feetech(IDS_FEETECH_POUSSE[1], STS_GOAL_POSITION, POS_DROITE_OPEN, &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            break;

        case bras_gauche_close:
            guide_prepare_motion("closebrasg", 1, POS_GAUCHE_CLOSE, 0, 0);
            Put_Feetech(IDS_FEETECH_POUSSE[0], STS_GOAL_POSITION, POS_GAUCHE_CLOSE, &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            break;

        case bras_droite_close:
            guide_prepare_motion("closebrasd", 0, 0, 1, POS_DROITE_CLOSE);
            Put_Feetech(IDS_FEETECH_POUSSE[1], STS_GOAL_POSITION, POS_DROITE_CLOSE, &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            break;
            
        case bras_free:
            guide_prepare_motion("freeguide", 0, 0, 0, 0);
            for (int i=0; i<2; ++i) Put_Feetech(IDS_FEETECH_POUSSE[i], STS_TORQUE_ENABLE, 0, i < 1 ? NULL : &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            break;
        case bras_gauche_open_max:
            guide_prepare_motion("openbrasgm", 1, POS_GAUCHE_OPEN_MAX, 0, 0);
            Put_Feetech(IDS_FEETECH_POUSSE[0], STS_GOAL_POSITION, POS_GAUCHE_OPEN_MAX, &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            break;

        case bras_droite_open_max:
            guide_prepare_motion("openbrasdm", 0, 0, 1, POS_DROITE_OPEN_MAX);
            Put_Feetech(IDS_FEETECH_POUSSE[1], STS_GOAL_POSITION, POS_DROITE_OPEN_MAX, &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            break;
        case bras_open :
            guide_prepare_motion("openbras", 1, POS_GAUCHE_OPEN, 1, POS_DROITE_OPEN);
            Put_Feetech(IDS_FEETECH_POUSSE[1], STS_GOAL_POSITION, POS_DROITE_OPEN, NULL);
            Put_Feetech(IDS_FEETECH_POUSSE[0], STS_GOAL_POSITION, POS_GAUCHE_OPEN, &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            
            break;
        case bras_close:
            guide_prepare_motion("closebras", 1, POS_GAUCHE_CLOSE, 1, POS_DROITE_CLOSE);
            Put_Feetech(IDS_FEETECH_POUSSE[1], STS_GOAL_POSITION, POS_DROITE_CLOSE, NULL);
            Put_Feetech(IDS_FEETECH_POUSSE[0], STS_GOAL_POSITION, POS_GAUCHE_CLOSE, &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            break;
        case bras_open_max:
            guide_prepare_motion("openbrasm", 1, POS_GAUCHE_OPEN_MAX, 1, POS_DROITE_OPEN_MAX);
            Put_Feetech(IDS_FEETECH_POUSSE[0], STS_GOAL_POSITION, POS_GAUCHE_OPEN_MAX, NULL);
            Put_Feetech(IDS_FEETECH_POUSSE[1], STS_GOAL_POSITION, POS_DROITE_OPEN_MAX, &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            break;
        case bras_init:
            guide_prepare_motion("initguide", 1, POS_GAUCHE_CLOSE, 1, POS_DROITE_CLOSE);
            Put_Feetech(IDS_FEETECH_POUSSE[0], STS_TORQUE_ENABLE, 1, NULL);
            Put_Feetech(IDS_FEETECH_POUSSE[1], STS_TORQUE_ENABLE, 1, NULL);
            Put_Feetech(IDS_FEETECH_POUSSE[0], STS_GOAL_POSITION, POS_GAUCHE_CLOSE, NULL);
            Put_Feetech(IDS_FEETECH_POUSSE[1], STS_GOAL_POSITION, POS_DROITE_CLOSE, &done_guide_tx);
            state_bras = GUIDE_STATE_WAIT_TX;
            break;
        case GUIDE_STATE_WAIT_TX:
            if (done_guide_tx) {
                uint8_t output_code = guide_output_code_from_sts(done_guide_tx);
                done_guide_tx = 0;
                if (output_code) {
                    guide_finish_action(output_code);
                } else if (!guide_track_left && !guide_track_right) {
                    guide_finish_action(0);
                } else {
                    state_bras = GUIDE_STATE_POLL_REQUEST;
                }
            }
            break;
        case GUIDE_STATE_POLL_REQUEST:
            if ((uint32_t)(Timer_ms1 - Timer_bras) > 20) {
                Timer_bras = Timer_ms1;
                done_guide_poll = 0;
                if (guide_track_left && guide_track_right) {
                    Get_Feetech(IDS_FEETECH_POUSSE[0], STS_PRESENT_POSITION, &guide_pos_left, NULL);
                    Get_Feetech(IDS_FEETECH_POUSSE[1], STS_PRESENT_POSITION, &guide_pos_right, &done_guide_poll);
                } else if (guide_track_left) {
                    Get_Feetech(IDS_FEETECH_POUSSE[0], STS_PRESENT_POSITION, &guide_pos_left, &done_guide_poll);
                } else if (guide_track_right) {
                    Get_Feetech(IDS_FEETECH_POUSSE[1], STS_PRESENT_POSITION, &guide_pos_right, &done_guide_poll);
                }
                state_bras = GUIDE_STATE_POLL_WAIT;
            }
            break;
        case GUIDE_STATE_POLL_WAIT:
            if (done_guide_poll) {
                uint8_t output_code = guide_output_code_from_sts(done_guide_poll);
                done_guide_poll = 0;
                if (output_code) {
                    guide_finish_action(output_code);
                    break;
                }

                if (((!guide_track_left) || (guide_abs_diff_u16(guide_pos_left, guide_target_left) <= GUIDE_POS_TOL)) &&
                    ((!guide_track_right) || (guide_abs_diff_u16(guide_pos_right, guide_target_right) <= GUIDE_POS_TOL))) {
                    guide_finish_action(0);
                } else if ((uint32_t)(Timer_ms1 - guide_move_start_ms) > GUIDE_MOVE_TIMEOUT_MS) {
                    guide_finish_action(GUIDE_OUTPUT_MOVE_TIMEOUT);
                } else {
                    state_bras = GUIDE_STATE_POLL_REQUEST;
                }
            }
            break;
        default:
            state_bras = 0;
            break;
    }
    return 0;
}



u8 open_bras_gauche(void){
    start_bras= bras_gauche_open;
    
    return 0;
    

}
u8 open_bras_gauche_max(void){
    start_bras= bras_gauche_open_max;
    return 0;

}

u8 close_bras_gauche(void){
    start_bras= bras_gauche_close;
    return 0;

}

u8 open_bras_droite(void){
    start_bras= bras_droite_open;
    return 0;

}
u8 open_bras_droite_max(void){
    start_bras= bras_droite_open_max;
    return 0;

}

u8 close_bras_droite(void){
    start_bras= bras_droite_close;
    return 0;

}

u8 free_guide(void){
    for (int i=0; i<2; ++i) Put_Feetech(IDS_FEETECH_POUSSE[i], STS_TORQUE_ENABLE, 0, NULL);
    start_bras= bras_free;
    return 0;

}
u8 open_bras_max(void){
    start_bras= bras_open_max;
    return 0;

}

u8 open_bras(void){
    start_bras= bras_open;
    return 0;

}

u8 close_bras(void){
    start_bras= bras_close;
    return 0;

}

u8 init_guide(void){
    start_bras = bras_init;
    return 0;
}