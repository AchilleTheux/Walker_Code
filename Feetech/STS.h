
#ifndef __STS_H_
#define __STS_H_


#define STS_BROADCAST               254



#define STS_CMD_NB_MAX_TRY_SEND 3
#define STS_CMD_LIST_SIZE 20
#define STS_CMD_BUFF_LENGTH 20



#define STS_STATUS_PENDING         0
#define STS_STATUS_OK              1

#define STS_STATUS_UNSUPORTED_CMD  0x81
#define STS_STATUS_TIMEOUT         0x82
#define STS_STATUS_CHKSUM_ERROR    0x83


// Instruction Set
#define STS_INST_PING                1
#define STS_INST_READ_DATA           2
#define STS_INST_WRITE_DATA          3
#define STS_INST_REG_WRITE           4
#define STS_INST_ACTION              5
#define STS_INST_RESET               6
#define STS_INST_SYNC_WRITE          131




#define STS_COM_MAXTIME 5 // ms


typedef struct {
    uint8_t STS_Id;
    uint8_t Command;
    uint8_t Reg_Addr;
    uint32_t Data_To_Send;
    void *Data_Answer;
    uint8_t Nb_Data; // Max 4 en send
    uint8_t *Done_Status;
} STS_Command;


void Init_Com_STS(void);

void STS_Loop(void);

void STS_Send_To_Uart(STS_Command Cmd);

uint8_t STS_All_Cmd_Done(void);

void Add_STS_Cmd(uint8_t STS_Id, uint8_t Command, uint8_t Reg_Addr, uint32_t Data_To_Send, void *Data_Answer, uint8_t Nb_Data, uint8_t *Done_Status);


void Put_Feetech(uint8_t id, uint8_t Reg, uint32_t Data, uint8_t *Done_Status);
void Get_Feetech(uint8_t id, uint8_t Reg, void *Data_Answer, uint8_t *Done_Status);

void Wait_For_All_STS_Cmd(void); // interdit dans le robot, sauf pour debug !!!




#endif
