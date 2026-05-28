/*
 *    ____________      _           _
 *   |___  /| ___ \    | |         | |
 *      / / | |_/ /___ | |__   ___ | |_
 *     / /  |    // _ \| '_ \ / _ \| __|
 *    / /   | |\ \ (_) | |_) | (_) | |_
 *   /_/    |_| \_\___/|____/ \___/'\__|
 *                 7robot.fr
 */

#include "main.h"

/******************************************************************************
 * Global Variables
 ******************************************************************************/




uint8_t AX_Loop_State = 0;

static AX_Command Liste_Command_AX[AX_CMD_LIST_SIZE];

static AX_Command Current_Command;
static uint8_t Current_Cmd_Status;
static uint8_t Current_Cmd_Nb_Try;


static uint8_t Command_AX_TODO = 0;
static uint8_t Command_AX_DONE = 0;




void Init_Com_AX(void) {
    Command_AX_TODO = 0;
    Command_AX_DONE = 0;
};


void AX_Loop(void) {
    uint8_t val8, i;
    switch(AX_Loop_State) {
        ///////////////////////////// repos /////////////////////////////       
        case 0: //etat d'attente d'un envoie a effectuer
            if (Command_AX_TODO != Command_AX_DONE){
                AX_Loop_State++;      
            }
            break;
        case 1: // check et initialisations
            Current_Cmd_Nb_Try = 0;
            Current_Cmd_Status = AX_STATUS_PENDING;
            Current_Command = Liste_Command_AX[Command_AX_DONE];
            Command_AX_DONE++;
            if (Command_AX_DONE == AX_CMD_LIST_SIZE)
                Command_AX_DONE = 0;

            AX_Loop_State ++;
            break;
        case 2:
            if (((Current_Command.Command == AX_INST_READ_DATA) &&
                    (Current_Command.AX_Id != AX_BROADCAST)) || // si ce nouvel ordre est supporte (read ou write)
                    (Current_Command.Command == AX_INST_WRITE_DATA)){
                AX_Loop_State = 10;
            } else { //si non supporte on le mets en status, et on passe a la suite
                Current_Cmd_Status = AX_STATUS_UNSUPORTED_CMD;
                AX_Loop_State = 100;
            }
            break;
        
        
        ///////////////////////////// en transmission ///////////////////////////// 
        case 10:    // debut de l'envoi
            AX_Send_To_Uart(Current_Command);
            AX_Loop_State ++;
            break;
        case 11:
            // attente fin de transmission
            if (AX_Uart_Half_Duplex_Send_Done()) {
                AX_Loop_State = 20;
            }
            break;
            
        ///////////////////////////// en reception /////////////////////////////    
        case 20:
            // si c'est un broadcast, on ne s'attend meme pas a recevoir quoique ce soit
            if (Current_Command.AX_Id == AX_BROADCAST) {
                Current_Cmd_Status = AX_STATUS_OK;
                AX_Loop_State = 100;
            } else {
                AX_Loop_State++;
            }
            break;
        case 21:
            // si on a recu une tramme de logueur coherente
            if ((AX_Uart_Half_Receive_Ptr > 3) && (AX_Uart_Half_Receive_Tab[3] == (AX_Uart_Half_Receive_Ptr - 4))) {
                AX_Loop_State = 30;   // va l'analyser
            // ou si on a depasse le maxtime, pb
            } else if ((Timer_ms1 - AX_Uart_Half_Duplex_Last_Receive_Time) > AX_COM_MAXTIME) {
                Current_Cmd_Status = AX_STATUS_TIMEOUT;
                AX_Loop_State = 90;
            }
            break;
        case 30:
            // ici la tramme a forcement la bonne longueur
            // verif du checksum
            val8 = 0;
            for (i = 2; i <= (AX_Uart_Half_Receive_Tab[3] + 2); i++)
                val8 += AX_Uart_Half_Receive_Tab[i];
            // verif si checksum est bon
            val8 = ~val8;
            if (val8 == AX_Uart_Half_Receive_Tab[AX_Uart_Half_Receive_Tab[3] + 3]) { // si le checksum en reception est bon
                Current_Cmd_Status = AX_STATUS_OK;
                AX_Loop_State = 31;
            } else {
                Current_Cmd_Status = AX_STATUS_CHKSUM_ERROR;
                AX_Loop_State = 90;
            }
            break;
        case 31:
            // transfert des donnees recues si c'est une lecture
            if ((Current_Command.Command == AX_INST_READ_DATA) &&
                (Current_Command.Data_Answer != NULL)          ){ 
                
                uint8_t *ptr_on_u8 = Current_Command.Data_Answer;
                for (i = 0; i < Current_Command.Nb_Data; i++) {
                    ptr_on_u8[i] = AX_Uart_Half_Receive_Tab[i + 5];
                }
            }
            AX_Loop_State = 100;
            break;
            
        case 90:
            // maxtime, ou checksum errror, relance si y a encore des essais a faire
            Current_Cmd_Nb_Try ++;
            if (Current_Cmd_Nb_Try < AX_CMD_NB_MAX_TRY_SEND) {
                AX_Loop_State = 10;   // relance la transmission
            } else {
                AX_Loop_State = 100;
            }
            break;
            
        case 100:
            if (Current_Command.Done_Status != NULL) {
                *Current_Command.Done_Status = Current_Cmd_Status;
            }
            AX_Loop_State = 0;
            break;
            
        default:
            AX_Loop_State = 0;
            break;
    }
}

