#ifndef __BRAS_H_
#define __BRAS_H_

#define ALL   0
#define LEFT  1
#define RIGHT 2

// -------------------------------------------------------------------
// Configuration statique d'un bras (IDs moteurs + positions cibles)
// -------------------------------------------------------------------
typedef struct { //structure de config pour les positions cibles 
    uint16_t arm_idle;
    uint16_t head_idle;
    uint16_t arm_mid;
    uint16_t arm_deployed;
    uint16_t head_deployed;
    uint16_t arm_inside;
    uint16_t arm_deployed_drop;
} AX_POS;

typedef struct {
    uint8_t  ax_id_arm;
    uint8_t  ax_id_head;
    AX_POS   pos;           // positions cibles propres à ce bras
    uint8_t  precision_arm;
    uint8_t  precision_head;
} Bras_Config;



// -------------------------------------------------------------------
// État dynamique d'un bras (une instance = un bras physique)
// -------------------------------------------------------------------
typedef struct {
    const Bras_Config *cfg;  // pointeur vers la config (lecture seule)

    // Ordres moteurs en attente
    uint8_t  ax_arm_order;
    uint8_t  ax_head_order;

    // Position courante lue sur les moteurs
    uint16_t pos_arm;
    uint16_t pos_head;
    uint8_t  done_ax12;

    // Machines à états des actions
    uint8_t  state_init;     // État de l'init
    uint8_t  state_grab_box;
    uint8_t  state_store_box;
    uint8_t  state_drop_box;
    uint8_t  state_grab_and_store_box;
    uint8_t  state_fold_arm;

    // EV
    uint8_t  state_EV_1;
    uint8_t  state_EV_2;

    //Drapeaux début d'action 
    uint8_t  start_init;  // Flag de démarrage init
    
    // Drapeaux de fin d'action
    uint8_t  is_init_done;  // Flag init terminée
    uint8_t  grab_done;
    uint8_t  store_done;
    uint8_t  drop_done;
    uint8_t  grab_and_store_done;

    // Paramètres d'action
    uint32_t grab_side;
    uint32_t store_side;
    uint32_t drop_side;
    uint8_t  drop_from_barillet;
    uint8_t store_in_barillet;

    // Timers internes
    uint32_t timer_init;       
    uint32_t timer_grab_box;
    uint32_t timer_store_box;
    uint32_t timer_drop_box;
    uint32_t timer_EV_1;
    uint32_t timer_EV_2;
    uint32_t timer_fold_arm;

    // Entrées boutons (lus depuis GPIO)
    bool BT1;
    bool BT2;
    uint8_t bt1_filter_cnt;
    uint8_t bt2_filter_cnt;

    // Pins GPIO associées à ce bras
    uint8_t  pin_bt1;
    uint8_t  pin_bt2;
    uint8_t  pin_en_ev1;
    uint8_t  pin_en_ev2;

    // Pompes associées à ce bras
    uint8_t  *state_pompe_1;
    uint8_t  *state_pompe_2;
} Bras_t;

// -------------------------------------------------------------------
// Instances des bras
// -------------------------------------------------------------------
extern Bras_t bras1;
extern Bras_t bras2;

// -------------------------------------------------------------------
// API générique : toutes les fonctions prennent un pointeur Bras_t*
// -------------------------------------------------------------------
void Bras_Init(Bras_t *b);
void Bras_Init_Loop(Bras_t *b);
void Bras_Loop(Bras_t *b);

//Déclaration des états d'ordre pour les moteurs
#define DEPLOY_ARM 2
#define DEPLOY_HEAD 3
#define RETRACT_ARM 4
#define RETRACT_HEAD 5
#define ARM_MID 6
#define PRESS_ARM_INSIDE 7
#define ARM_DEPLOYED_DROP 8


// Actions (équivalent des action_xxx mais sur l'instance choisie)
uint8_t Bras_Demarrer_Init         (Bras_t *b);
uint8_t Bras_Attraper_Cagette      (Bras_t *b, uint32_t grab_side);
uint8_t Bras_Ranger_Cagette        (Bras_t *b, uint32_t store_side, uint32_t in_barillet);
uint8_t Bras_Deposer_Cagette       (Bras_t *b, uint32_t drop_side, uint32_t from_barillet);
uint8_t Bras_Attraper_Et_Ranger    (Bras_t *b, uint32_t grab_side, uint32_t in_barillet);
uint8_t Bras_Rentrer_Bras          (Bras_t *b);
uint8_t Bras_Free                   (Bras_t *b);

//Fonctions interpreteurs
uint8_t action_attraper_cagette(void);
uint8_t action_ranger_cagette(void);
uint8_t action_deposer_cagette(void);
uint8_t action_attraper_et_ranger_cagette(void);
uint8_t action_rentrer_bras(void);
uint8_t Bras_Free_All(void);
uint8_t Bras_Demarrer_Init_All(void);



#endif /* __BRAS_H_ */
