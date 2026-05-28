#include "main.h"
// RETOURNE_LOOP
uint32_t Timer_action_retourne = 0;
uint32_t Timer_couple = 0;
uint32_t Timer_lecture = 0;

uint8_t state_retourne_droite = 0;
uint8_t state_retourne_gauche = 0;
uint8_t state_theta_droite = 0;
uint8_t state_theta_gauche = 0;

uint8_t start_retourne_droite = 0;
uint8_t start_retourne_gauche = 0;

u8 IDS_FEETECH_RETOURNE[5] = {30, 31, 34, 35, 36};
u8 DONE_FEETECH_RETOURNE[5] = {1, 1, 1, 1, 1};

uint8_t state_error_gauche = 0;
uint8_t state_error_droite = 0;

uint32_t couple = 0;

/*
 * Codes d'erreur retourne:
 * 0  : OK
 * 1  : surcharge cote droite (charge)
 * 2  : surcharge cote gauche (charge)
 * 3  : surcharge milieu_bas (impacte droite et gauche)
 * 10 : erreur mouvement droite - milieu ext
 * 11 : erreur mouvement droite - rotation haut
 * 12 : erreur mouvement droite - milieu int
 * 13 : erreur mouvement droite - decomp bas
 * 20 : erreur mouvement gauche - milieu ext
 * 21 : erreur mouvement gauche - rotation haut
 * 22 : erreur mouvement gauche - milieu int
 * 23 : erreur mouvement gauche - decomp bas
 */
#define RET_ERR_OK 0
#define RET_ERR_LOAD_DROITE 1
#define RET_ERR_LOAD_GAUCHE 2
#define RET_ERR_LOAD_MILIEU 3
#define RET_ERR_D_EXT 10
#define RET_ERR_D_ROT 11
#define RET_ERR_D_INT 12
#define RET_ERR_D_DECOMP 13
#define RET_ERR_G_EXT 20
#define RET_ERR_G_ROT 21
#define RET_ERR_G_INT 22
#define RET_ERR_G_DECOMP 23
#define SET_ERR_IF_OK(err_var, code) do { if ((err_var) == RET_ERR_OK) (err_var) = (code); } while (0)

#define RETOURNE_POS_TOL 50
#define RETOURNE_POLL_PERIOD_MS 20
#define RETOURNE_MOVE_TIMEOUT_MS 2500

// Wait optionnel apres verification de position (ms)
#define RETOURNE_WAIT_AFTER_COMP_MS 750
#define RETOURNE_WAIT_AFTER_EXT_MS 0
#define RETOURNE_WAIT_AFTER_ROT_MS 0
#define RETOURNE_WAIT_AFTER_INT_MS 200
#define RETOURNE_WAIT_AFTER_DECOMP_MS 0

#define RETOURNE_MOVE_WAITING 0
#define RETOURNE_MOVE_DONE 1
#define RETOURNE_MOVE_ERROR 2

typedef struct {
    uint8_t done_move;
    uint8_t done_poll;
    uint8_t phase; // 0: wait command ack, 1: poll position, 2: extra wait
    uint8_t servo_id;
    uint16_t target_pos;
    uint16_t present_pos;
    uint32_t move_start_ms;
    uint32_t last_poll_ms;
    uint32_t wait_start_ms;
    uint32_t extra_wait_ms;
} RetourneMoveTracker;

static RetourneMoveTracker move_tracker_droite = {0};
static RetourneMoveTracker move_tracker_gauche = {0};
static uint32_t comp_wait_start_droite = 0;
static uint32_t comp_wait_start_gauche = 0;

static uint16_t retourne_abs_diff_u16(uint16_t a, uint16_t b) {
    return (a > b) ? (a - b) : (b - a);
}

static uint8_t retourne_status_to_error(uint8_t sts_status) {
    if (sts_status == STS_STATUS_OK) return 0;
    return sts_status;
}

static void retourne_reset_move_tracker(RetourneMoveTracker *tracker) {
    tracker->done_move = 0;
    tracker->done_poll = 0;
    tracker->phase = 0;
    tracker->servo_id = 0;
    tracker->target_pos = 0;
    tracker->present_pos = 0;
    tracker->move_start_ms = 0;
    tracker->last_poll_ms = 0;
    tracker->wait_start_ms = 0;
    tracker->extra_wait_ms = 0;
}

