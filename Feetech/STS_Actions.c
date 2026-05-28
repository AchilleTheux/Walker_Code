#include "main.h"

static const uint32_t feetech_baud_by_code[] = {
    1000000,
    500000,
    250000,
    128000,
    115200,
    76800,
    57600,
    38400,
};

static uint8_t feetech_wait_status(uint8_t status) {
    if (status == STS_STATUS_OK) {
        return 0;
    }

    printf("sts_status,%u\n", status);
    return IMPOSSIBLE_STATE_ERROR_CODE;
}

static uint8_t feetech_write_wait(uint8_t id, uint8_t reg, uint32_t data) {
    uint8_t done = STS_STATUS_PENDING;

    Put_Feetech(id, reg, data, &done);
    Wait_For_All_STS_Cmd();

    return feetech_wait_status(done);
}

static uint8_t get_u8_param(uint8_t *out) {
    u32 val32;

    if (Get_Param_u32(&val32)) {
        return PARAM_ERROR_CODE;
    }

    if (val32 > 255) {
        return PARAM_OUT_OF_RANGE_ERROR_CODE;
    }

    *out = val32;
    return 0;
}

uint8_t Feetech_Write_Func(void) {
    uint8_t id;
    uint8_t reg;
    u32 data;
    uint8_t err;

    if ((err = get_u8_param(&id))) {
        return err;
    }
    if ((err = get_u8_param(&reg))) {
        return err;
    }
    if (Get_Param_u32(&data)) {
        return PARAM_ERROR_CODE;
    }

    return feetech_write_wait(id, reg, data);
}

uint8_t Feetech_Read_Func(void) {
    uint8_t id;
    uint8_t reg;
    u32 data = 0;
    uint8_t done = STS_STATUS_PENDING;
    uint8_t err;

    if ((err = get_u8_param(&id))) {
        return err;
    }
    if ((err = get_u8_param(&reg))) {
        return err;
    }

    Get_Feetech(id, reg, &data, &done);
    Wait_For_All_STS_Cmd();

    err = feetech_wait_status(done);
    if (!err) {
        printf("read,%u,%u,%lu\n", (unsigned)id, (unsigned)reg, data);
    }

    return err;
}

uint8_t Feetech_Move_Func(void) {
    uint8_t id;
    u32 pos;
    uint8_t err;

    if ((err = get_u8_param(&id))) {
        return err;
    }
    if (Get_Param_u32(&pos)) {
        return PARAM_ERROR_CODE;
    }

    return feetech_write_wait(id, STS_GOAL_POSITION, pos);
}

uint8_t Feetech_Position_Func(void) {
    uint8_t id;
    u32 pos = 0;
    uint8_t done = STS_STATUS_PENDING;
    uint8_t err;

    if ((err = get_u8_param(&id))) {
        return err;
    }

    Get_Feetech(id, STS_PRESENT_POSITION, &pos, &done);
    Wait_For_All_STS_Cmd();

    err = feetech_wait_status(done);
    if (!err) {
        printf("pos,%u,%lu\n", (unsigned)id, pos);
    }

    return err;
}

uint8_t Feetech_Torque_Func(void) {
    uint8_t id;
    uint8_t enable;
    uint8_t err;

    if ((err = get_u8_param(&id))) {
        return err;
    }
    if ((err = get_u8_param(&enable))) {
        return err;
    }
    if (enable > 1) {
        return PARAM_OUT_OF_RANGE_ERROR_CODE;
    }

    return feetech_write_wait(id, STS_TORQUE_ENABLE, enable);
}

uint8_t Feetech_Set_Id_Func(void) {
    uint8_t old_id;
    uint8_t new_id;
    uint8_t err;

    if ((err = get_u8_param(&old_id))) {
        return err;
    }
    if ((err = get_u8_param(&new_id))) {
        return err;
    }
    if ((old_id > STS_BROADCAST) || (new_id >= STS_BROADCAST)) {
        return PARAM_OUT_OF_RANGE_ERROR_CODE;
    }

    if ((err = feetech_write_wait(old_id, STS_LOCK, 0))) {
        return err;
    }
    if ((err = feetech_write_wait(old_id, STS_ID, new_id))) {
        return err;
    }

    sleep_ms(20);
    err = feetech_write_wait(new_id, STS_LOCK, 1);
    if (!err) {
        printf("id,%u,%u\n", (unsigned)old_id, (unsigned)new_id);
    }

    return err;
}

uint8_t Feetech_Set_Servo_Baud_Func(void) {
    uint8_t id;
    uint8_t baud_code;
    uint8_t err;

    if ((err = get_u8_param(&id))) {
        return err;
    }
    if ((err = get_u8_param(&baud_code))) {
        return err;
    }
    if ((id > STS_BROADCAST) || (baud_code >= sizetab(feetech_baud_by_code))) {
        return PARAM_OUT_OF_RANGE_ERROR_CODE;
    }

    if ((err = feetech_write_wait(id, STS_LOCK, 0))) {
        return err;
    }
    if ((err = feetech_write_wait(id, STS_BAUD_RATE, baud_code))) {
        return err;
    }

    sleep_ms(20);
    STS_Uart_Half_Duplex_Set_Freq((float)feetech_baud_by_code[baud_code]);

    err = feetech_write_wait(id, STS_LOCK, 1);
    if (!err) {
        printf("servo_baud,%u,%u,%lu\n",
               (unsigned)id,
               (unsigned)baud_code,
               feetech_baud_by_code[baud_code]);
    }

    return err;
}

uint8_t Feetech_Set_Bus_Baud_Func(void) {
    u32 baud;

    if (Get_Param_u32(&baud)) {
        return PARAM_ERROR_CODE;
    }
    if (baud == 0) {
        return PARAM_OUT_OF_RANGE_ERROR_CODE;
    }

    STS_Uart_Half_Duplex_Set_Freq((float)baud);
    printf("bus_baud,%lu\n", baud);
    return 0;
}

static uint8_t scan_start = 0;
static uint8_t scan_state = 0;
static uint8_t scan_done = 0;
static uint16_t scan_return = 0;
static uint16_t scan_id = 0;

void STS_Search_ID_Loop(void) {
    switch (scan_state) {
    case 0:
        if (scan_start) {
            scan_start = 0;
            scan_id = 0;
            scan_return = 0;
            printf("scan,start\n");
            scan_state = 1;
        }
        break;

    case 1:
        if (scan_id < STS_BROADCAST) {
            scan_done = STS_STATUS_PENDING;
            Get_Feetech(scan_id, STS_ID, &scan_return, &scan_done);
            scan_state = 2;
        } else {
            printf("scan,done\n");
            scan_state = 0;
        }
        break;

    case 2:
        if (scan_done != STS_STATUS_PENDING) {
            if ((scan_done == STS_STATUS_OK) && (scan_return != 0)) {
                printf("id,%u\n", (unsigned)scan_return);
            }
            scan_return = 0;
            scan_id++;
            scan_state = 1;
        }
        break;

    default:
        scan_state = 0;
        break;
    }
}

uint8_t Feetech_Scan_Func(void) {
    scan_start = 1;
    return 0;
}
