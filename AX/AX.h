
#ifndef __AX_H_
#define __AX_H_


#define AX_BROADCAST               254



#define AX_CMD_NB_MAX_TRY_SEND 3
#define AX_CMD_LIST_SIZE 100
#define AX_CMD_BUFF_LENGTH 20



#define AX_STATUS_PENDING         0
#define AX_STATUS_OK              1

#define AX_STATUS_UNSUPORTED_CMD  0x81
#define AX_STATUS_TIMEOUT         0x82
#define AX_STATUS_CHKSUM_ERROR    0x83


// Instruction Set
#define AX_INST_PING                1
#define AX_INST_READ_DATA           2
#define AX_INST_WRITE_DATA          3
#define AX_INST_REG_WRITE           4
#define AX_INST_ACTION              5
#define AX_INST_RESET               6
#define AX_INST_SYNC_WRITE          131




#define AX_COM_MAXTIME 5 // ms


typedef struct {
    uint8_t AX_Id;
    uint8_t Command;
    uint8_t Reg_Addr;
    uint32_t Data_To_Send;
    void *Data_Answer;
    uint8_t Nb_Data; // Max 4 en send
    uint8_t *Done_Status;
} AX_Command;


void Init_Com_AX(void);

void AX_Loop(void);

void AX_Send_To_Uart(AX_Command Cmd);

uint8_t AX_All_Cmd_Done(void);

void Add_AX_Cmd(uint8_t AX_Id, uint8_t Command, uint8_t Reg_Addr, uint32_t Data_To_Send, void *Data_Answer, uint8_t Nb_Data, uint8_t *Done_Status);


void Put_AX12(uint8_t id, uint8_t Reg, uint32_t Data, uint8_t *Done_Status);
void Get_AX12(uint8_t id, uint8_t Reg, void *Data_Answer, uint8_t *Done_Status);

void Wait_For_All_AX_Cmd(void); // interdit dans le robot, sauf pour debug !!!

#endif

