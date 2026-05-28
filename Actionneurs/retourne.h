#ifndef __RETOURNE_H__
#define __RETOURNE_H__

#define gauche_bas 31
#define droite_bas 30
#define droite_haut 35
#define gauche_haut 34
#define milieu_bas 36

#define POS_RB_COMPRES 2277
#define POS_RB_DECOMPRES 1565
#define POS_RB_GAP 1739

#define POS_LB_COMPRES 174
#define POS_LB_DECOMPRES 839
#define POS_LB_GAP 605

#define POS_MILIEU_EXT 3600 //3400
#define POS_MILIEU_EXT_INIT 3617
#define POS_MILIEU_INT 2900

#define POS_UL_RETOURNE0 53//1964 //UL = Up Left
#define POS_UL_RETOURNE1 2101 //4000

#define POS_UR_RETOURNE0 3979//UR = Up Right
#define POS_UR_RETOURNE1 1931

extern u8 IDS_FEETECH_RETOURNE[5] ;
extern u8 DONE_FEETECH_RETOURNE[5];
extern uint8_t state_retourne_droite ;
extern uint8_t state_retourne_gauche ;
void Retourne_Init(void);
void Retourne_Loop(void);

uint8_t action_retourner_cagette_droite (void);
uint8_t action_retourner_cagette_gauche (void);

uint8_t free_retourne(void);

#endif // __BARILLET_H__
