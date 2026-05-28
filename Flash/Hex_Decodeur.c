#include "main.h"


uint8_t Hex_Decoder_En = 0;
uint32_t Hex_Decoder_Flash_Base_Addr = 0;
uint32_t Hex_Decoder_Max_Len = 0;

uint32_t Hex_Decoder_Addr;
uint32_t Hex_Decoder_Nb_Octets;

uint8_t Hex_Decoder_Error = 0;


uint32_t Hex_Decoder_File_Len = 0;

uint8_t Hex_Decoder_Flash_Buff[256];
uint32_t Hex_Decoder_Flash_Buff_Addr = 0;

uint8_t Must_Triger_Led = 0;

void Hex_Decoder_Flash_Buff_Init(void) {
    uint32_t i;
    for (i = 0; i < 256; i ++) {
        Hex_Decoder_Flash_Buff[i] = 0xFF;
    }
    Hex_Decoder_Flash_Buff_Addr = 0;
}

void Hex_Decoder_Flash_Buff_Purge(void) {
    uint32_t Nb_In_Buff = 0, i = 0;
    for (i = 0; i < 256; i ++) {
        if (Hex_Decoder_Flash_Buff[i] != 0xFF) {
            Nb_In_Buff = i+1;
        }
    }
    if (Nb_In_Buff) {
        uint32_t Addr =  (Hex_Decoder_Flash_Buff_Addr & ~0xFF) + Hex_Decoder_Flash_Base_Addr;
        Flash_wr(Nb_In_Buff, Addr, Hex_Decoder_Flash_Buff);
        Must_Triger_Led = 1;
        for (i = 0; i < 256; i ++) {
            Hex_Decoder_Flash_Buff[i] = 0xFF;
        }
    }
}

void Hex_Decoder_Flash_Buff_Add(uint32_t Addr, uint8_t Data)
{
    // si nouvelle page dans la flash
    if ((Addr & 0xFFFFFF00) != (Hex_Decoder_Flash_Buff_Addr & 0xFFFFFF00)) {
        Hex_Decoder_Flash_Buff_Purge();
        Hex_Decoder_Flash_Buff_Addr = Addr & 0xFFFFFF00;
    }
    Hex_Decoder_Flash_Buff[Addr & 0xFF] = Data;
}

void HEX_decodeur(char Current_Cmd[]) {
    uint32_t nbr_octet = 0; //nbr octet de donn�es dans la ligne du .hex

    if(Hex_Decoder_En) {
        if(Current_Cmd[7] == '0') { //si c'est de la data 
            nbr_octet = Hex_To_Decimal(&Current_Cmd[0], 2); //on regarde le nbr d'octet qui sont fournis
            Hex_Decoder_Addr = (Hex_Decoder_Addr & 0xFFFF0000);
            Hex_Decoder_Addr += Hex_To_Decimal(&Current_Cmd[2], 4); //partie basse (16 LSB) de l'adresse
            
            if (nbr_octet > 16) {
                Hex_Decoder_Error = 1;
            } else if (Hex_Decoder_Error) {
                // do nothing
            } else {
                u8 i;
                for(i = 0; i< nbr_octet ; i++) {
                    uint8_t Data = Hex_To_Decimal(&Current_Cmd[8+2*i], 2);
                    uint32_t Addr = Hex_Decoder_Addr + i;
                    if (Addr < Hex_Decoder_Max_Len) {
                        Hex_Decoder_Flash_Buff_Add(Addr, Data);
                        Hex_Decoder_Nb_Octets = Max_Ternaire(Hex_Decoder_Nb_Octets, Addr);
                    } else {
                        Hex_Decoder_Error = 1;
                    }
                }
                // if((Hex_Decoder_Addr + nbr_octet) < Hex_Decoder_Max_Len) {
                //     //uint32_t Addr =  Hex_Decoder_Addr+Hex_Decoder_Flash_Base_Addr+4;
                //     uint32_t Addr =  Hex_Decoder_Addr+Hex_Decoder_Flash_Base_Addr;
                //     Flash_wr(nbr_octet, Addr, Hex_Decoder_Data_Buff);
                //     Hex_Decoder_Nb_Octets = Max_Ternaire((Hex_Decoder_Addr + nbr_octet), Hex_Decoder_Nb_Octets);
                // }
            }
            
        } else if(Current_Cmd[7] == '4') { //adressage en 32 bits 
            Hex_Decoder_Addr = Hex_To_Decimal(&Current_Cmd[8], 4);
            Hex_Decoder_Addr = Hex_Decoder_Addr << 16;
            Hex_Decoder_Addr -= FLASH_BASE_INT_ADDR;    // enleve la partie dependante du fait que le 
            
        } else if(Current_Cmd[7] == '1') { // si c'est la fin du fichier, on va ecrire la longueur max vue au d�but pour servir d'entete
            Hex_Decoder_En = 0;
            if (!Hex_Decoder_Error) {
                Hex_Decoder_Flash_Buff_Purge();
                //Flash_wr(4, Hex_Decoder_Flash_Base_Addr, &Hex_Decoder_Nb_Octets);
                Hex_Decoder_File_Len = Hex_Decoder_Nb_Octets;
                printf("Download,OK\nLen %lu\n", Hex_Decoder_Nb_Octets);
            } else {
                printf("Download,ERROR\n");
            }
        }
    }
}


