#ifndef __TEST_FUNCS_H_
#define __TEST_FUNCS_H_

#include "AX/AX_Uart_Half_Duplex.h"
#include "AX/AX.h"
#include "AX/AX_Actions.h"
#include "AX/Registers_AX12.h"

// initialize all things
void Test_Loop(void);
u8 Test_Func (void);
void All_Flash_Read_Loop(void);

u8 Ping_Func (void);
uint8_t Timer_Func(void);

extern uint8_t Debug_En;
uint8_t Debug_En_Func(void);

uint8_t Retourne_gauche_loop(void);
uint8_t Retourne_droite_loop(void);
uint8_t action_retourner_cagette_droite (void);
uint8_t action_retourner_cagette_gauche (void);

#endif // __TEST_FUNCS_H_
