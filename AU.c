#include "main.h"

uint8_t State_AU = 0;
static uint8_t au_released = 0;
uint8_t AU_Soft = 0;

void init_AU(void)
{
    gpio_init(PIN_AU);
    gpio_set_dir(PIN_AU, GPIO_IN);
    gpio_init(EN_12V_PIN);
    gpio_set_dir(EN_12V_PIN, GPIO_OUT);
    gpio_put(EN_12V_PIN, 1);

    State_AU = 0;
}

u8 trigger_Soft_AU(void)
{
    AU_Soft = 1;
    return 0;
}

void AU_Loop(void) {
    uint8_t au_pressed = gpio_get(PIN_AU) ? PUSH_AU : PULL_AU;

    if (au_pressed == PUSH_AU || AU_Soft) {
        gpio_put(EN_12V_PIN, 0);
        free_barillet();
        free_guide();
        free_retourne();
        Set_RGB_State_AU();
        while (1) {
            if (au_pressed == PULL_AU) {
                au_pressed = gpio_get(PIN_AU) ? PUSH_AU : PULL_AU;
            }
            if ((Timer_ms1 - Last_Timer_Led) > 1000) {
                Last_Timer_Led = Timer_ms1;
                gpio_put(LED_PIN, LED_State);
                LED_State = !LED_State;
            }
            Timer_ms1 = time_ms_32();

		    int c = getchar_timeout_us(0);
		    if (c >= 0) {
			    Interp(c);
		    }
            Print_All_Cmd_Loop();
            AX_Uart_Half_Duplex_Loop();
		    AX_Loop();
		    STS_Uart_Half_Duplex_Loop();
		    STS_Loop();
            RGB_Loop();
            if (gpio_get(PIN_AU) == PULL_AU && au_pressed == PUSH_AU) {
                au_released = 1;
                break;
            }
        }
    } if (au_released) {
        watchdog_reboot(0, 0, 1);
    }
}
