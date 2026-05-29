#include "main.h"

#define WALKER_SERVO_LEFT_ID 11
#define WALKER_SERVO_RIGHT_ID 12

#define WALKER_SERVO_CENTER_POS 2048
#define WALKER_SERVO_STEPS_PER_RAD (4096.0f / (2.0f * 3.14159265358979323846f))

#define WALKER_BASE_HALF_MM 22.4f
#define WALKER_CRANK_MM 88.0f
#define WALKER_LINK_MM 108.0f
#define WALKER_FOOT_EXTENSION_MM 38.0f

#define WALKER_EXTENSION_THETA1 1
#define WALKER_EXTENSION_THETA2 2

typedef struct {
    float t1;
    float t2;
    uint16_t pos1;
    uint16_t pos2;
} WalkerIkSolution;

static uint8_t walker_extension_side = WALKER_EXTENSION_THETA2;

static float normalize_angle(float theta) {
    const float pi = 3.14159265358979323846f;
    const float two_pi = 2.0f * pi;

    while (theta > pi) {
        theta -= two_pi;
    }
    while (theta < -pi) {
        theta += two_pi;
    }

    return theta;
}

static uint8_t solve_angle(float a, float b, float c, uint8_t branch, float *theta) {
    float r = sqrtf((a * a) + (b * b));

    if (r <= 0.0001f) {
        return IMPOSSIBLE_STATE_ERROR_CODE;
    }

    float x = -c / r;
    if ((x < -1.0f) || (x > 1.0f)) {
        return IMPOSSIBLE_STATE_ERROR_CODE;
    }

    float phi = atan2f(b, a);
    float alpha = acosf(x);
    *theta = normalize_angle(phi + (branch ? -alpha : alpha));

    return 0;
}

static uint8_t theta_to_feetech_pos(float theta, uint16_t *pos) {
    int32_t raw = (int32_t)(WALKER_SERVO_CENTER_POS + (theta * WALKER_SERVO_STEPS_PER_RAD) + (theta >= 0.0f ? 0.5f : -0.5f));

    if ((raw < 0) || (raw > 4095)) {
        return PARAM_OUT_OF_RANGE_ERROR_CODE;
    }

    *pos = (uint16_t)raw;
    return 0;
}

static uint8_t walker_solve_ik_theta1_extension(float fx, float fy, uint8_t branch1, uint8_t branch2, WalkerIkSolution *sol) {
    uint8_t err;

    float extended_link = WALKER_LINK_MM + WALKER_FOOT_EXTENSION_MM;
    float a1 = -2.0f * WALKER_CRANK_MM * fx;
    float b1 = -2.0f * WALKER_CRANK_MM * (fy - WALKER_BASE_HALF_MM);
    float c1 = (fx * fx) +
               ((fy - WALKER_BASE_HALF_MM) * (fy - WALKER_BASE_HALF_MM)) +
               (WALKER_CRANK_MM * WALKER_CRANK_MM) -
               (extended_link * extended_link);

    err = solve_angle(a1, b1, c1, branch1, &sol->t1);
    if (err) {
        return err;
    }

    float px = ((WALKER_LINK_MM * fx) + (WALKER_FOOT_EXTENSION_MM * WALKER_CRANK_MM * cosf(sol->t1))) /
               extended_link;
    float py = ((WALKER_LINK_MM * fy) +
                (WALKER_FOOT_EXTENSION_MM * (WALKER_BASE_HALF_MM + (WALKER_CRANK_MM * sinf(sol->t1))))) /
               extended_link;
    float dy = py + WALKER_BASE_HALF_MM;

    float a2 = -2.0f * WALKER_CRANK_MM * px;
    float b2 = -2.0f * WALKER_CRANK_MM * dy;
    float c2 = (px * px) + (dy * dy) + (WALKER_CRANK_MM * WALKER_CRANK_MM) - (WALKER_LINK_MM * WALKER_LINK_MM);

    err = solve_angle(a2, b2, c2, branch2, &sol->t2);
    if (err) {
        return err;
    }

    err = theta_to_feetech_pos(sol->t1, &sol->pos1);
    if (err) {
        return err;
    }

    return theta_to_feetech_pos(sol->t2, &sol->pos2);
}

