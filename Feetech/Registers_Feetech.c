#include "main.h"



uint8_t RegisterLen_Feetech(uint8_t address) {
    switch (address) {
    case STS_ID: case STS_BAUD_RATE: case STS_DELAY_TIME_RETURN: case STS_LEVEL_RETURN: case STS_MAX_TEMP_LIMIT: case STS_MAX_INPUT_VOLT:
    case STS_MIN_INPUT_VOLT: case STS_SETTING_BYTE: case STS_PROTECTION_ENABLE: case STS_ALARM_LED: case STS_CW_DEAD: case STS_CCW_DEAD:
    case STS_RESOLUTION: case STS_MODE: case STS_TORQUE_ENABLE: case STS_LOCK: case STS_PRESENT_VOLTAGE:
    case STS_ACC: case STS_PRESENT_TEMPERATURE: case STS_MOVING:
        return 1;
        break;
    case STS_MODEL: case STS_MIN_ANGLE_LIMIT:  case STS_MAX_ANGLE_LIMIT: case STS_MAX_TORQUE_LIMIT:  
    case STS_OFS: case STS_MIN_START_TORQUE: case STS_OVERLOAD_CURRENT: case STS_GOAL_POSITION: case STS_GOAL_TIME: case STS_GOAL_SPEED: 
    case STS_TORQUE_LIMIT: case STS_PRESENT_POSITION: case STS_PRESENT_SPEED: case STS_PRESENT_LOAD: case STS_PRESENT_CURRENT:
        return 2;
        break;
    default:
        return 0; // Unknown register address
        break;
    }
}





