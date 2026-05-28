
#ifndef __INTERPRETEUR_H
#define __INTERPRETEUR_H

#define MAX_CMD_LENGTH 100

//pour ne pas avoir de message d'erreur
//#define WORLD_OF_SILENCE

#define PARAM_ERROR_CODE 1
#define PARAM_OUT_OF_RANGE_ERROR_CODE 2
#define IMPOSSIBLE_STATE_ERROR_CODE 3


typedef struct {
    char* Name;
    u8 (*Func)(void);
} Command;


void Interp(char c);

char To_UpperCase (char c);

u8 Get_Param_Float (float *retour);
u8 Get_Param_u32(u32 *retour);
u8 Get_Param_x32(u32 *retour);

u8 Get_Param_End_String(char Dest_Str[], u8 Max_Len);

u8 Print_All_CMD_Cmd (void);
void Print_All_Cmd_Loop (void);


#endif