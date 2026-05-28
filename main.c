

#include "main.h"

uint32_t Timer_ms1 = 0;
uint32_t Last_Timer_Led = 0;
uint8_t LED_State = 1;


int main() {

	Init_All();
	gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

	while (1) {
		Timer_ms1 = time_ms_32();

		if ((Timer_ms1 - Last_Timer_Led) > 250) {
            Last_Timer_Led = Timer_ms1;
            gpio_put(LED_PIN, LED_State);
            LED_State = !LED_State;
        }

		int c = getchar_timeout_us(0);
		if (c >= 0) {
			Interp(c);
		}

		Print_All_Cmd_Loop();

		// Feetech
		STS_Uart_Half_Duplex_Loop();
		STS_Loop();
		Init_STS_Actions_Loop();
		STS_Actions_Loop();
		STS_Search_ID_Loop();
	}
}
