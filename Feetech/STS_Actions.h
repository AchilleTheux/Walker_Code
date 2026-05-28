/*
 * File:   actions_ax12.h
 * Author: 7robot
 *
 * Created on 21 Mai 2017, 18:41
 */

 #ifndef __STS_ACTIONS_H
 #define __STS_ACTIONS_H

// interpreteur function speak to register
u8 Send_STS_Func(void);
u8 Get_STS_Func(void);

void STS_Search_ID_Loop(void);
uint8_t Test_ID_STS_Func(void);
uint8_t Move_STS_Func(void);
uint8_t Get_Pos_STS_Func(void);
//INIT
uint8_t init_STS_Func (void);
 

// ACTIONS LOOP
extern uint8_t start_init_STS;
void Init_STS_Actions_Loop(void);
void STS_Actions_Loop(void);
uint8_t speed_STS_Func(void);



#define STS_INIT_DONE 20


#define ID_STS_BOTTOM_LEFT 11 //11
#define ID_STS_BOTTOM_RIGHT 13 //13
#define ID_STS_TOP_LEFT 12 //12
#define ID_STS_TOP_RIGHT 14 //14


#define RAD_PER_STEP  (2*3.1415/4096) // 0.0015 rad/step
#define STEP_PER_RAD (4096/(2*3.1415)) // 682.67 step/rad



#endif	/* __STS_ACTIONS_H */