uint32_t Hex_To_Decimal(char HEX[], uint8_t len_tab) { //retourne le nombre en d�cimale form� par x"HEX"
    uint32_t nbr = 0;
    u8 i;
    for (i = 0; i < len_tab; i++) {
        nbr *= 16;
        if ((HEX[i] >= '0') && (HEX[i] <= '9')) {
            nbr += (HEX[i] - '0');
        } else if ((HEX[i] >= 'A') && (HEX[i] <= 'F')) {
            nbr += (HEX[i] - 'A') + 10;
        }
    }
    return nbr;
}

uint8_t HEX_Decodeur_Start_Func(void)
{
    Hex_Decoder_Addr = 0; //init addr programme du pic
    Hex_Decoder_Nb_Octets = 0;
    Hex_Decoder_Error = 0;
    Hex_Decoder_Max_Len = FL_REPROG_CODE_LENGTH*FL_SECTOR_BYTES - 5; // revoir ? 

    Hex_Decoder_Flash_Buff_Init();

    uint8_t i;
    for (i = 0; i < FL_REPROG_CODE_LENGTH; i++) {
        Flash_sectorerase(FL_REPROG_CODE_BLOCK + i);
    }
    Hex_Decoder_Flash_Base_Addr = FL_REPROG_CODE_BLOCK*FL_SECTOR_BYTES;
    Hex_Decoder_En = 1;
    return 0;
}

uint8_t HEX_Decodeur_Stop_Func(void)
{
    Hex_Decoder_En = 0; 
	return 0;
}


uint32_t Reprog_Flash_Addr_Source;
uint32_t Reprog_Flash_Len;


static void __no_inline_not_in_flash_func(flash_cs_force)(bool high) {
    uint32_t field_val = high ?
        IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_VALUE_HIGH :
        IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_VALUE_LOW;
    hw_write_masked(&ioqspi_hw->io[1].ctrl,
        field_val << IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_LSB,
        IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_BITS
    );
}

void __no_inline_not_in_flash_func(My_Flash_Transfert)(uint8_t *buf, uint32_t count) {
    flash_cs_force(0);

    uint32_t tx_remaining = count;
    uint32_t rx_remaining = count;

    while (rx_remaining) {
        uint32_t flags = ssi_hw->sr;
        bool can_put = !!(flags & SSI_SR_TFNF_BITS);
        bool can_get = !!(flags & SSI_SR_RFNE_BITS);
        if (can_put && (tx_remaining == rx_remaining)) {
            ssi_hw->dr0 = *buf;
            tx_remaining--;
        }
        if (can_get && rx_remaining) {
            *buf++ = (uint8_t)ssi_hw->dr0;
            rx_remaining--;
        }
    }
    flash_cs_force(1);
}


