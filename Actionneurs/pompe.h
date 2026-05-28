#ifndef __POMPE_H_
#define __POMPE_H_

#define PIN_EN_POMPE_1 6
#define PIN_EN_POMPE_2 8
#define PIN_EN_EV_1 7
#define PIN_EN_EV_2 9

#define PIN_BT_1 26
#define PIN_BT_2 22

#define PIN_BT_1_B 26
#define PIN_BT_2_B 22

#define PIN_EN_EV_1_B 7
#define PIN_EN_EV_2_B 9

#define POMPE_ON 1
#define POMPE_OFF 2

void Pompe_Init (void);
void Pompe_Loop (void);

void Vanne_Init(void);
void EV_CLOSE(void);
void EV_OPEN(void);

void Bouton_Init(void);

uint8_t All_Pompe_ON(void);
uint8_t EN_Vanne_Func(void);

extern uint8_t state_pompe_1;
extern uint8_t state_pompe_2;
extern uint8_t state_pompe_3; //Pompes dédiées au bras 2
extern uint8_t state_pompe_4;
#endif // __POMPE_H_