// STS_Uart_Half_Duplex.h
#ifndef STS_UART_HALF_DUPLEX_H
#define STS_UART_HALF_DUPLEX_H


#define STS_UART_HALF_BUFF_LENGTH 20
#define STS_UART_HALF_PIN 21

#define STS_UART_HALF_DEFAULT_BAUD 115200 //115200  

#define STS_UART_DIR 27

#define STS_PIO_UART_HALF pio1


void STS_Uart_Half_Duplex_Init(void);
void STS_Uart_Half_Duplex_Loop(void);

void STS_Uart_Half_Duplex_Send(uint8_t Tab[], uint8_t nb_to_send);
bool STS_Uart_Half_Duplex_Send_Done(void);

void STS_Uart_Half_Duplex_Set_Freq(float Wanted_Freq);
uint8_t STS_Uart_Half_Duplex_Set_Freq_Func (void);


extern uint8_t STS_Uart_Half_Receive_Tab[STS_UART_HALF_BUFF_LENGTH];
extern uint8_t STS_Uart_Half_Receive_Ptr;
extern uint32_t STS_Uart_Half_Duplex_Last_Receive_Time;



#endif
