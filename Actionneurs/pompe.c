#include "main.h"


uint8_t state_pompe_1 = POMPE_OFF;
uint8_t state_pompe_2 = POMPE_OFF;
uint8_t state_pompe_3 = POMPE_OFF;
uint8_t state_pompe_4 = POMPE_OFF;

uint32_t timer_pompe = 0;

void Pompe_Init (void){
    gpio_init(PIN_EN_POMPE_1);
    gpio_set_dir(PIN_EN_POMPE_1, GPIO_OUT);
    gpio_put(PIN_EN_POMPE_1, 0);
    
    gpio_init(PIN_EN_POMPE_2);
    gpio_set_dir(PIN_EN_POMPE_2, GPIO_OUT);
    gpio_put(PIN_EN_POMPE_2, 0);
    
}
void Vanne_Init(void){
    gpio_init(PIN_EN_EV_1);
    gpio_set_dir(PIN_EN_EV_1, GPIO_OUT);
    gpio_init(PIN_EN_EV_2);
    gpio_set_dir(PIN_EN_EV_2, GPIO_OUT);
}

void Bouton_Init(void){
    gpio_init(PIN_BT_1);
    gpio_set_dir(PIN_BT_1, GPIO_IN);

    gpio_init(PIN_BT_2);
    gpio_set_dir(PIN_BT_2, GPIO_IN);
}

void EV_CLOSE(void){
    gpio_put(PIN_EN_EV_1, 0);
    gpio_put(PIN_EN_EV_2, 0);
}
void EV_OPEN(void){
    gpio_put(PIN_EN_EV_1, 1);
    gpio_put(PIN_EN_EV_2, 1);
}

void Pompe_Loop(void) {
    if ((Timer_ms1 - timer_pompe) >= 50) 
    timer_pompe = Timer_ms1;

    if (State_AU == PUSH_AU) {
        gpio_put(PIN_EN_POMPE_1, 0);
        gpio_put(PIN_EN_POMPE_2, 0);
    }
    gpio_put(PIN_EN_POMPE_1, state_pompe_1 == POMPE_ON ? 1 : 0); //Si l'état de la pompe 1 est ON, on met le pin de la pompe à 1, sinon à 0  
    gpio_put(PIN_EN_POMPE_2, state_pompe_2 == POMPE_ON ? 1 : 0);
}

uint8_t All_Pompe_ON(void)
{
    uint32_t state;
    if (Get_Param_u32(&state)) return PARAM_ERROR_CODE;
    if (state) {
        state_pompe_1 = POMPE_ON;
        state_pompe_2 = POMPE_ON;
        printf("Pompe ON\n");
    } else {
        state_pompe_1 = POMPE_OFF;
        state_pompe_2 = POMPE_OFF;
        printf("Pompe OFF\n");
    }
    return 0;
}

uint8_t EN_Vanne_Func(void)
{
    uint32_t state;
    if (Get_Param_u32(&state)) return PARAM_ERROR_CODE;
    if (state) {
        EV_OPEN();
        printf("Vanne OUVERTE\n");

    } else {
        EV_CLOSE();
        printf("Vanne FERMEE\n");        
    }
    return 0;
}