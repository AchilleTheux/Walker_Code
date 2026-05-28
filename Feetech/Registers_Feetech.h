
#ifndef __REGISTERS_FEETECH_H_
#define __REGISTERS_FEETECH_H_


// ------STS2032------
// based on doc, keep only _L and len = 2
//-------EPROM--------
//#define  STS_MODEL_L 3
//#define  STS_MODEL_H 4
#define  STS_MODEL 3

//-------EPROM--------
#define  STS_ID 5
#define  STS_BAUD_RATE 6
#define  STS_DELAY_TIME_RETURN 7
#define  STS_LEVEL_RETURN 8
//#define  STS_MIN_ANGLE_LIMIT_L 9
//#define  STS_MIN_ANGLE_LIMIT_H 10
#define  STS_MIN_ANGLE_LIMIT 9
//#define  STS_MAX_ANGLE_LIMIT_L 11
//#define  STS_MAX_ANGLE_LIMIT_H 12
#define  STS_MAX_ANGLE_LIMIT 11
#define  STS_MAX_TEMP_LIMIT 13
#define  STS_MAX_INPUT_VOLT 14
#define  STS_MIN_INPUT_VOLT 15
//#define  STS_MAX_TORQUE_LIMIT_L 16
//#define  STS_MAX_TORQUE_LIMIT_H 17
#define  STS_MAX_TORQUE_LIMIT 16
#define  STS_SETTING_BYTE 18
#define  STS_PROTECTION_ENABLE 19
#define  STS_ALARM_LED 20
#define  STS_P_FACTOR 21
#define  STS_D_FACTOR 22
#define  STS_I_FACTOR 23
#define  STS_MIN_START_TORQUE 24
#define  STS_CW_DEAD 26
#define  STS_CCW_DEAD 27
//#define  STS_OVERLOAD_CURRENT_L 28
//#define  STS_OVERLOAD_CURRENT_H 29
#define  STS_OVERLOAD_CURRENT 28
#define  STS_RESOLUTION 30
//#define  STS_OFS_L 31
//#define  STS_OFS_H 32
#define  STS_OFS 31
#define  STS_MODE 33

//-------SRAM--------
#define  STS_TORQUE_ENABLE 40
#define  STS_ACC 41
//#define  STS_GOAL_POSITION_L 42
//#define  STS_GOAL_POSITION_H 43
#define  STS_GOAL_POSITION 42
//#define  STS_GOAL_TIME_L 44
//#define  STS_GOAL_TIME_H 45
#define  STS_GOAL_TIME 44
//#define  STS_GOAL_SPEED_L 46
//#define  STS_GOAL_SPEED_H 47
#define  STS_GOAL_SPEED 46
//#define  STS_TORQUE_LIMIT_L 48
//#define  STS_TORQUE_LIMIT_H 49
#define  STS_TORQUE_LIMIT 48
#define  STS_LOCK 55

//-------SRAM--------
//#define  STS_PRESENT_POSITION_L 56
//#define  STS_PRESENT_POSITION_H 57
#define  STS_PRESENT_POSITION 56
//#define  STS_PRESENT_SPEED_L 58
//#define  STS_PRESENT_SPEED_H 59
#define  STS_PRESENT_SPEED 58
//#define  STS_PRESENT_LOAD_L 60
//#define  STS_PRESENT_LOAD_H 61
#define  STS_PRESENT_LOAD 60
#define  STS_PRESENT_VOLTAGE 62
#define  STS_PRESENT_TEMPERATURE 63
#define  STS_MOVING 66
//#define  STS_PRESENT_CURRENT_L 69
//#define  STS_PRESENT_CURRENT_H 70
#define  STS_PRESENT_CURRENT 69


uint8_t RegisterLen_Feetech(uint8_t address);


// astuces
// feetech neuf : id = 1, vitesse = 1M
// pour changer le baudrate ou l'id, commencer par delock l'EEPROM : wr 0 in reg STS_LOCK(55)
// pour changer l'id, ecrire new_id in reg STS_ID(5)
// pour changer le baudrate ecrire 4 in reg STS_BAUD_RATE(6)
// (0 => 1000000)  (1 => 500000)  (2 => 250000)  (3 => 128000) (4 => 115200) (5 => 76800) (6 => 57600) (7 => 38400)
// warning, l'id et la vitesse changent tout de suite

// par defaut, le feetech a son couple desactiver : wr in STS_TORQUE_ENABLE(40)

#endif  //__REGISTERS_FEETECH_H_

