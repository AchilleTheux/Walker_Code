    #ifndef GESTION_EXPANDER_IO_H
#define	GESTION_EXPANDER_IO_H



#define EXPANDER_IO_TIMER_REQUEST 10

#define EXPANDER_IO_BASE_ADDR  0x23  // sur carte base
#define EXPANDER_IO_POWER_ADDR 0x27  // PCA9555 with A0=A1=A2=1


#define EXPIO_CONF_REG  6
#define EXPIO_OUT_REG   2
#define EXPIO_IN_REG    0

// ExpIO sur base :

typedef struct{
    uint8_t I2C_Addr;
    uint16_t In_Reg;
    uint16_t Out_Reg;
    uint16_t Conf_Reg;
    uint8_t Loop_State;
    uint32_t Last_Timer;
    uint8_t I2C_Buff[4];
    uint8_t I2C_Done;
} ExpIO;


// Expio_Power
//#define EXPIO_EN_5V     0x0001
//#define EXPIO_AU_BTN    0x0040


#define EXPIO_VANNE1   0x0040 // PCA9554ADB_112 (I06)
#define EXPIO_VANNE2   0x0002 // PCA9554ADB_112 (IO1)
#define EXPIO_POMPE1   0x0020 // PCA9554ADB_112 (I05)
#define EXPIO_POMPE2   0x0004 // PCA9554ADB_112 (I02)

#define EXPIO_LED1      0x4000 //Pin 19 du PCA9555 (I1_6)
#define EXPIO_VBAT_DETECT      0x8000 //Pin 20 du PCA9555 (I1_7)


// extern uint8_t ExpIO_En_5V;
// extern uint8_t ExpIO_AU_Btn;

extern uint8_t ExpIO_Pompe1;
extern uint8_t ExpIO_Pompe2;
extern uint8_t ExpIO_Vanne1;
extern uint8_t ExpIO_Vanne2;
extern uint8_t ExpIO_vbat_detect;

// extern uint8_t ExpIO_Laisse;
// extern uint8_t ExpIO_Team;
// extern uint8_t ExpIO_SW1;



uint8_t ExpIO_LED_Func(void);
uint8_t ExpIO_PS_SW_Func(void);

void Expander_IO_Init(void);
void Gestion_Expandeur_Loop(void);

void Switch_Decode_Loop(void);

#endif	/* GESTION_EXPANDER_IO_H */

