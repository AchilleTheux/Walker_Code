#ifndef __GESTION_I2C_H
#define	__GESTION_I2C_H

#define I2C_DONE_OK 1
#define I2C_CMD_LIST_SIZE 30

#define I2C_WANTED_FREQ 50000   // 30kHz

typedef struct{
    uint8_t i2c_addr;
    uint8_t nbr_byte_to_send;
    uint8_t nbr_byte_to_read;
    uint8_t *data_read;
    uint8_t *data_send;
    void *Done;
}I2C_command;

void Init_I2C(void);
void Add_I2C_command(uint8_t i2c_addr, uint8_t nbr_byte_to_send, uint8_t nbr_byte_to_read,uint8_t data_send[], uint8_t data_read[], void *Done);
void Gestion_I2C_Master_Loop(void);

// I2C1
#define I2C_SDA_PIN     13
#define I2C_SCL_PIN     12


void I2C_Discover_Loop(void);
uint8_t I2C_Discover_Cmd(void);



#endif	// __GESTION_I2C_H