void AX_Send_To_Uart(AX_Command Cmd) {
    uint8_t i, val8;
    uint8_t AX_Transmit_Tab[AX_CMD_BUFF_LENGTH];
    uint8_t AX_Transmit_Goal;
    // preparation de la trame
    AX_Transmit_Tab [0] = 0xFF;
    AX_Transmit_Tab [1] = 0xFF;
    AX_Transmit_Tab [2] = Cmd.AX_Id;
    // la longueur du packet, dans le 3, apres

    AX_Transmit_Tab [4] = Cmd.Command;
    AX_Transmit_Tab [5] = Cmd.Reg_Addr;

    if (Cmd.Command == AX_INST_WRITE_DATA) {
        uint32_t Data_To_Send = Cmd.Data_To_Send;
        for (i = 0; i < Cmd.Nb_Data; i++) {
            AX_Transmit_Tab [6 + i] = Data_To_Send & 0xFF;
            Data_To_Send = Data_To_Send >> 8;
        }
        AX_Transmit_Tab [3] = Cmd.Nb_Data + 3; // ID + CMD + REG + x*datas
    } else if (Cmd.Command == AX_INST_READ_DATA) {
        AX_Transmit_Tab [6] = Cmd.Nb_Data;
        AX_Transmit_Tab [3] = 4; // ID + CMD + Addr_Reg + Nb data to read
    }
    
    AX_Transmit_Goal = AX_Transmit_Tab [3] + 4; //  2*FF + Len + [] + chksum
    
    // calcul du checksum
    val8 = 0;
    for (i = 2; i <= (AX_Transmit_Tab [3] + 2); i++) {
        val8 += AX_Transmit_Tab[i];
    }
    AX_Transmit_Tab[AX_Transmit_Goal - 1] = ~val8;

    AX_Uart_Half_Duplex_Send(&AX_Transmit_Tab[0], AX_Transmit_Goal);
}


uint8_t AX_All_Cmd_Done(void) {
    return ((Command_AX_TODO == Command_AX_DONE) && (AX_Loop_State == 0));
}

void Add_AX_Cmd(uint8_t AX_Addr, uint8_t Command, uint8_t Reg_Addr, uint32_t Data_To_Send, void *Data_Answer, uint8_t Nb_Data, uint8_t *Done_Status) {
    Liste_Command_AX[Command_AX_TODO].AX_Id = AX_Addr;
    Liste_Command_AX[Command_AX_TODO].Command = Command;
    Liste_Command_AX[Command_AX_TODO].Reg_Addr = Reg_Addr;
    Liste_Command_AX[Command_AX_TODO].Data_To_Send = Data_To_Send;
    Liste_Command_AX[Command_AX_TODO].Data_Answer = Data_Answer;
    Liste_Command_AX[Command_AX_TODO].Nb_Data = Nb_Data;
    Liste_Command_AX[Command_AX_TODO].Done_Status = Done_Status;

    if (Done_Status != NULL) {
        *Done_Status = AX_STATUS_PENDING;
    }

    Command_AX_TODO++;
    if (Command_AX_TODO == AX_CMD_LIST_SIZE)
        Command_AX_TODO = 0;
}


void Put_AX12(uint8_t id, uint8_t Reg, uint32_t Data, uint8_t *Done_Status) {
    Add_AX_Cmd(id, AX_INST_WRITE_DATA, Reg, Data, NULL, RegisterLen_AX12(Reg), Done_Status);
}

void Get_AX12(uint8_t id, uint8_t Reg, void *Data_Answer, uint8_t *Done_Status) {
    Add_AX_Cmd(id, AX_INST_READ_DATA, Reg, 0, Data_Answer, RegisterLen_AX12(Reg), Done_Status);
}

void Wait_For_All_AX_Cmd(void) { // interdit dans le robot, sauf pour debug !!!
    while (!AX_All_Cmd_Done()) {
        AX_Loop();
        AX_Uart_Half_Duplex_Loop();
        Timer_ms1 = time_ms_32();
    }
}


