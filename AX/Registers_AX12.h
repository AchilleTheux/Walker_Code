
#ifndef __REGISTERS_AX12_H_
#define __REGISTERS_AX12_H_


// -------AX12-------

// EEPROM Registers
#define AX_MODEL_NUMBER             0
#define AX_VERSION                  2
#define AX_ID                       3
#define AX_BAUD_RATE                4
#define AX_RETURN_DELAY_TIME        5
#define AX_CW_ANGLE_LIMIT           6
#define AX_CCW_ANGLE_LIMIT          8

#define AX_RESERVED_1               10
#define AX_LIMIT_TEMPERATURE        11
#define AX_DOWN_LIMIT_VOLTAGE       12
#define AX_UP_LIMIT_VOLTAGE         13
#define AX_MAX_TORQUE               14
#define AX_RETURN_LEVEL             16
#define AX_ALARM_LED                17
#define AX_ALARM_SHUTDOWN           18
#define AX_RESERVED_2               19
#define AX_DOWN_CALIBRATION         20
#define AX_UP_CALIBRATION           22

// RAM Registers
#define AX_TORQUE_ENABLE            24
#define AX_LED                      25
#define AX_CW_COMPLIANCE_MARGIN     26
#define AX_CCW_COMPLIANCE_MARGIN    27
#define AX_CW_COMPLIANCE_SLOPE      28
#define AX_CCW_COMPLIANCE_SLOPE     29
#define AX_GOAL_POSITION            30
#define AX_MOVING_SPEED             32
#define AX_TORQUE_LIMIT             34
#define AX_PRESENT_POSITION         36
#define AX_PRESENT_SPEED            38
#define AX_PRESENT_LOAD             40
#define AX_PRESENT_VOLTAGE          42
#define AX_PRESENT_TEMPERATURE      43
#define AX_REGISTERED_INSTRUCTION   44
#define AX_PAUSE_TIME               45
#define AX_MOVING                   46
#define AX_LOCK                     47
#define AX_PUNCH                    48





uint8_t RegisterLen_AX12(uint8_t address);


#endif  //__REGISTERS_AX12_H_

