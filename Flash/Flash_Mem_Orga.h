
#ifndef __FLASH_MEM_ORGA_H
#define __FLASH_MEM_ORGA_H


// flash rp2040_zero : autant que rpi pico !
// flash pico = 32 secteurs
// 1 secteur = 64ko

// flash pour le code standard : 14 premiers secteurs = 896ko
#define FL_CODE_AREA_BLOCK  0
#define FL_CODE_AREA_LENGTH 14

#define FL_REPROG_CODE_BLOCK  14
#define FL_REPROG_CODE_LENGTH 14

// fichier setup en block 28
#define FL_SETUP_FILE_BLOCK 28

#define FL_LOGFILE_BLOCK        29         // secteur de debut du fichier log        
#define FL_LOGFILE_LENGTH       3         // longeur max du fichier log pour ne pas rentrer dans le prochain truc


#endif


