// bras_instances.c
// Ce fichier est le SEUL endroit où l'on configure les deux bras.
// Pour modifier les positions ou les IDs, tout se passe ici.

#include "main.h"

// -------------------------------------------------------------------
//  BRAS 1  (configuration d'origine)
// -------------------------------------------------------------------
static const Bras_Config cfg_bras1 = {
    .ax_id_arm  = 12,   // AX_ID_ARM
    .ax_id_head = 7,    // AX_ID_HEAD
    .pos = {
        .arm_inside       = 535,
        .arm_idle         = 430,
        .arm_mid          = 360,
        .arm_deployed     = 160,
        .head_idle        = 200,
        .head_deployed    = 820,
        .arm_deployed_drop = 820,
    },
    .precision_arm  = 50,
    .precision_head = 30,
};

Bras_t bras1 = {
    .cfg         = &cfg_bras1,
    .pin_bt1     = PIN_BT_1,
    .pin_bt2     = PIN_BT_2,
    .pin_en_ev1  = PIN_EN_EV_1,
    .pin_en_ev2  = PIN_EN_EV_2,
    .state_pompe_1 = &state_pompe_1,   // pointeurs vers les variables globales de pompe
    .state_pompe_2 = &state_pompe_2,
};

// -------------------------------------------------------------------
//  BRAS 2  (nouveau bras — IDs et positions à adapter)
// -------------------------------------------------------------------
static const Bras_Config cfg_bras2 = {
    .ax_id_arm  = 1,   // ← à remplacer par l'ID réel du bras 2
    .ax_id_head = 14,   // ← idem
    .pos = {
        .arm_inside       = 515,   // ← à calibrer pour le bras 2
        .arm_idle         = 434,
        .arm_mid          = 360,
        .arm_deployed     = 183,
        .head_idle        = 1022,
        .head_deployed    = 493,
        .arm_deployed_drop = 820,
    },
    .precision_arm  = 50,
    .precision_head = 30,
};

Bras_t bras2 = {
    .cfg         = &cfg_bras2,
    .pin_bt1     = PIN_BT_1_B,   // ← pins GPIO du bras 2
    .pin_bt2     = PIN_BT_2_B,
    .pin_en_ev1  = PIN_EN_EV_1_B,
    .pin_en_ev2  = PIN_EN_EV_2_B,
    .state_pompe_1 = &state_pompe_3,   // pompes dédiées au bras 2
    .state_pompe_2 = &state_pompe_4,
};

// Tableau d'instances de bras, indexé par numéro de bras (1 ou 2) pour commandes interpreteurs
Bras_t* bras_instances[3] = { NULL, &bras1, &bras2 };