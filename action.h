#ifndef __ACTION_H_
#define __ACTION_H_

#define TIME_END_MATCH 100500


extern uint8_t State_Timer_match; // Possibilité de faire les actions



void Action_init(void);
void Action_Loop(void);

uint8_t restart_match_Func (void);
uint8_t start_timer_match_Func (void);
void timer_match_Loop(void);


uint8_t retourne_n_Loop(void) ;
uint8_t retourne_n(void);

void Init_Actionneurs_Loop(void);
uint8_t init_actionneurs_Func();



#endif // __ACTION_H_