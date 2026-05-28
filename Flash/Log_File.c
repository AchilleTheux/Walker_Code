
#include "main.h"



// Log File
// Ecriture s�quencielle en flash
// Flash_LogFile_Init() : inititalise le pointeur m�moire (Flash_LogFile_add) par recherche dun premier FF au d�marrage du syst�me.
// Flash_LogFile_puts(*str) : Ecrit par page, et met � jour Flash_LogFile_add.
// Flash_LogFile_RESET() : efface le buffer

volatile u32 Flash_LogFile_add=FL_LOG_ADD;    // M�moire adresse fichier log

// variables pour l'affichage du log en boucle

u8 Log_Must_Print_Save = 0;    // 1 = print, 2 = save
u8 Status_Log_Print_Save = 0;

volatile u8 Buff_Log_Print[101];
volatile u8 Buff_Log_Print_Flash_Rd_Done = 1;


uint32_t Last_Timer_Log_Print;

u8 Log_File_Temp_Buff[LOG_FILE_TEMP_BUFF_SIZE];
u32 Log_File_Temp_Buff_TODO = 0;
u32 Log_File_Temp_Buff_DONE = 0;

volatile u8 Log_File_Temp_Buff_Flash_Wr_Done = 1;

u8  Must_Fill_Log = 0;
u32 i_Must_Fill_Log = 0;


u8 Must_Erase_Init_Log = 0;



int Flash_LogFile_Init (void)                                         // Open file
{
    Must_Erase_Init_Log = LOG_MUST_INIT;
    
    Log_File_Temp_Buff_TODO = 0;
    Log_File_Temp_Buff_DONE = 0;
    Flash_LogFile_add = FL_LOG_ADD;         // Pointer la fin courante du fichier : Recherche du premier 0xFF dans la flash
    
    return 0;
}

void log_puts(const char *str)                        // Ecriture de STR dans la flash
{
    u16 i = 0;
    
    while (str[i] != 0) {
        Log_File_Temp_Buff[Log_File_Temp_Buff_TODO] = str[i];
        Log_File_Temp_Buff_TODO ++;
        if (Log_File_Temp_Buff_TODO >= LOG_FILE_TEMP_BUFF_SIZE) {
            Log_File_Temp_Buff_TODO = 0;
        }
        i ++;
    }
}

void Log_Add(const char *str)
{
    char Date[30];
    //sprintf(&Date[0], "%2d,%2d,%2d,%2d,", My_Time.Day, My_Time.Hour, My_Time.Min, My_Time.Sec);
    sprintf(&Date[0], "%8d:", Timer_ms1);
    log_puts(&Date[0]);
    log_puts(str);
}


