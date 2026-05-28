#include "main.h"
#include "bras.h"

#define BRAS_BUTTON_FILTER_MAX 3u
#define BRAS_ARM_SPEED_DEFAULT 120u
#define BRAS_ARM_SPEED_STORE_RETRACT 40u
#define BRAS_STORE_RETRACT_TIMEOUT_MS 1500u

static bool Bras_Filter_Button(bool raw_pressed, uint8_t *counter, bool filtered_state) {
    if (raw_pressed) {
        if (*counter < BRAS_BUTTON_FILTER_MAX) {
            (*counter)++;
        }
    } else if (*counter > 0u) {
        (*counter)--;
    }

    if (*counter == BRAS_BUTTON_FILTER_MAX) {
        return true;
    }
    if (*counter == 0u) {
        return false;
    }
    return filtered_state;
}

// -------------------------------------------------------------------
//  INIT
// -------------------------------------------------------------------
void Bras_Init(Bras_t *b) {
    b->done_ax12              = 1;
    // Initialisation de la machine d'état d'init
    b->state_init             = 0;
    b->start_init             = 0;
    b->is_init_done           = 0;
    b->timer_init             = 0;

    b->drop_from_barillet     = 1;
    b->store_in_barillet    = 1;
    b->state_grab_box         = 0;
    b->state_store_box        = 0;
    b->state_drop_box         = 0;
    b->state_grab_and_store_box = 0;
    b->state_fold_arm         = 0;
    b->state_EV_1             = 0;
    b->state_EV_2             = 0;
    b->grab_done              = 0;
    b->store_done             = 0;
    b->drop_done              = 0;
    b->grab_and_store_done    = 0;
    b->ax_arm_order           = 0;
    b->ax_head_order          = 0;
    b->BT1                    = false;
    b->BT2                    = false;
    b->bt1_filter_cnt         = 0u;
    b->bt2_filter_cnt         = 0u;
}
void Bras_Init_Loop(Bras_t *b){
    const Bras_Config *cfg = b->cfg;
    // ----------------------------------------------------------------
    //  INITIALISATION DES MOTEURS
    // ----------------------------------------------------------------
    switch (b->state_init) {
        case 0:
            if (b->start_init) {
                b->start_init = 0;
                b->state_init++;
                printf("Start INIT Bras (Arm:%d, Head:%d)\n", cfg->ax_id_arm, cfg->ax_id_head);
            }
            break;

        case 1:
            // Activer le couple
            Put_AX12(cfg->ax_id_arm, AX_TORQUE_ENABLE, 1, NULL);
            Put_AX12(cfg->ax_id_head, AX_TORQUE_ENABLE, 1, NULL);

            // Initialisation de la vitesse
            Put_AX12(cfg->ax_id_arm, AX_MOVING_SPEED, BRAS_ARM_SPEED_DEFAULT, NULL);

            b->timer_init = Timer_ms1;
            b->state_init++;
            break;

        case 2:
            if ((Timer_ms1 - b->timer_init) > 100) {
                Put_AX12(cfg->ax_id_head, AX_GOAL_POSITION, cfg->pos.head_idle, NULL);
                Put_AX12(cfg->ax_id_arm, AX_GOAL_POSITION, cfg->pos.arm_idle, NULL);
                b->state_init++;
            }
            break;

        case 3:
            printf("***** Initialisation Bras OK *******\n");
            b->state_init = 0;
            b->is_init_done = 1;
            break;
    }
}
// -------------------------------------------------------------------
//  LOOP
// -------------------------------------------------------------------
void Bras_Loop(Bras_t *b) {
    const Bras_Config *cfg = b->cfg;

    // --- Lecture des positions moteurs ---
    if (b->done_ax12) {
        b->done_ax12 = 0;
        Get_AX12(cfg->ax_id_arm,  AX_PRESENT_POSITION, &b->pos_arm,  NULL);
        Get_AX12(cfg->ax_id_head, AX_PRESENT_POSITION, &b->pos_head, &b->done_ax12);
    }

    // --- Lecture des boutons ---
    b->BT1 = Bras_Filter_Button(!gpio_get(b->pin_bt1), &b->bt1_filter_cnt, b->BT1);
    b->BT2 = Bras_Filter_Button(!gpio_get(b->pin_bt2), &b->bt2_filter_cnt, b->BT2);

    // Arrêt d'urgence
    if (State_AU == PUSH_AU) {
        b->state_grab_box         = 0;
        b->state_store_box        = 0;
        b->state_drop_box         = 0;
        b->state_grab_and_store_box = 0;
    }

    // ----------------------------------------------------------------
    //  ACTION : ATTRAPER CAGETTE
    // ----------------------------------------------------------------
    switch (b->state_grab_box) {
        case 0:
            break;

        case 1:
            b->grab_done      = 0;
            b->ax_arm_order   = DEPLOY_ARM;
            b->timer_grab_box = Timer_ms1;
            b->state_grab_box++;
            break;

        case 2:
            if (b->pos_arm > 400) {
                if (b->grab_side == LEFT || b->grab_side == ALL) {
                    *b->state_pompe_1 = POMPE_ON;
                    b->state_EV_1 = 0;
                }
                if (b->grab_side == RIGHT || b->grab_side == ALL) {
                    *b->state_pompe_2 = POMPE_ON;
                    b->state_EV_2 = 0;
                }
                b->timer_grab_box = Timer_ms1;
                b->ax_head_order  = DEPLOY_HEAD;
                b->state_grab_box++;
            }
            break;

        case 3:
            if (b->BT2 || b->BT1) {
                b->state_grab_box = 0;
                b->grab_done      = 1;
                printf("bras grab 0\n");
            } else if (Timer_ms1 - b->timer_grab_box > 2000) {
                printf("bras grab 1\n");
                b->state_grab_box = 0;
            }
            break;
    }

    // ----------------------------------------------------------------
    //  ACTION : RANGER CAGETTE
    // ----------------------------------------------------------------
    switch (b->state_store_box) {
        case 0:
            break;

        case 1:
            b->timer_store_box = Timer_ms1;
            b->ax_arm_order    = ARM_MID;
            b->store_done      = 0;
            b->state_store_box++;
            break;

        case 2: {
            int16_t gap = (int16_t)b->pos_arm - (int16_t)cfg->pos.arm_mid;
            if (abs(gap) < cfg->precision_arm) {
                if (b->store_in_barillet == 0){ //si on ne stocke pas dans le barillet, on stoppe l'action dès que le bras est levé.
                b->state_store_box = 0;
                printf("bras store 0\n");
                }
                else{ //sinon on poursuit l'action
                b->ax_head_order   = RETRACT_HEAD;
                b->timer_store_box = Timer_ms1;
                b->state_store_box++;
                }
            }
            break;
        }

        case 3: {
            int16_t gap = (int16_t)b->pos_head - (int16_t)cfg->pos.head_idle;
            if (abs(gap) < cfg->precision_head) {
                b->ax_arm_order    = PRESS_ARM_INSIDE;
                b->timer_store_box = Timer_ms1;
                b->state_store_box++;
            }
            break;
        }

        case 4: {
            int16_t gap = (int16_t)b->pos_arm - (int16_t)cfg->pos.arm_inside;
            if (abs(gap) < cfg->precision_arm) {
                if (b->store_side == LEFT || b->store_side == ALL) {
                    *b->state_pompe_1 = POMPE_OFF;
                    b->state_EV_1 = 1;
                }
                if (b->store_side == RIGHT || b->store_side == ALL) {
                    *b->state_pompe_2 = POMPE_OFF;
                    b->state_EV_2 = 1;
                }
                b->timer_store_box = Timer_ms1;
                b->state_store_box = 6;

            } else if ((Timer_ms1 - b->timer_store_box) > 1500) { //echec : le bras n'est pas rentré dans le barillet
                b->ax_arm_order = ARM_MID;
                b->timer_store_box = Timer_ms1;
                //b->drop_from_barillet = 0; //en cas d'echec, le dépôt suivant ne se fera pas depuis le barillet (commenté car à effectuer sur ROS)
                b->state_store_box = 5;
            }
            break;
        }
        case 5: //cas actif qu'après un échec de dépôt dans le barillet 
            if (Timer_ms1 - b->timer_store_box > 500) {
                b->ax_head_order   = DEPLOY_HEAD; //on redéploie la tête après un échec
                b->timer_store_box = Timer_ms1;
                b->state_store_box=0;
                printf("bras store 1\n"); //message d'erreur pour ROS
            }
            break;

        case 6:
            if (Timer_ms1 - b->timer_store_box > 1000) {
                Put_AX12(cfg->ax_id_arm, AX_MOVING_SPEED, BRAS_ARM_SPEED_STORE_RETRACT, NULL);
                b->ax_arm_order    = RETRACT_ARM;
                b->timer_store_box = Timer_ms1;
                b->state_store_box++;
            }
            break;

        case 7: {
            int16_t gap = (int16_t)b->pos_arm - (int16_t)cfg->pos.arm_idle;
            if (abs(gap) < cfg->precision_arm ||
                (Timer_ms1 - b->timer_store_box) > BRAS_STORE_RETRACT_TIMEOUT_MS) {
                Put_AX12(cfg->ax_id_arm, AX_MOVING_SPEED, BRAS_ARM_SPEED_DEFAULT, NULL);
                b->state_store_box = 0;
                b->store_done      = 1; 
                b->ax_arm_order    = 0;
                printf("bras store 0\n");
            }
            break;
        }
    }

    // ----------------------------------------------------------------
    //  ACTION : DEPOSER CAGETTE
    // ----------------------------------------------------------------
    switch (b->state_drop_box) {
        case 0:
            break;

        case 1:
            b->drop_done      = 0;
            if (b->drop_side == LEFT || b->store_side == ALL) {
                    *b->state_pompe_1 = POMPE_ON;
                }
                if (b->store_side == RIGHT || b->store_side == ALL) {
                    *b->state_pompe_2 = POMPE_ON;
                }

            if (b->drop_from_barillet) {
                b->ax_arm_order   = PRESS_ARM_INSIDE;
                b->timer_drop_box = Timer_ms1;
                b->state_drop_box++;
            } else {
                b->timer_drop_box = Timer_ms1;
                b->state_drop_box = 3;
            }
            break;

        case 2: {
            int16_t gap = (int16_t)b->pos_arm - (int16_t)cfg->pos.arm_inside;
            if (abs(gap) < cfg->precision_arm && (b->BT1 || b->BT2)) {
                b->ax_arm_order   = ARM_MID;
                b->timer_drop_box = Timer_ms1;
                b->state_drop_box++;
            }
            break;
        }

        case 3: {
            int16_t gap = (int16_t)b->pos_arm - (int16_t)cfg->pos.arm_mid;
            if (abs(gap) < cfg->precision_arm || !b->drop_from_barillet) {
                b->drop_from_barillet = 1;
                b->timer_drop_box     = Timer_ms1;
                b->ax_head_order      = DEPLOY_HEAD;
                b->state_drop_box++;
            }
            break;
        }

        case 4: {
            int16_t gap = (int16_t)b->pos_head - (int16_t)cfg->pos.head_deployed;
            if (abs(gap) < cfg->precision_head) {
                b->ax_arm_order   = DEPLOY_ARM;
                b->timer_drop_box = Timer_ms1;
                b->state_drop_box++;
            }
            break;
        }

        case 5:
            if (abs((int16_t)b->pos_arm - (int16_t)cfg->pos.arm_deployed) < cfg->precision_arm) {
                if (b->store_side == LEFT || b->store_side == ALL) {
                    *b->state_pompe_1 = POMPE_OFF;
                    b->state_EV_1 = 1;
                }
                if (b->store_side == RIGHT || b->store_side == ALL) {
                    *b->state_pompe_2 = POMPE_OFF;
                    b->state_EV_2 = 1;
                }
                b->timer_drop_box = Timer_ms1;
                b->state_drop_box = 7;

            } else if (Timer_ms1 - b->timer_drop_box > 2000) { //erreur : blocage sur le barillet
                b->timer_drop_box = Timer_ms1;
                b->ax_arm_order   = ARM_MID;
                b->state_drop_box = 6;
            }
            break;


        case 6:
            if (Timer_ms1 - b->timer_drop_box > 200) {
                b->ax_arm_order   = RETRACT_ARM;
                b->timer_drop_box = Timer_ms1;
                b->state_drop_box++;
            }
            break;

        case 7:
            if (abs((int16_t)b->pos_arm - (int16_t)cfg->pos.arm_mid) < cfg->precision_arm) {
                b->ax_head_order  = RETRACT_HEAD;
                b->timer_drop_box = Timer_ms1;
                b->state_drop_box++;
            }
            break;

        case 8:
            b->drop_done      = 1;
            b->state_drop_box = 0;
            printf("bras drop 0\n");
            break;
    }

    // ----------------------------------------------------------------
    //  ACTION : ATTRAPER ET RANGER (séquence combinée)
    // ----------------------------------------------------------------
    switch (b->state_grab_and_store_box) {
        case 0:
            break;

        case 1:
            b->grab_and_store_done = 0;
            b->grab_done           = 0;
            b->state_grab_box      = 1;
            b->state_grab_and_store_box++;
            break;

        case 2:
            if (b->grab_done) {
                b->store_side   = ALL;
                b->state_store_box = 1;
                b->state_grab_and_store_box++;
            }
            break;

        case 3:
            if (b->store_done) {
                b->state_grab_and_store_box = 0;
                b->grab_and_store_done      = 1;
            }
            break;
    }

    // ----------------------------------------------------------------
    //  ACTION : RENTRER LE BRAS (pour vision caméra)
    // ----------------------------------------------------------------
    switch (b->state_fold_arm) {
        case 0:
            break;

        case 1:
            b->ax_arm_order    = PRESS_ARM_INSIDE;
            b->timer_fold_arm  = Timer_ms1;
            b->state_fold_arm++;
            break;

        case 2: {
            int16_t gap = (int16_t)b->pos_arm - (int16_t)cfg->pos.arm_inside;
            if (abs(gap) < cfg->precision_arm) {
                printf("bras fold 0\n");
                b->state_fold_arm = 0;
            } else if ((Timer_ms1 - b->timer_fold_arm) > 1500) {
                printf("bras fold 1\n");
                b->state_fold_arm = 0;
            }
            break;
        }
    }

    // ----------------------------------------------------------------
    //  GESTION EV 1
    // ----------------------------------------------------------------
    if (b->state_EV_1 == 1) {
        if ((Timer_ms1 - b->timer_EV_1) < 2000) {
            gpio_put(b->pin_en_ev1, 1);
        } else {
            b->state_EV_1 = 0;
        }
    } else {
        gpio_put(b->pin_en_ev1, 0);
        b->state_EV_1  = 0;
        b->timer_EV_1  = Timer_ms1;
    }

    // ----------------------------------------------------------------
    //  GESTION EV 2
    // ----------------------------------------------------------------
    if (b->state_EV_2 == 1) {
        if ((Timer_ms1 - b->timer_EV_2) < 2000) {
            gpio_put(b->pin_en_ev2, 1);
        } else {
            b->state_EV_2 = 0;
        }
    } else {
        gpio_put(b->pin_en_ev2, 0);
        b->state_EV_2  = 0;
        b->timer_EV_2  = Timer_ms1;
    }

    // ----------------------------------------------------------------
    //  ORDRES MOTEURS (traités en fin de loop pour éviter les doublons)
    // ----------------------------------------------------------------
    switch (b->ax_arm_order) {
        case DEPLOY_ARM:
            Put_AX12(cfg->ax_id_arm, AX_GOAL_POSITION, cfg->pos.arm_deployed, NULL);
            b->ax_arm_order = 0; break;
        case RETRACT_ARM:
            Put_AX12(cfg->ax_id_arm, AX_GOAL_POSITION, cfg->pos.arm_idle, NULL);
            b->ax_arm_order = 0; break;
        case ARM_MID:
            Put_AX12(cfg->ax_id_arm, AX_GOAL_POSITION, cfg->pos.arm_mid, NULL);
            b->ax_arm_order = 0; break;
        case PRESS_ARM_INSIDE:
            Put_AX12(cfg->ax_id_arm, AX_GOAL_POSITION, cfg->pos.arm_inside, NULL);
            b->ax_arm_order = 0; break;
        case ARM_DEPLOYED_DROP:
            Put_AX12(cfg->ax_id_arm, AX_GOAL_POSITION, cfg->pos.arm_deployed_drop, NULL);
            b->ax_arm_order = 0; break;
        default: break;
    }
    switch (b->ax_head_order) {
        case DEPLOY_HEAD:
            Put_AX12(cfg->ax_id_head, AX_GOAL_POSITION, cfg->pos.head_deployed, NULL);
            b->ax_head_order = 0; break;
        case RETRACT_HEAD:
            Put_AX12(cfg->ax_id_head, AX_GOAL_POSITION, cfg->pos.head_idle, NULL);
            b->ax_head_order = 0; break;
        default: break;
    }
}

