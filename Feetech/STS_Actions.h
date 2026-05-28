#ifndef STS_ACTIONS_H
#define STS_ACTIONS_H

uint8_t Feetech_Write_Func(void);
uint8_t Feetech_Read_Func(void);
uint8_t Feetech_Move_Func(void);
uint8_t Feetech_Position_Func(void);
uint8_t Feetech_Torque_Func(void);
uint8_t Feetech_Set_Id_Func(void);
uint8_t Feetech_Set_Servo_Baud_Func(void);
uint8_t Feetech_Set_Bus_Baud_Func(void);
uint8_t Feetech_Scan_Func(void);

void STS_Search_ID_Loop(void);

#endif