void __no_inline_not_in_flash_func(Reprog_All)(void)
{
   // rom_connect_internal_flash_fn connect_internal_flash = (rom_connect_internal_flash_fn)rom_func_lookup_inline(ROM_FUNC_CONNECT_INTERNAL_FLASH);
    rom_flash_exit_xip_fn flash_exit_xip = (rom_flash_exit_xip_fn)rom_func_lookup_inline(ROM_FUNC_FLASH_EXIT_XIP);
    rom_flash_enter_cmd_xip_fn flash_enter_xip = (rom_flash_enter_cmd_xip_fn)rom_func_lookup_inline(ROM_FUNC_FLASH_ENTER_CMD_XIP);
    //rom_flash_range_program_fn flash_range_program = (rom_flash_range_program_fn)rom_func_lookup_inline(ROM_FUNC_FLASH_RANGE_PROGRAM);

    //connect_internal_flash();
    
    uint8_t Flash_Buff[260];    // 256 + Cmd + 3x addr
    uint32_t Addr1 = Reprog_Flash_Addr_Source;
    uint32_t Addr2 = 0;
    uint32_t Len = (Reprog_Flash_Len | 0xFF) + 1;     // longeur a copier

    uint32_t i;
    //multicore_lockout_start_blocking(); // bloque le 2eme coeur
    uint32_t IT_Status = save_and_disable_interrupts();
    flash_exit_xip();

    uint8_t Flash_Status = 0x01;
    while (Flash_Status & 0x01) {   // check that flash is not busy 
        Flash_Buff[0] = 0x05;   // read status
        My_Flash_Transfert(Flash_Buff, 2);
        Flash_Status = Flash_Buff[1];
    }
    
    while (Len) {

        // si on est allignés sur un secteur, faut l'effacer d'abbord
        if (!(Addr2 & (FLASH_SECTOR_SIZE - 1))) {
            Flash_Buff[0] = 0x06; // WrEn
            My_Flash_Transfert(Flash_Buff, 1);

            Flash_Buff[0] = 0x20;   // Sector erase
            Flash_Buff[1] = Addr2 >> 16;
            Flash_Buff[2] = Addr2 >> 8;
            Flash_Buff[3] = Addr2;
            My_Flash_Transfert(Flash_Buff, 4);

            Flash_Status = 0x01;
            while (Flash_Status & 0x01) {   // check that flash is not busy 
                Flash_Buff[0] = 0x05;   // read status
                My_Flash_Transfert(Flash_Buff, 2);
                Flash_Status = Flash_Buff[1];
            }
        }
        
        // recupere 256 octets
        Flash_Buff[0] = 0x03;   // normal read
        Flash_Buff[1] = Addr1 >> 16;
        Flash_Buff[2] = Addr1 >> 8;
        Flash_Buff[3] = Addr1;
        My_Flash_Transfert(Flash_Buff, 260);


        Flash_Buff[0] = 0x06; // WrEn
        My_Flash_Transfert(Flash_Buff, 1);

        Flash_Buff[0] = 0x02;   // Page Programm
        Flash_Buff[1] = Addr2 >> 16;
        Flash_Buff[2] = Addr2 >> 8;
        Flash_Buff[3] = Addr2;

        // les 256 octets ont été recupérés avant

        My_Flash_Transfert(Flash_Buff, 260);

        Flash_Status = 0x01;
        while (Flash_Status & 0x01) {   // check that flash is not busy 
            Flash_Buff[0] = 0x05;   // read status
            My_Flash_Transfert(Flash_Buff, 2);
            Flash_Status = Flash_Buff[1];
        }

        Addr1 += 256;
        Addr2 += 256;
        Len -= 256;
    }

    watchdog_hw->ctrl = WATCHDOG_CTRL_TRIGGER_BITS;
}




uint8_t HEX_Decodeur_Reprog_Func(void)
{
    Reprog_Flash_Addr_Source = FL_REPROG_CODE_BLOCK*FL_SECTOR_BYTES;
    Reprog_Flash_Len = Hex_Decoder_File_Len;

    if (Reprog_Flash_Len) {
        //Flash_rd(4, Reprog_Flash_Addr_Source, &Reprog_Flash_Len);
        //Reprog_Flash_Addr_Source += 4;

        watchdog_enable(5000,0);

        Reprog_All();
    } else {
        printf("File to reprog must be transfered first !\n");
    }
	return 0;
}
