/*
 * File:   actions_ax12.h
 * Author: 7robot
 *
 * Created on 21 Mai 2017, 18:41
 */

 #ifndef __AX_ACTIONS_H
 #define __AX_ACTIONS_H

// interpreteur function speak to register
u8 Send_AX_Func(void);
u8 Get_AX_Func(void);

void AX_Search_ID_Loop(void);
uint8_t Test_ID_AX_Func(void);
uint8_t Move_AX_Func(void);
uint8_t Get_Pos_AX_Func(void);
//INIT
uint8_t init_AX_Func (void);
 

// ACTIONS LOOP
extern uint8_t start_init_AX;
void Init_AX_Actions_Loop(void);
void AX_Actions_Loop(void);
uint8_t speed_AX_Func(void);


#endif	/* __AX_ACTIONS_H */