// -------------------------------------------------------------------
//  API ACTIONS
// -------------------------------------------------------------------
uint8_t Bras_Demarrer_Init(Bras_t *b) {
    b->start_init = 1;
    return 0;
}

uint8_t Bras_Free(Bras_t *b) {
    Put_AX12(b->cfg->ax_id_arm,  AX_TORQUE_ENABLE, 0, NULL);
    Put_AX12(b->cfg->ax_id_head, AX_TORQUE_ENABLE, 0, NULL);
    return 0;
}

uint8_t Bras_Attraper_Cagette(Bras_t *b, uint32_t side) {
    b->grab_side  = side;
    b->state_grab_box = 1;
    return 0;
}

uint8_t Bras_Ranger_Cagette(Bras_t *b, uint32_t side, uint32_t in_barillet) {
    b->store_side  = side;
    b->store_in_barillet = in_barillet;
    b->state_store_box = 1;
    return 0;
}

uint8_t Bras_Deposer_Cagette(Bras_t *b, uint32_t side, uint32_t from_barillet) {
    b->drop_side = side;
    b->drop_from_barillet = from_barillet;
    b->state_drop_box = 1;
    return 0;
}

uint8_t Bras_Attraper_Et_Ranger(Bras_t *b, uint32_t side, uint32_t in_barillet) {
    b->grab_side = side;
    b->store_in_barillet = in_barillet;
    b->state_grab_and_store_box = 1;
    return 0;
}

