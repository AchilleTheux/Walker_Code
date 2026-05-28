
#ifndef __SETUP_FILE_H
#define __SETUP_FILE_H


// besoin que soit d�fini dans le fichier Flash.h les nombres suivants : 
//#define FL_SETUP_FILE_BLOCK

#define FL_SETUP_ADD          (FL_SETUP_FILE_BLOCK*FL_SECTOR_BYTES)          // Calcul de l'adresse de base
#define FL_SETUP_ADD_MAX      (FL_SETUP_ADD + FL_SECTOR_BYTES - 5)    // Calcul de l'adresse de fin // on reserve un peu de marge pour qu'il reste des 0xFF � la fin 


void Setup_File_Init(void);
void Setup_File_Loop(void);
uint8_t Setup_File_Get(uint8_t *c);
uint8_t Setup_File_Is_Iddle(void);
uint8_t Setup_File_Erase_Func(void);
uint8_t Setup_File_Start_Read_Func(void);
uint8_t Setup_File_Reload_Func(void);
uint8_t Setup_File_Add_Func(void);


#endif
