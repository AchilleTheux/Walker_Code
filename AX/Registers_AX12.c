
#include "main.h"




uint8_t RegisterLen_AX12(uint8_t address) {
    switch (address) {
        case AX_VERSION: case AX_ID: case AX_BAUD_RATE: case AX_RETURN_DELAY_TIME: case AX_LIMIT_TEMPERATURE: case AX_DOWN_LIMIT_VOLTAGE: case AX_UP_LIMIT_VOLTAGE: case AX_RETURN_LEVEL:
        case AX_ALARM_LED: case AX_ALARM_SHUTDOWN: case AX_RESERVED_2: case AX_TORQUE_ENABLE: case AX_LED: case AX_CW_COMPLIANCE_MARGIN: case AX_CCW_COMPLIANCE_MARGIN: case AX_CW_COMPLIANCE_SLOPE:
        case AX_CCW_COMPLIANCE_SLOPE: case AX_PRESENT_VOLTAGE: case AX_PRESENT_TEMPERATURE: case AX_REGISTERED_INSTRUCTION: case AX_MOVING: case AX_LOCK:
            return 1;
            break;
        case AX_MODEL_NUMBER: case AX_CW_ANGLE_LIMIT: case AX_CCW_ANGLE_LIMIT: case AX_MAX_TORQUE: case AX_DOWN_CALIBRATION: case AX_UP_CALIBRATION: case AX_GOAL_POSITION: case AX_MOVING_SPEED:
        case AX_TORQUE_LIMIT: case AX_PRESENT_POSITION: case AX_PRESENT_SPEED: case AX_PRESENT_LOAD: case AX_PUNCH:
            return 2;
            break;
        default:
            return 0;
            break;
    }
}

