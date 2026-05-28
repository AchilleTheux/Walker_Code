
#ifndef __LOG_FILE_H
#define __LOG_FILE_H


// besoin que soit d�fini dans le fichier Flash.h les nombres suivants : 
// le fichier log se mets dans la derniere bank
//#define FL_LOGFILE_BLOCK                                              // secteur de d�but du fichier log        
//#define FL_LOGFILE_LENGTH                                             // longeur max du fichier log pour ne pas rentrer dans le prochain truc (fichier trace)

// FL_LOG_ADD : le fichier log se mets dans la derniere bank, juste avant le fichier trace
#define FL_LOG_ADD          (FL_LOGFILE_BLOCK*FL_SECTOR_BYTES)                      // Calcul de l'adresse de base
#define FL_LOG_ADD_MAX      (FL_LOG_ADD + FL_LOGFILE_LENGTH*FL_SECTOR_BYTES - 1)    // Calcul de l'adresse de fin // on reserve un octet pour qu'il reste un 0xFF � la fin 

#define INIT_LOG_STRING     "Time(ms):what\r\n"


#define LOG_FILE_TEMP_BUFF_SIZE 100

#define LOG_MUST_ERASE 1
#define LOG_MUST_INIT 2


int Flash_LogFile_Init (void);

void log_puts(const char *str);
void Log_Add(const char *str);
void Flash_LogFile_Loop (void);


u8 Flash_LogFile_RESET_Func(void);  
u8 InitLog_Func(void);

u32 Get_Remaining_Place_In_Log();


u8 printLog_Func(void);
u8 Log_Fill_Test_Func(void);
u8 Log_Add_Func(void);

void Log_Print_Save_Loop(void);
u8 Is_Log_Print_Save_Running (void);


#endif
