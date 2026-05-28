
#ifndef __FLASH_H
#define __FLASH_H


u32 Flash_wr(u32 nbr_data, u32 addr, void *ptr_on_data);
u32 Flash_wr_Ext_Done(u32 nbr_data, u32 addr, void *ptr_on_data, volatile void *ptr_On_Done);
u32 Flash_rd(u32 nbr_data, u32 addr, void *ptr_on_data);
u32 Flash_rd_Ext_Done(u32 nbr_data, u32 addr, void *ptr_on_data, volatile void *ptr_On_Done);

void Flash_sectorerase(u32 num_sector);
u8 Flash_isnotready(void);
u32 Flash_Get_Max_Chunk_Size (u32 addr);

u8 Flash_Sector_is_Full_FF(u32 num_sector);

#ifdef _HARDWARE_FLASH_H
#define FL_PAGE_BYTES FLASH_PAGE_SIZE
#define FL_SECTOR_BYTES FLASH_BLOCK_SIZE
//#define FLASH_SECTOR_SIZE (1u << 12)
#else
#define FL_SECTOR_BYTES 0x010000   // Component Sector/Block size
#define FL_PAGE_BYTES   0x000100
#endif

//#define FL_NB_SECTOR    128
#define FL_NB_SECTOR (PICO_FLASH_SIZE_BYTES/FL_SECTOR_BYTES)

#define FLASH_BASE_INT_ADDR  0x10000000



u8 FlashRd_Func(void);
u8 FlashWr_Func(void);
u8 FlashEr_Func(void);
u8 FlashRawSector_Rd_Func(void);  // pas vraiment un secteur entier...


#endif




