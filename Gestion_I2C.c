#include "main.h"


volatile uint8_t State_I2C = 0;

uint16_t I2C_timer; //stockage du temps
uint8_t I2C_Maxtimed = 0;
uint8_t Maxtimed_Sate = 0;
uint8_t I2C_Nacked = 0;
uint8_t Nacked_State = 0;

uint8_t I2C_i = 0;

I2C_command I2C_Command_List[I2C_CMD_LIST_SIZE];
I2C_command Current_I2C_Cmd;

uint8_t I2C_Command_List_Done = 0;
uint8_t I2C_Command_List_Todo = 0;

uint32_t true_I2C1_BR;

void Init_I2C(void) {

    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    true_I2C1_BR = i2c_init(i2c0, 400 * 1000);


    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    gpio_set_drive_strength(I2C_SDA_PIN, GPIO_DRIVE_STRENGTH_2MA);
    gpio_set_drive_strength(I2C_SCL_PIN, GPIO_DRIVE_STRENGTH_2MA);

    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

}

// void __attribute__((interrupt,auto_psv)) _MI2C1Interrupt (void) {
    
//     IFS1bits.MI2C1IF = 0;
//     if ((State_I2C == 10) || (State_I2C == 20)) {   // attente de l'envoi du start ou du restart
//         if (I2C1STATbits.S == 1) { // in case of start or restart event
//             I2C_i = 0;
//             if (State_I2C == 10) {  //send Wr device addr
//                 I2C1TRN = (Current_I2C_Cmd.i2c_addr << 1) + 0; // 7bits addr + R/W bit
//             } else {                 //send Rd device addr
//                 I2C1TRN = (Current_I2C_Cmd.i2c_addr << 1) + 1; // 7bits addr + R/W bit
//             }
//             State_I2C ++;
//         }
//     } else if (State_I2C == 11) {  // phase d'envoi, adresse puis data
//         if (!I2C1STATbits.TBF && !I2C1STATbits.TRSTAT) {    // envoi actuel fini
//             if (!I2C1STATbits.ACKSTAT) {    // si il y a bien eu ACK du slave
//                 if (I2C_i < Current_I2C_Cmd.nbr_byte_to_send) {    // tant qu'on a qqchose � envoyer, on envoie
//                     I2C1TRN = Current_I2C_Cmd.data_send[I2C_i];
//                     I2C_i ++;
//                     // on reste dans le meme etat
//                 } else if (Current_I2C_Cmd.nbr_byte_to_read) { // si on a qqchose � aller lire
//                     I2C1CONbits.RSEN = 1;   // ReStart
//                     State_I2C = 20;
//                 } else {
//                     I2C1CONbits.PEN = 1;    // envoi du stop
//                     State_I2C = 30;
//                 }
//             } else {
//                 I2C_Nacked = 1;
//                 Nacked_State = State_I2C;
//                 I2C1CONbits.PEN = 1;    // envoi du stop
//                 State_I2C = 30;
//             }
//         }
        
//     } else if (State_I2C == 21) {    // on a envoy� l'adresse read
//         if (!I2C1STATbits.TBF && !I2C1STATbits.TRSTAT) {    // envoi fini
//             if (!I2C1STATbits.ACKSTAT) {    // si il y a bien eu ACK du slave
//                 I2C1CONbits.RCEN = 1; //allow the master to receive data
//                 State_I2C ++;
//             } else {
//                 I2C_Nacked = 1;
//                 Nacked_State = State_I2C;
//                 I2C1CONbits.PEN = 1;    // envoi du stop
//                 State_I2C = 30;
//             }
//         }
//     } else if (State_I2C == 22) {  // attente reception
//         if (I2C1STATbits.RBF) {
//             Current_I2C_Cmd.data_read[I2C_i] = I2C1RCV;
//             I2C_i++;
//             if (I2C_i < Current_I2C_Cmd.nbr_byte_to_read) {
//                 I2C1CONbits.ACKDT = 0; //ack
//                 I2C1CONbits.ACKEN = 1;
//             } else {
//                 I2C1CONbits.ACKDT = 1; //nack
//                 I2C1CONbits.ACKEN = 1;
//             }
//             State_I2C ++;
//         }
//     } else if (State_I2C == 23) {  // attente de la fin de l'ACK to slave
//         if (!I2C1CONbits.ACKEN) {
//             if (I2C_i < Current_I2C_Cmd.nbr_byte_to_read) {
//                 I2C1CONbits.RCEN = 1; //allow the master to receive data
//                 State_I2C --;
//             } else {
//                 I2C1CONbits.PEN = 1;    // envoi du stop
//                 State_I2C = 30;
//             }
//         }
//     } else if (State_I2C == 30) {
//         if (!I2C1CONbits.PEN) {
//             State_I2C = 40;
//         }
//     }
// }

