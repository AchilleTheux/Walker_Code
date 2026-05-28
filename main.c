

#include "main.h"

uint32_t Timer_ms1 = 0;
uint32_t Last_Timer_Led = 0;
uint8_t LED_State = 1;


int main() {

	Init_All();
	gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

	while (1) {
		if ((Timer_ms1 - Last_Timer_Led) > 250) {
            Last_Timer_Led = Timer_ms1;
            gpio_put(LED_PIN, LED_State);
            LED_State = !LED_State;
        }
		Gestion_I2C_Master_Loop();
		Timer_ms1 = time_ms_32();

		int c = getchar_timeout_us(0);
		if (c >= 0) {
			Interp(c);
		}
		uint8_t c_Setup;
		if (Setup_File_Get(&c_Setup)) {
			Interp(c_Setup);
		}

		Print_All_Cmd_Loop();
		Flash_LogFile_Loop();
		Log_Print_Save_Loop();
		Setup_File_Loop();

		//Gestion_Expandeur_Loop();
		
		// AX
		
		AX_Uart_Half_Duplex_Loop();
		AX_Loop();
		Init_AX_Actions_Loop();
		Init_Actionneurs_Loop();

		AX_Actions_Loop();
		AX_Search_ID_Loop();

		// Feetech
		STS_Uart_Half_Duplex_Loop();
		STS_Loop();
		Init_STS_Actions_Loop();
		STS_Actions_Loop();
		STS_Search_ID_Loop();

		Action_Loop();
		
		curseur_Loop();

		Pompe_Loop();
		action_tourne_barillet_loop();
		Retourne_Loop();
		retourne_n_Loop();
		Test_Loop();
		Bras_Init_Loop(&bras1);
    	Bras_Init_Loop(&bras2);
		Bras_Loop(&bras1);
		Bras_Loop(&bras2);
		guide_Loop();
		RGB_Loop();
	    AU_Loop();
	}
}