uint8_t Bras_Rentrer_Bras(Bras_t *b) {
    b->state_fold_arm = 1;
    return 0;
}

// -------------------------------------------------------------------
//  WRAPPERS INTERPRÉTEUR (lisent le numéro de bras puis les paramètres)
// -------------------------------------------------------------------

uint8_t Bras_Demarrer_Init_All(void) {
    Bras_Demarrer_Init(&bras1);
    Bras_Demarrer_Init(&bras2);
    return 0;
}

uint8_t Bras_Free_All(void) {
    Bras_Free(&bras1);
    Bras_Free(&bras2);
    return 0;
}

uint8_t action_attraper_cagette(void) {
    uint32_t bras_num;
    if (Get_Param_u32(&bras_num)) return PARAM_ERROR_CODE;
    if (bras_num < 1 || bras_num > 2) return PARAM_ERROR_CODE;
    uint32_t side;
    if (Get_Param_u32(&side)) return PARAM_ERROR_CODE;
    return Bras_Attraper_Cagette(bras_instances[bras_num], side);
}

uint8_t action_ranger_cagette(void) {
    uint32_t bras_num;
    if (Get_Param_u32(&bras_num)) return PARAM_ERROR_CODE;
    if (bras_num < 1 || bras_num > 2) return PARAM_ERROR_CODE;
    uint32_t side;
    if (Get_Param_u32(&side)) return PARAM_ERROR_CODE;
    uint32_t in_barillet;
    if (Get_Param_u32(&in_barillet)) return PARAM_ERROR_CODE;
    return Bras_Ranger_Cagette(bras_instances[bras_num], side, in_barillet);
}