void Add_I2C_command(uint8_t i2c_addr, uint8_t nbr_byte_to_send, uint8_t nbr_byte_to_read, uint8_t data_send[], uint8_t data_read[], void *Done){
    uint8_t Loc_Todo = I2C_Command_List_Todo;
    I2C_Command_List[Loc_Todo].nbr_byte_to_send = nbr_byte_to_send;
    I2C_Command_List[Loc_Todo].nbr_byte_to_read = nbr_byte_to_read;
    I2C_Command_List[Loc_Todo].data_read = data_read;
    I2C_Command_List[Loc_Todo].data_send = data_send;
    I2C_Command_List[Loc_Todo].i2c_addr = i2c_addr;
    I2C_Command_List[Loc_Todo].Done = Done;

    Loc_Todo++;
    if(Loc_Todo == I2C_CMD_LIST_SIZE)
        Loc_Todo = 0;
    I2C_Command_List_Todo = Loc_Todo;
}

void Gestion_I2C_Master_Loop(void) {

    if(I2C_Command_List_Todo != I2C_Command_List_Done) {
        Current_I2C_Cmd = I2C_Command_List[I2C_Command_List_Done];

        //uint8_t *ptr_on_done = (uint8_t*)(I2C_Command_List[I2C_Command_List_Done].Done);

        bool must_I2C_wr = Current_I2C_Cmd.nbr_byte_to_send != 0;
        bool must_I2C_rd = Current_I2C_Cmd.nbr_byte_to_read != 0;

        if (must_I2C_wr) {
            // s'il faut lire apres, pas de stop
            i2c_write_blocking(i2c0, Current_I2C_Cmd.i2c_addr, Current_I2C_Cmd.data_send, Current_I2C_Cmd.nbr_byte_to_send, must_I2C_rd);
        }
        if (must_I2C_rd) {
            i2c_read_blocking(i2c0, Current_I2C_Cmd.i2c_addr, Current_I2C_Cmd.data_read, Current_I2C_Cmd.nbr_byte_to_read, false);
        }

        *(uint8_t*)(Current_I2C_Cmd.Done) = 1;

        //*ptr_on_done = 1;
        I2C_Command_List_Done++;
        if(I2C_Command_List_Done == I2C_CMD_LIST_SIZE)
            I2C_Command_List_Done = 0;
    }

    // switch (State_I2C) {
    //     case 0:
    //         if(I2C_Command_List_Todo != I2C_Command_List_Done) {
    //             I2C_Nacked = 0;
    //             I2C_Maxtimed = 0;
    //             IFS1bits.MI2C1IF = 0;
    //             Current_I2C_Cmd = I2C_Command_List[I2C_Command_List_Done];
    //             if (!Current_I2C_Cmd.nbr_byte_to_read && !Current_I2C_Cmd.nbr_byte_to_send) {
    //                 State_I2C = 40;     // si rien a faire direct fin
    //             } else {
    //                 I2C_timer = Timer_ms1;
    //                 if (Current_I2C_Cmd.nbr_byte_to_send) {
    //                     State_I2C = 10;
    //                 } else {
    //                     State_I2C = 20;
    //                 }
    //                 I2C1CONbits.SEN = 1; // Initiates the Start condition on the SDAx and SCLx pins
    //             }
    //         }
    //         break;
            
    //     // attente en IT :
    //     case 10:    // Start pour Write
    //     case 11:    // envoi de l'adresse Wr puis des donnees
            
    //     case 20:    // (Re) Start pour Read
    //     case 21:    // envoi adresse Rd
    //     case 22:    // atente reception donnees
    //     case 23:    // envoi de l'ACK
            
    //     case 30 :   // envoi du Stop
            
    //         if ((Timer_ms1 - I2C_timer) > 30){ //max time
    //             I2C_Maxtimed = 1;
    //             Maxtimed_Sate = State_I2C;
    //             State_I2C = 100; 
    //         }
    //         break;
            
    //     case 40:
    //         // previent qu'on a fini :
            
    //         I2C_Command_List_Done++;
    //         if (I2C_Command_List_Done >= I2C_CMD_LIST_SIZE){
    //             I2C_Command_List_Done = 0;  
    //         }
            
    //         if (I2C_Maxtimed) {
    //             *((uint8_t*)Current_I2C_Cmd.Done) = 4;
    //             State_I2C = 41;
    //         } else if (I2C_Nacked) {
    //             *((uint8_t*)Current_I2C_Cmd.Done) = 2;
    //             State_I2C = 42;
    //         } else {
    //             *((uint8_t*)Current_I2C_Cmd.Done) = 1;
    //             State_I2C = 0;
    //         }
    //         break;
    //     case 41:
    //         printf("I2C_Maxtimed in %u\n", (int)(Maxtimed_Sate));
    //         State_I2C = 45;
    //         break;
    //     case 42:
    //         printf("I2C_Nacked in %u\n", (int)(Nacked_State));
    //         State_I2C = 45;
    //         break;
            
    //     case 45:
    //         I2C_timer = Timer_ms1;
    //         I2C1CONbits.I2CEN = 0;
    //         I2C_SCL_PIN = 0;
    //         I2C_SCL_TRIS = 0;   // fabrique un coup d'horloge sur l'I2C
    //         State_I2C ++;
    //         break;
    //     case 46:
    //         if ((Timer_ms1 - I2C_timer) > 1) {
    //             I2C_timer = Timer_ms1;
    //             I2C_SCL_PIN = 1;
    //             I2C_SCL_TRIS = 1;   // relache l'I2C
    //             I2C1CONbits.I2CEN = 1;
    //             State_I2C ++;
    //         }
    //         break;
    //     case 47:
    //         if ((Timer_ms1 - I2C_timer) > 1) {
    //             State_I2C = 0;
    //         }
    //         break;
    //     default:
    //         printf("Unexpected I2C_State : %d\n", State_I2C);
    //         State_I2C = 40;		// "faux" fini
    //         break;
    // }
}



