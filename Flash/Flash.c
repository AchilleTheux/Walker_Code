#include "main.h"







u8 Dummy_DONE;

void Falsh_Wr_Wrapper(u32 nbr_data, u32 addr, uint8_t *Ptr_on_u8)
{
    u32 val32, i;
    uint8_t Temp[256];
    for (i = 0; i < 256; i++) {
        Temp[i] = 0xFF;
    }
    for (i = 0; i < nbr_data; i++) {
        Temp[(i+addr)&0xFF] = Ptr_on_u8[i];
    }
    //multicore_lockout_start_blocking(); // bloque le 2eme coeur
    uint32_t IT_Status = save_and_disable_interrupts();
    flash_range_program(addr & 0xFFFFFF00, &Temp[0], 256);
    restore_interrupts(IT_Status);
    //multicore_lockout_end_blocking();   // libere le 2eme coeur

}

//-----------------------Wr
u32 Flash_wr(u32 nbr_data, u32 addr, void * ptr_on_data )
{
    u8 *Ptr_on_u8 = (u8*)(ptr_on_data);
    u32 val32;
    while (nbr_data != 0) {
        if ( ( (addr % FL_PAGE_BYTES) + nbr_data) > FL_PAGE_BYTES) {   // si addr+nbr data passe � la page d'apr�s on �crit tout ce que l'on peut
            val32 =  (FL_PAGE_BYTES -(addr%FL_PAGE_BYTES));
            // ADD_DMA_Flash_Request(DMA_SPI_FLASH_WR, addr, val32 , Ptr_on_u8, &Dummy_DONE);
            Falsh_Wr_Wrapper(val32, addr, Ptr_on_u8);
            nbr_data -= val32;
            Ptr_on_u8 =  &Ptr_on_u8[val32];
            addr += val32;
        } else {    // sinon, on est � la fin de l'�criture
            // ADD_DMA_Flash_Request(DMA_SPI_FLASH_WR, addr, nbr_data , Ptr_on_u8, ((u8*)(&Flash_WR_DONE)));
            Falsh_Wr_Wrapper(nbr_data, addr, Ptr_on_u8);
            addr += nbr_data;
            nbr_data = 0;
        }
    }
    return  addr;
}


//-----------------------Wr avec un Done Externe
u32 Flash_wr_Ext_Done(u32 nbr_data, u32 addr, void *ptr_on_data, volatile void *ptr_On_Done )
{
    addr = Flash_wr(nbr_data, addr, ptr_on_data);
    *((volatile u8*)(ptr_On_Done)) = 1;
    return addr;

    // u32 val32;
    // u8 *Ptr_on_u8 = (u8*)(ptr_on_data);
    // if (nbr_data != 0) {
    //     while (nbr_data != 0) {
    //         // while (!Place_In_DMA_Flash_Requests()) {
	// 		// 	Flash_Handler_Loop();
	// 		// 	SPI_Low_Handler_Loop();
	// 		// }
    //         if ( ( (addr % FL_PAGE_BYTES) + nbr_data) > FL_PAGE_BYTES) {   // si addr+nbr data passe � la page d'apr�s on �crit tout ce que l'on peut
    //             val32 =  (FL_PAGE_BYTES -(addr%FL_PAGE_BYTES));
    //             //ADD_DMA_Flash_Request(DMA_SPI_FLASH_WR, addr, val32 , Ptr_on_u8, &Dummy_DONE);
    //             nbr_data -= val32;
    //             Ptr_on_u8 =  &Ptr_on_u8[val32];
    //             addr += val32;
    //         } else {    // sinon, on est � la fin de l'�criture
    //             // ADD_DMA_Flash_Request(DMA_SPI_FLASH_WR, addr, nbr_data , Ptr_on_u8, ptr_On_Done);
    //             addr += nbr_data;
    //             nbr_data = 0;
    //             *((volatile u8*)(ptr_On_Done)) = 1;
    //         }
    //     }
    // } else {
    //     *((volatile u8*)(ptr_On_Done)) = 1;
    // }
    return  addr;
}

//--------------------------Rd
u32 Flash_rd(u32 nbr_data, u32 addr, void * ptr_on_data )
{
    uint8_t *ptr_on_flash = (uint8_t *)(FLASH_BASE_INT_ADDR + addr);
    u8 *Ptr_on_u8 = (u8*)(ptr_on_data);

    uint32_t i = 0;
    while (i != nbr_data) {
        Ptr_on_u8[i] = ptr_on_flash[i];
        i ++;
    }
    return addr + nbr_data;
} 

