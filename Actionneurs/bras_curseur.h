#ifndef __bras_curseur_H__
#define __bras_curseur_H__



extern uint8_t state_curseur_descente ;
extern uint8_t start_curseur_descente;
extern uint8_t state_curseur_montee ;
extern uint8_t start_curseur_montees;

#define ID_BRAS_CURSEUR 50

uint8_t curseur_Init(void);
uint8_t curseur_free(void);
void curseur_Loop(void);

uint8_t action_descendre_bras_curseur (void);
uint8_t action_montee_bras_curseur (void);

#endif // __BARILLET_H__