uint8_t I2C_Discover_Addr = 0;
uint8_t I2C_Discover_Data = 0;
uint16_t I2C_Disco_Timer = 0;
uint8_t I2C_Discover_Done = 0;

void I2C_Discover_Loop(void)
{
    // if (I2C_Discover_Addr) {
    //     if (I2C_Discover_Done && ((Timer_ms1 - I2C_Disco_Timer) > 30)) {
    //         I2C_Disco_Timer = Timer_ms1;
    //         if (I2C_Discover_Addr > 1) {    // si c'est pas la premiere
    //             if (I2C_Discover_Done == 1) {
    //                 printf(" OK\n");
    //             } else {
    //                 printf(" NOK\n");
    //             }
    //         }
    //         if (I2C_Discover_Addr > 127) {
    //             I2C_Discover_Addr = 0;
    //         } else {
    //             printf("Trying Addr %3d 0x%02X ", I2C_Discover_Addr, I2C_Discover_Addr);
    //             I2C_Discover_Done = 0;
    //             I2C_Discover_Data = 0;
    //             Add_I2C_command(I2C_Discover_Addr, 1, 0, &I2C_Discover_Data, &I2C_Discover_Data, &I2C_Discover_Done);
    //             I2C_Discover_Addr ++;
    //         }
    //     }
    // }
}
// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

uint8_t I2C_Discover_Cmd(void)
{
    // I2C_Discover_Addr = 1;
    // I2C_Discover_Done = 1;


   
    u32 val32;
    if (!Get_Param_u32(&val32)) {    // si y a param, on le prend
        true_I2C1_BR = i2c_init(i2c0, val32);
        printf("BR is %lu\n", true_I2C1_BR);
    }

    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
 

    for (int addr = 0; addr < 128; addr++) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else {
            ret = i2c_read_blocking(i2c0, addr, &rxdata, 1, false);
        }


        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");

        for (uint32_t i = 0; i < 0x000FFFFF; i++);
    }
    printf("Done.\n");
    return 0;
}

