
#include "main.h"



uint32_t Flash_Setup_File_Add = FL_SETUP_ADD;

uint8_t Flash_Setup_File_State = 0;
// 0 = iddle
// 1,2 = mode lecture_interp, caractere pas pret
// 3 = mode lecture_interp, caractere pret
// 4 = mode lecture_interp, caractere pris en compte
// 10,11 = mode lecture_print
// 20,21 = y a un truc a ecrire

uint8_t Setup_File_Buff[100];
uint8_t Setup_File_Buff_i = 0;
uint8_t Setup_File_Buff_Rd_Wr_Done = 1;

uint32_t Last_Timer_Setup_File;


void Setup_File_Init(void)
{
	Flash_Setup_File_Add = FL_SETUP_ADD;
	Flash_Setup_File_State = 1;	  // start lecture/interp
}

void Setup_File_Loop(void)
{
	if (Flash_Setup_File_State) {
		if (Flash_Setup_File_State == 1) {
			Setup_File_Buff_Rd_Wr_Done = 0;
			Setup_File_Buff_i = 0;
			Flash_rd_Ext_Done(100, Flash_Setup_File_Add, &Setup_File_Buff[0], &Setup_File_Buff_Rd_Wr_Done);
			Flash_Setup_File_State ++;
		} else if (Flash_Setup_File_State == 2) {
			if (Setup_File_Buff_Rd_Wr_Done) {
				if (Setup_File_Buff[Setup_File_Buff_i] != 0xFF) {
					Flash_Setup_File_Add ++;
					Flash_Setup_File_State ++;
				} else {
					Flash_Setup_File_State = 0;	// on a trouve 0xFF, cest la fin de la partie ecrite du fichier
				}
			}
		//} else if (Flash_Setup_File_State == 3) {	// attente de la prise en compte
		} else if (Flash_Setup_File_State == 4) {	// ca a ete pris en compte, on retourne lire / proposer
			Setup_File_Buff_i ++;
			if (Setup_File_Buff_i < 100) {
				Flash_Setup_File_State = 2;
			} else {
				Flash_Setup_File_State = 1;
			}
		
		
		} else if (Flash_Setup_File_State == 10) {
			Setup_File_Buff_Rd_Wr_Done = 0;
			Setup_File_Buff_i = 0;
			Flash_rd_Ext_Done(100, Flash_Setup_File_Add, &Setup_File_Buff[0], &Setup_File_Buff_Rd_Wr_Done);
			Flash_Setup_File_State ++;
		} else if (Flash_Setup_File_State == 11) {
			if (Setup_File_Buff_Rd_Wr_Done) { // && Is_Std_Out_Empty()) {
				while ((Setup_File_Buff_i < 100) && (Setup_File_Buff[Setup_File_Buff_i] != 0xFF)) {
					printf("%c", Setup_File_Buff[Setup_File_Buff_i]);
					Flash_Setup_File_Add ++;
					Setup_File_Buff_i ++;
				}
				if (Setup_File_Buff_i == 100) {
					Flash_Setup_File_State ++;
					Last_Timer_Setup_File = Timer_ms1;
				} else {
					Flash_Setup_File_State = 0;
				}
			}
		} else if (Flash_Setup_File_State == 12) {
			if ((Timer_ms1 - Last_Timer_Setup_File) > 50) {
				Flash_Setup_File_State = 10;
			}
		} else if (Flash_Setup_File_State == 20) {
			if ((Flash_Setup_File_Add + Setup_File_Buff_i) < (FL_SETUP_ADD_MAX)) {		// s'il y a la place
				Setup_File_Buff_Rd_Wr_Done = 0;
				Flash_Setup_File_Add = Flash_wr_Ext_Done(Setup_File_Buff_i, Flash_Setup_File_Add, &Setup_File_Buff[0], &Setup_File_Buff_Rd_Wr_Done);
				Flash_Setup_File_State ++;
			} else {
				Flash_Setup_File_State = 0;
			}
		} else if (Flash_Setup_File_State == 21) {
			if (Setup_File_Buff_Rd_Wr_Done) {
				Flash_Setup_File_State = 0;
			}
		}
	}
}

uint8_t Setup_File_Get(uint8_t *c)
{
	if (Flash_Setup_File_State == 3) {
		*c = Setup_File_Buff[Setup_File_Buff_i];
		Flash_Setup_File_State ++;
		return 1;
	} else {
		return 0;
	}
}

uint8_t Setup_File_Erase_Func(void)
{
	if (Flash_Setup_File_State != 0) 	return IMPOSSIBLE_STATE_ERROR_CODE;
	Flash_sectorerase(FL_SETUP_FILE_BLOCK);
	Flash_Setup_File_Add = FL_SETUP_ADD;
	return 0;
}

uint8_t Setup_File_Start_Read_Func(void)
{
	if (Flash_Setup_File_State != 0) 	return IMPOSSIBLE_STATE_ERROR_CODE;
	Flash_Setup_File_Add = FL_SETUP_ADD;
	Flash_Setup_File_State = 10;
	return 0;
}

uint8_t Setup_File_Reload_Func(void)
{
	if (Flash_Setup_File_State != 0) 	return IMPOSSIBLE_STATE_ERROR_CODE;
	Setup_File_Init();
	return 0;
}

uint8_t Setup_File_Add_Func(void)
{
	if (Flash_Setup_File_State != 0) 	return IMPOSSIBLE_STATE_ERROR_CODE;
	uint8_t Len = Get_Param_End_String((char*)&Setup_File_Buff[0], 99);
	if (!Len) 							return PARAM_ERROR_CODE;
	Setup_File_Buff[Len] = '\n';
	Setup_File_Buff_i = Len + 1;
	Flash_Setup_File_State = 20;
	return 0;
}