static uint8_t walker_solve_ik_theta2_extension(float fx, float fy, uint8_t branch1, uint8_t branch2, WalkerIkSolution *sol) {
    uint8_t err;
    float extended_link = WALKER_LINK_MM + WALKER_FOOT_EXTENSION_MM;

    float a2 = -2.0f * WALKER_CRANK_MM * fx;
    float b2 = -2.0f * WALKER_CRANK_MM * (fy + WALKER_BASE_HALF_MM);
    float c2 = (fx * fx) +
               ((fy + WALKER_BASE_HALF_MM) * (fy + WALKER_BASE_HALF_MM)) +
               (WALKER_CRANK_MM * WALKER_CRANK_MM) -
               (extended_link * extended_link);

    err = solve_angle(a2, b2, c2, branch2, &sol->t2);
    if (err) {
        return err;
    }

    float px = ((WALKER_LINK_MM * fx) + (WALKER_FOOT_EXTENSION_MM * WALKER_CRANK_MM * cosf(sol->t2))) /
               extended_link;
    float py = ((WALKER_LINK_MM * fy) +
                (WALKER_FOOT_EXTENSION_MM * (-WALKER_BASE_HALF_MM + (WALKER_CRANK_MM * sinf(sol->t2))))) /
               extended_link;
    float cy = py - WALKER_BASE_HALF_MM;

    float a1 = -2.0f * WALKER_CRANK_MM * px;
    float b1 = -2.0f * WALKER_CRANK_MM * cy;
    float c1 = (px * px) + (cy * cy) + (WALKER_CRANK_MM * WALKER_CRANK_MM) - (WALKER_LINK_MM * WALKER_LINK_MM);

    err = solve_angle(a1, b1, c1, branch1, &sol->t1);
    if (err) {
        return err;
    }

    err = theta_to_feetech_pos(sol->t1, &sol->pos1);
    if (err) {
        return err;
    }

    return theta_to_feetech_pos(sol->t2, &sol->pos2);
}

static uint8_t walker_solve_ik(float fx, float fy, uint8_t branch1, uint8_t branch2, WalkerIkSolution *sol) {
    if (walker_extension_side == WALKER_EXTENSION_THETA1) {
        return walker_solve_ik_theta1_extension(fx, fy, branch1, branch2, sol);
    }

    return walker_solve_ik_theta2_extension(fx, fy, branch1, branch2, sol);
}

static void print_ik_solution(const WalkerIkSolution *sol) {
    int32_t t1_mrad = (int32_t)((sol->t1 * 1000.0f) + (sol->t1 >= 0.0f ? 0.5f : -0.5f));
    int32_t t2_mrad = (int32_t)((sol->t2 * 1000.0f) + (sol->t2 >= 0.0f ? 0.5f : -0.5f));

    printf("ik,%u,%ld,%ld,%u,%u\n",
           (unsigned)walker_extension_side,
           t1_mrad,
           t2_mrad,
           (unsigned)sol->pos1,
           (unsigned)sol->pos2);
}

static uint8_t read_ik_params(float *fx, float *fy, uint8_t *branch1, uint8_t *branch2) {
    u32 branch;

    if (Get_Param_Float(fx)) {
        return PARAM_ERROR_CODE;
    }
    if (Get_Param_Float(fy)) {
        return PARAM_ERROR_CODE;
    }

    *branch1 = 0;
    *branch2 = 0;

    if (!Get_Param_u32(&branch)) {
        *branch1 = branch ? 1 : 0;
    }
    if (!Get_Param_u32(&branch)) {
        *branch2 = branch ? 1 : 0;
    }

    return 0;
}

uint8_t Walker_Ik_Func(void) {
    float fx;
    float fy;
    uint8_t branch1;
    uint8_t branch2;
    WalkerIkSolution sol;
    uint8_t err = read_ik_params(&fx, &fy, &branch1, &branch2);

    if (err) {
        return err;
    }

    err = walker_solve_ik(fx, fy, branch1, branch2, &sol);
    if (!err) {
        print_ik_solution(&sol);
    }

    return err;
}

uint8_t Walker_Foot_Func(void) {
    float fx;
    float fy;
    uint8_t branch1;
    uint8_t branch2;
    WalkerIkSolution sol;
    uint8_t done1 = STS_STATUS_PENDING;
    uint8_t done2 = STS_STATUS_PENDING;
    uint8_t err = read_ik_params(&fx, &fy, &branch1, &branch2);

    if (err) {
        return err;
    }

    err = walker_solve_ik(fx, fy, branch1, branch2, &sol);
    if (err) {
        return err;
    }

    Put_Feetech(WALKER_SERVO_LEFT_ID, STS_GOAL_POSITION, sol.pos1, &done1);
    Put_Feetech(WALKER_SERVO_RIGHT_ID, STS_GOAL_POSITION, sol.pos2, &done2);
    Wait_For_All_STS_Cmd();

    if ((done1 != STS_STATUS_OK) || (done2 != STS_STATUS_OK)) {
        printf("sts_status,%u,%u\n", (unsigned)done1, (unsigned)done2);
        return IMPOSSIBLE_STATE_ERROR_CODE;
    }

    print_ik_solution(&sol);
    return 0;
}

uint8_t Walker_Extension_Side_Func(void) {
    u32 side;

    if (Get_Param_u32(&side)) {
        printf("extside,%u\n", (unsigned)walker_extension_side);
        return 0;
    }

    if ((side != WALKER_EXTENSION_THETA1) && (side != WALKER_EXTENSION_THETA2)) {
        return PARAM_OUT_OF_RANGE_ERROR_CODE;
    }

    walker_extension_side = side;
    printf("extside,%u\n", (unsigned)walker_extension_side);
    return 0;
}