//--------------------------Rd avec un Done externe
u32 Flash_rd_Ext_Done(u32 nbr_data, u32 addr, void *ptr_on_data, volatile void *ptr_On_Done )
{
    addr = Flash_rd(nbr_data, addr, ptr_on_data);
    *((volatile u8*)(ptr_On_Done)) = 1;
    return addr;
} 


// ----------------------- Erase sector
//volatile u8 Flash_ER_DONE = 0;

void Flash_sectorerase(u32 num_sector)
{
    u32 addr = FL_SECTOR_BYTES * num_sector;
    //multicore_lockout_start_blocking(); // bloque le 2eme coeur
    uint32_t IT_Status = save_and_disable_interrupts();
    flash_range_erase(addr, FL_SECTOR_BYTES);
    restore_interrupts(IT_Status);
    //multicore_lockout_end_blocking();   // libere le 2eme coeur
    // //volatile u8 Flash_ER_DONE = 0;
    
    // while (!Place_In_DMA_Flash_Requests()) {
	// 	Flash_Handler_Loop();
	// 	SPI_Low_Handler_Loop();
	// }
    // //Flash_ER_DONE = 0;
    // ADD_DMA_Flash_Request(DMA_SPI_FLASH_ER, addr, 1, 0, &Dummy_DONE);
    // //ADD_DMA_Flash_Request(DMA_SPI_FLASH_ER, addr, 1, 0, ((u8*)(&Flash_ER_DONE)));
    // //while (!Flash_ER_DONE);
    // // on se moque de quand c'est fini
}

u8 Flash_Sector_is_Full_FF(u32 num_sector)
{
    u8 will_return = 1;
    u32 addr = num_sector*FL_SECTOR_BYTES;
    u32 last_addr = addr + FL_SECTOR_BYTES;
    u32 i;
    u8 big_buff[FL_PAGE_BYTES] = {0};
    
    while ( (addr < last_addr) && (will_return) ) {
        addr = Flash_rd(FL_PAGE_BYTES, addr, &big_buff[0]);
        for (i = 0; i < FL_PAGE_BYTES; i++) {
            if (big_buff[i] != 0xFF)
                will_return = 0;
        }
    }
    return will_return;
}

u8 Flash_isnotready(void)
{   
	return 0;		// =1 si DMA n'est pas au repos
}   

// pour savoir combien on peut �crire jusq'� la fin de la page
// plutot simple, mais permetra de garde les algos si on change de flash
u32 Flash_Get_Max_Chunk_Size (u32 addr)
{
    return FL_PAGE_BYTES - (addr % FL_PAGE_BYTES);   
}


// Commandes terminal :
// ***********************

u8 FlashRd_Func(void)
{
    u32 add;
    u32 val32;
    if(Get_Param_x32(&add))  return PARAM_ERROR_CODE;
	Flash_rd(4,add,&val32);
	printf("0x%lX\n",val32);
    return 0;
}

u8 FlashWr_Func(void)
{
    u32 add;
    u32 val32;
    if(Get_Param_x32(&add))  return PARAM_ERROR_CODE;
	if(Get_Param_x32(&val32))  return PARAM_ERROR_CODE;
	Flash_wr(4,add,&val32);
    return 0;
}

u8 FlashEr_Func(void)
{
    u32 val;
    if(Get_Param_x32(&val))  return PARAM_ERROR_CODE;
    if (val >= FL_NB_SECTOR)  return PARAM_OUT_OF_RANGE_ERROR_CODE;
	
    printf("Erase sector 0x%lX..", val);
    Flash_sectorerase(val);
    printf(" Ok!\r\n");
    return 0;
}

u8 FlashRawSector_Rd_Func(void)  // pas vraiment un secteur entier...
{
    u32 add,i;
    u8 buff[512];
    if(Get_Param_x32(&add))  return PARAM_ERROR_CODE;
    
    printf("Reading address 0x%08lX:\r\n", add);
    
    Flash_rd(512,add,&buff[0]);
    
    for (i = 0; i < 512; i++) {
        printf("%02X", buff[i]);
        if ((i&0x1F) == 0x1F ) printf("\r\n");
    }
    return 0;
}