void Flash_LogFile_Loop (void)
{
    if (Must_Erase_Init_Log) {
        if (Must_Erase_Init_Log & LOG_MUST_ERASE) {
            if (Flash_LogFile_add & 0x80000000) {
                if (!Flash_isnotready()) {
                    Flash_LogFile_add &= ~0x80000000;
                    if (Flash_LogFile_add >= (FL_LOGFILE_BLOCK + FL_LOGFILE_LENGTH)) {
                        printf(" Ok!\r\n");
                        Must_Erase_Init_Log &= ~LOG_MUST_ERASE;
                        Flash_LogFile_Init();
                        
                        // My_Time.Day = 0;
                        // My_Time.Hour = 0;
                        // My_Time.Min = 0;
                        // My_Time.Sec = 0;
                    }
                }
            } else { 
                if (!Flash_Sector_is_Full_FF(Flash_LogFile_add)) {
                    Flash_sectorerase(Flash_LogFile_add);
                    printf("e");
                } else {
                    printf(".");
                }
                Flash_LogFile_add ++;
                Flash_LogFile_add |= 0x80000000;
            }
            Log_File_Temp_Buff_TODO = 0;
            Log_File_Temp_Buff_DONE = 0;
            
        } else if (Must_Erase_Init_Log & LOG_MUST_INIT) {
            u8 locbuf[FL_PAGE_BYTES];
            u16 i;
            u8 found = 0;
            // Pointer la fin courante du fichier : Recherche du premier 0xFF dans la flash
            Flash_rd(FL_PAGE_BYTES, Flash_LogFile_add, &locbuf[0]);
            if (locbuf[FL_PAGE_BYTES-1] == 0xFF) {      // si le dernier octet de ce buffer est vide, alors faut regarder tout le buffer
                i = 0;
                while (!found) {
                    if (locbuf[i] == 0xFF)
                        found = 1;
                    else
                        i ++;
                }
                Flash_LogFile_add += i;
            } else {        // sinon on passe desuite � la page suivante
                Flash_LogFile_add += FL_PAGE_BYTES; 
            }
            if ((found) || (Flash_LogFile_add >= FL_LOG_ADD_MAX)) {
                Must_Erase_Init_Log &= ~LOG_MUST_INIT;
                Log_File_Temp_Buff_Flash_Wr_Done = 1;

                Log_File_Temp_Buff_TODO = 0;
                Log_File_Temp_Buff_DONE = 0;
                if (Flash_LogFile_add == FL_LOG_ADD) {
                    log_puts(INIT_LOG_STRING);
                }
                printf("Log_Init Done\r\n");
                
            }
        } else {
            Must_Erase_Init_Log = 0;
        }
        
    } else if (Log_File_Temp_Buff_DONE != Log_File_Temp_Buff_TODO) {
        if (Log_File_Temp_Buff_Flash_Wr_Done) {
            // combien d'octet � transferer : 
            u32 Count_Data = 0;
            if (Log_File_Temp_Buff_TODO < Log_File_Temp_Buff_DONE) {
                Count_Data = LOG_FILE_TEMP_BUFF_SIZE - Log_File_Temp_Buff_DONE;
            } else {
                Count_Data = Log_File_Temp_Buff_TODO - Log_File_Temp_Buff_DONE;
            }
            
            u32 Max_Chunk = Flash_Get_Max_Chunk_Size(Flash_LogFile_add);
            Count_Data = Min_Ternaire(Count_Data, Max_Chunk);
            
            if ((Count_Data + Flash_LogFile_add) < FL_LOG_ADD_MAX) {
                Log_File_Temp_Buff_Flash_Wr_Done = 0;
                Flash_LogFile_add = Flash_wr_Ext_Done(Count_Data, Flash_LogFile_add, &Log_File_Temp_Buff[Log_File_Temp_Buff_DONE], &Log_File_Temp_Buff_Flash_Wr_Done);
            }
            u32 Next_DONE = (Log_File_Temp_Buff_DONE + Count_Data) % LOG_FILE_TEMP_BUFF_SIZE;
            Log_File_Temp_Buff_DONE = Next_DONE;
            //printf("%ld\r\n", Flash_LogFile_add);
            
        }
    } else if (Must_Fill_Log) {
        char Temp[100];
        sprintf(&Temp[0], "%lud:25.78;24.78;26.78;28.78\n", i_Must_Fill_Log);
        //printf(&Temp[0]);
        i_Must_Fill_Log ++;
        log_puts(&Temp[0]);
    }
}



u8 Flash_LogFile_RESET_Func(void)       // Effacement de toute la zone log, reinit Flash_LogFile_add au deb
{
    Must_Erase_Init_Log = LOG_MUST_ERASE;
    Flash_LogFile_add = FL_LOGFILE_BLOCK;
    printf("Log file Erase ");
    /*
    
    LOG_MUST_INIT
    u16 sectortoerase = FL_LOGFILE_BLOCK;
    
    
    while(sectortoerase<(FL_LOGFILE_BLOCK + FL_LOGFILE_LENGTH)) {
        if (!Flash_Sector_is_Full_FF(sectortoerase)) {
            Flash_sectorerase(sectortoerase);
            printf("e");
        } else {
            printf(".");
        }
        while (Flash_isnotready());
        sectortoerase ++;
    }
    Flash_LogFile_Init();
    printf(" Ok!\r\n");*/
    return 0;
}