uint8_t action_deposer_cagette(void) {
    uint32_t bras_num;
    if (Get_Param_u32(&bras_num)) return PARAM_ERROR_CODE;
    if (bras_num < 1 || bras_num > 2) return PARAM_ERROR_CODE;
    uint32_t side;
    if (Get_Param_u32(&bras_num)) return PARAM_ERROR_CODE;
    uint32_t from_barillet;
    if (Get_Param_u32(&from_barillet)) return PARAM_ERROR_CODE;
    return Bras_Deposer_Cagette(bras_instances[bras_num], side, from_barillet);
}

uint8_t action_attraper_et_ranger_cagette(void) {
    uint32_t bras_num;
    if (Get_Param_u32(&bras_num)) return PARAM_ERROR_CODE;
    if (bras_num < 1 || bras_num > 2) return PARAM_ERROR_CODE;
    uint32_t side;
    if (Get_Param_u32(&side)) return PARAM_ERROR_CODE;
    uint32_t in_barillet;
    if (Get_Param_u32(&in_barillet)) return PARAM_ERROR_CODE;
    return Bras_Attraper_Et_Ranger(bras_instances[bras_num], side, in_barillet);
}

uint8_t action_rentrer_bras(void) {
    uint32_t bras_num;
    if (Get_Param_u32(&bras_num)) return PARAM_ERROR_CODE;
    if (bras_num < 1 || bras_num > 2) return PARAM_ERROR_CODE;
    return Bras_Rentrer_Bras(bras_instances[bras_num]);
}