static void retourne_start_move(RetourneMoveTracker *tracker, uint8_t servo_id, uint16_t target_pos, uint32_t extra_wait_ms) {
    tracker->done_move = 0;
    tracker->done_poll = 0;
    tracker->phase = 0;
    tracker->servo_id = servo_id;
    tracker->target_pos = target_pos;
    tracker->extra_wait_ms = extra_wait_ms;
    tracker->move_start_ms = Timer_ms1;
    tracker->last_poll_ms = Timer_ms1;
    tracker->wait_start_ms = 0;
    Put_Feetech(servo_id, STS_GOAL_POSITION, target_pos, &tracker->done_move);
}

static uint8_t retourne_wait_move(RetourneMoveTracker *tracker) {
    if (tracker->phase == 0) {
        if (!tracker->done_move) return RETOURNE_MOVE_WAITING;
        if (retourne_status_to_error(tracker->done_move)) return RETOURNE_MOVE_ERROR;
        tracker->done_move = 0;
        tracker->phase = 1;
        tracker->last_poll_ms = Timer_ms1;
    }

    if (tracker->phase == 1) {
        if ((uint32_t)(Timer_ms1 - tracker->move_start_ms) > RETOURNE_MOVE_TIMEOUT_MS) return RETOURNE_MOVE_ERROR;

        if (!tracker->done_poll && ((uint32_t)(Timer_ms1 - tracker->last_poll_ms) > RETOURNE_POLL_PERIOD_MS)) {
            tracker->last_poll_ms = Timer_ms1;
            Get_Feetech(tracker->servo_id, STS_PRESENT_POSITION, &tracker->present_pos, &tracker->done_poll);
        }

        if (!tracker->done_poll) return RETOURNE_MOVE_WAITING;
        if (retourne_status_to_error(tracker->done_poll)) return RETOURNE_MOVE_ERROR;
        tracker->done_poll = 0;

        if (retourne_abs_diff_u16(tracker->present_pos, tracker->target_pos) <= RETOURNE_POS_TOL) {
            if (!tracker->extra_wait_ms) return RETOURNE_MOVE_DONE;
            tracker->phase = 2;
            tracker->wait_start_ms = Timer_ms1;
        }
        return RETOURNE_MOVE_WAITING;
    }

    if (tracker->phase == 2) {
        if ((uint32_t)(Timer_ms1 - tracker->wait_start_ms) > tracker->extra_wait_ms) return RETOURNE_MOVE_DONE;
        return RETOURNE_MOVE_WAITING;
    }

    return RETOURNE_MOVE_WAITING;
}

void Retourne_Init(void) {
    Put_Feetech(droite_haut, STS_GOAL_POSITION, POS_UR_RETOURNE0, NULL);
    Put_Feetech(gauche_haut, STS_GOAL_POSITION, POS_UL_RETOURNE0, NULL);
}