u8 InitLog_Func(void)
{
    Flash_LogFile_Init();
    return 0;
}

u32 Get_Remaining_Place_In_Log (void)
{
    return (FL_LOG_ADD_MAX - Flash_LogFile_add);
}

u8 printLog_Func(void)
{
    Log_Must_Print_Save = 1;
    Status_Log_Print_Save = 0;
    return 0;
}

u8 Log_Fill_Test_Func(void)
{
    Must_Fill_Log = !Must_Fill_Log;
    i_Must_Fill_Log = 0;
    printf("Must Fill Log = %d\r\n", Must_Fill_Log);
    return 0;
}

u8 Log_Add_Func(void)
{
    char Test_Str[120];
    if (!Get_Param_End_String(&Test_Str[0], 100)) return PARAM_ERROR_CODE;
    strcat(&Test_Str[0], "\r\n");
    Log_Add(&Test_Str[0]);
    return 0;
}


void Log_Print_Save_Loop(void)
{
    static u8 Pos_Last = 0;
    static u32 Addr_Log_Rd_Print_Save = 0;
    
    if (Log_Must_Print_Save) {
        
        switch (Status_Log_Print_Save) {
            case 0:
                Addr_Log_Rd_Print_Save = FL_LOG_ADD;
                Status_Log_Print_Save = 3;  // go direct � l'aiguillage
                Pos_Last = 100;     // permier coup, on va �crire le nom et num�ro de s�rie avant de lire le contenu pour de vrai...
                sprintf((char*)(&Buff_Log_Print[0]), "Log\r\n");
                break;
            case 1:
                Buff_Log_Print_Flash_Rd_Done = 0;
                Addr_Log_Rd_Print_Save = Flash_rd_Ext_Done(100, Addr_Log_Rd_Print_Save, (void*)(&Buff_Log_Print[0]), &Buff_Log_Print_Flash_Rd_Done);
                Status_Log_Print_Save = 2;
                break;
            case 2:
                if (Buff_Log_Print_Flash_Rd_Done) {
                    Pos_Last = 0;
                    while ((Buff_Log_Print[Pos_Last] != 0xFF) && (Pos_Last < 100))
                    {   Pos_Last++;    }
                    Buff_Log_Print[Pos_Last] = 0;
                    // si le premier est 0xFF, c'est qu'on a fini / rien � faire
                    if (Pos_Last == 0) {
                        Log_Must_Print_Save = 0;
                        Status_Log_Print_Save = 0;
                    } else {
                        Status_Log_Print_Save = 3;
                    }
                }
                break;
            case 3:     // aiguillage
                if (Log_Must_Print_Save == 1) // print simple
                    Status_Log_Print_Save = 10; 
                break;
            case 10: // print simple
                printf((char*)(&Buff_Log_Print[0]));
                Status_Log_Print_Save = 11;
                break;
            case 11:
                Last_Timer_Log_Print = Timer_ms1;
                Status_Log_Print_Save ++;
                break;
            case 12:
                if ((Timer_ms1 - Last_Timer_Log_Print) > 10) {
                    Status_Log_Print_Save = 40;
                }
                // if (Is_Std_Out_Empty()) {
                // }
                break;
            case 40:    // si la ligne actuelle est partie, on regarde si on peut en prendre une autre ou s'il faut s'arreter :
                if (Pos_Last != 100 || (Addr_Log_Rd_Print_Save >= FL_LOG_ADD_MAX) ) { // on a trouv� un FF   ou on est all� trop loin
                    
                    Log_Must_Print_Save = 0;
                    Status_Log_Print_Save = 0;
                } else {
                    Status_Log_Print_Save = 1;
                }
                break;
                
            default : // probl�me, jamais sens� passer par ici...
                Log_Must_Print_Save = 0;
                Status_Log_Print_Save = 0;
                break;
        }
    }
}

u8 Is_Log_Print_Save_Running (void)
{
    return (Log_Must_Print_Save != 0);
}