void Retourne_Loop(void) {
    if (State_AU == 1) {
        start_retourne_droite = 0;
        start_retourne_gauche = 0;
        state_retourne_droite = 0;
        state_retourne_gauche = 0;
        retourne_reset_move_tracker(&move_tracker_droite);
        retourne_reset_move_tracker(&move_tracker_gauche);
        for (int i = 0; i < 5; ++i) {
            if (DONE_FEETECH_RETOURNE[i]) {
                DONE_FEETECH_RETOURNE[i] = 0;
                Put_Feetech(IDS_FEETECH_RETOURNE[i], STS_TORQUE_ENABLE, 0, &DONE_FEETECH_RETOURNE[i]);
            }
        }
    }

    //if (Timer_ms1 - Timer_couple > 1000) {
    //    Timer_couple = Timer_ms1;
    //
    //    for (int i = 0; i < 5; i++) {
    //        Get_Feetech(IDS_FEETECH_RETOURNE[i], STS_PRESENT_LOAD, &couple, NULL);
    //        if (Timer_ms1 - Timer_lecture > 100) {
    //            Timer_lecture = Timer_ms1;
    //            if (couple > 1600) {
    //                if (i == 4) {
    //                    SET_ERR_IF_OK(state_error_droite, RET_ERR_LOAD_MILIEU);
    //                    SET_ERR_IF_OK(state_error_gauche, RET_ERR_LOAD_MILIEU);
    //                } else if (i % 2 == 0) {
    //                    SET_ERR_IF_OK(state_error_droite, RET_ERR_LOAD_DROITE);
    //                } else {
    //                    SET_ERR_IF_OK(state_error_gauche, RET_ERR_LOAD_GAUCHE);
    //                }
    //            }
    //        }
    //        couple = 0;
    //    }
    //}

    switch (state_retourne_droite) {
        case 0:
            if (start_retourne_droite == 1) {
                start_retourne_droite = 0;
                retourne_reset_move_tracker(&move_tracker_droite);
                state_error_droite = RET_ERR_OK;
                state_retourne_droite = 1;
            }
            break;
        case 1: // compression cagette (pas d'attente)
            Put_Feetech(droite_bas, STS_GOAL_POSITION, POS_RB_COMPRES, NULL);
            comp_wait_start_droite = Timer_ms1;
            state_retourne_droite = 2;
            break;
        case 2: // wait optionnel apres compression (sans polling), puis milieu ext
            if (RETOURNE_WAIT_AFTER_COMP_MS &&
                ((uint32_t)(Timer_ms1 - comp_wait_start_droite) <= RETOURNE_WAIT_AFTER_COMP_MS)) {
                break;
            }
            retourne_start_move(&move_tracker_droite, milieu_bas, POS_MILIEU_EXT, RETOURNE_WAIT_AFTER_EXT_MS);
            state_retourne_droite = 3;
            break;
        case 3: {
            uint8_t move_state = retourne_wait_move(&move_tracker_droite);
            if (move_state == RETOURNE_MOVE_DONE) state_retourne_droite = 4;
            else if (move_state == RETOURNE_MOVE_ERROR) {
                SET_ERR_IF_OK(state_error_droite, RET_ERR_D_EXT);
                state_retourne_droite = 10;
            }
            break;
        }
        case 4: // rotation haut + verification + wait optionnel
            if (state_theta_droite) {
                retourne_start_move(&move_tracker_droite, droite_haut, POS_UR_RETOURNE0, RETOURNE_WAIT_AFTER_ROT_MS);
                state_theta_droite = 0;
            } else {
                retourne_start_move(&move_tracker_droite, droite_haut, POS_UR_RETOURNE1, RETOURNE_WAIT_AFTER_ROT_MS);
                state_theta_droite = 1;
            }
            state_retourne_droite = 5;
            break;
        case 5: {
            uint8_t move_state = retourne_wait_move(&move_tracker_droite);
            if (move_state == RETOURNE_MOVE_DONE) state_retourne_droite = 6;
            else if (move_state == RETOURNE_MOVE_ERROR) {
                SET_ERR_IF_OK(state_error_droite, RET_ERR_D_ROT);
                state_retourne_droite = 10;
            }
            break;
        }
        case 6: // milieu int + verification + wait optionnel
            retourne_start_move(&move_tracker_droite, milieu_bas, POS_MILIEU_INT, RETOURNE_WAIT_AFTER_INT_MS);
            state_retourne_droite = 7;
            break;
        case 7: {
            uint8_t move_state = retourne_wait_move(&move_tracker_droite);
            if (move_state == RETOURNE_MOVE_DONE) state_retourne_droite = 8;
            else if (move_state == RETOURNE_MOVE_ERROR) {
                SET_ERR_IF_OK(state_error_droite, RET_ERR_D_INT);
                state_retourne_droite = 10;
            }
            break;
        }
        case 8: // decomp + verification + wait optionnel
            retourne_start_move(&move_tracker_droite, droite_bas, POS_RB_DECOMPRES, RETOURNE_WAIT_AFTER_DECOMP_MS);
            state_retourne_droite = 9;
            break;
        case 9: {
            uint8_t move_state = retourne_wait_move(&move_tracker_droite);
            if (move_state == RETOURNE_MOVE_DONE) state_retourne_droite = 10;
            else if (move_state == RETOURNE_MOVE_ERROR) {
                SET_ERR_IF_OK(state_error_droite, RET_ERR_D_DECOMP);
                state_retourne_droite = 10;
            }
            break;
        }
        case 10:
            state_retourne_droite = 0;
            printf("retourne retourned %i\n", state_error_droite);
            break;
    }

    switch (state_retourne_gauche) {
        case 0:
            if (start_retourne_gauche == 1) {
                start_retourne_gauche = 0;
                retourne_reset_move_tracker(&move_tracker_gauche);
                state_error_gauche = RET_ERR_OK;
                state_retourne_gauche = 1;
            }
            break;
        case 1: // compression cagette (pas d'attente)
            Put_Feetech(gauche_bas, STS_GOAL_POSITION, POS_LB_COMPRES, NULL);
            comp_wait_start_gauche = Timer_ms1;
            state_retourne_gauche = 2;
            break;
        case 2: // wait optionnel apres compression (sans polling), puis milieu ext
            if (RETOURNE_WAIT_AFTER_COMP_MS &&
                ((uint32_t)(Timer_ms1 - comp_wait_start_gauche) <= RETOURNE_WAIT_AFTER_COMP_MS)) {
                break;
            }
            retourne_start_move(&move_tracker_gauche, milieu_bas, POS_MILIEU_EXT, RETOURNE_WAIT_AFTER_EXT_MS);
            state_retourne_gauche = 3;
            break;
        case 3: {
            uint8_t move_state = retourne_wait_move(&move_tracker_gauche);
            if (move_state == RETOURNE_MOVE_DONE) state_retourne_gauche = 4;
            else if (move_state == RETOURNE_MOVE_ERROR) {
                SET_ERR_IF_OK(state_error_gauche, RET_ERR_G_EXT);
                state_retourne_gauche = 10;
            }
            break;
        }
        case 4: // rotation haut + verification + wait optionnel
            if (state_theta_gauche) {
                retourne_start_move(&move_tracker_gauche, gauche_haut, POS_UL_RETOURNE0, RETOURNE_WAIT_AFTER_ROT_MS);
                state_theta_gauche = 0;
            } else {
                retourne_start_move(&move_tracker_gauche, gauche_haut, POS_UL_RETOURNE1, RETOURNE_WAIT_AFTER_ROT_MS);
                state_theta_gauche = 1;
            }
            state_retourne_gauche = 5;
            break;
        case 5: {
            uint8_t move_state = retourne_wait_move(&move_tracker_gauche);
            if (move_state == RETOURNE_MOVE_DONE) state_retourne_gauche = 6;
            else if (move_state == RETOURNE_MOVE_ERROR) {
                SET_ERR_IF_OK(state_error_gauche, RET_ERR_G_ROT);
                state_retourne_gauche = 10;
            }
            break;
        }
        case 6: // milieu int + verification + wait optionnel
            retourne_start_move(&move_tracker_gauche, milieu_bas, POS_MILIEU_INT, RETOURNE_WAIT_AFTER_INT_MS);
            state_retourne_gauche = 7;
            break;
        case 7: {
            uint8_t move_state = retourne_wait_move(&move_tracker_gauche);
            if (move_state == RETOURNE_MOVE_DONE) state_retourne_gauche = 8;
            else if (move_state == RETOURNE_MOVE_ERROR) {
                SET_ERR_IF_OK(state_error_gauche, RET_ERR_G_INT);
                state_retourne_gauche = 10;
            }
            break;
        }
        case 8: // decomp + verification + wait optionnel
            retourne_start_move(&move_tracker_gauche, gauche_bas, POS_LB_DECOMPRES, RETOURNE_WAIT_AFTER_DECOMP_MS);
            state_retourne_gauche = 9;
            break;
        case 9: {
            uint8_t move_state = retourne_wait_move(&move_tracker_gauche);
            if (move_state == RETOURNE_MOVE_DONE) state_retourne_gauche = 10;
            else if (move_state == RETOURNE_MOVE_ERROR) {
                SET_ERR_IF_OK(state_error_gauche, RET_ERR_G_DECOMP);
                state_retourne_gauche = 10;
            }
            break;
        }
        case 10:
            state_retourne_gauche = 0;
            printf("retourne retourneg %i\n", state_error_gauche);
            break;
    }
}

uint8_t action_retourner_cagette_droite(void) {
    start_retourne_droite = 1;
    return 0;
}

uint8_t action_retourner_cagette_gauche(void) {
    start_retourne_gauche = 1;
    return 0;
}

uint8_t free_retourne(void) {
    Put_Feetech(gauche_bas, STS_TORQUE_ENABLE, 0, NULL);
    Put_Feetech(gauche_haut, STS_TORQUE_ENABLE, 0, NULL);
    Put_Feetech(droite_bas, STS_TORQUE_ENABLE, 0, NULL);
    Put_Feetech(droite_haut, STS_TORQUE_ENABLE, 0, NULL);
    Put_Feetech(milieu_bas, STS_TORQUE_ENABLE, 0, NULL);
    printf("Done\n");
    return 0;
}
