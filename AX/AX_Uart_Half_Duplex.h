// Uart_Half_Duplex.h
#ifndef AX_UART_HALF_DUPLEX_H
#define AX_UART_HALF_DUPLEX_H


#define AX_UART_HALF_BUFF_LENGTH 20
#define AX_UART_HALF_PIN 21

#define AX_UART_HALF_DEFAULT_BAUD 57600 //115200  

#define AX_UART_DIR 27

#define AX_PIO_UART_HALF pio1


void AX_Uart_Half_Duplex_Init(void);
void AX_Uart_Half_Duplex_Loop(void);

void AX_Uart_Half_Duplex_Send(uint8_t Tab[], uint8_t nb_to_send);
bool AX_Uart_Half_Duplex_Send_Done(void);

void AX_Uart_Half_Duplex_Set_Freq(float Wanted_Freq);
uint8_t AX_Uart_Half_Duplex_Set_Freq_Func (void);


extern uint8_t AX_Uart_Half_Receive_Tab[AX_UART_HALF_BUFF_LENGTH];
extern uint8_t AX_Uart_Half_Receive_Ptr;
extern uint32_t AX_Uart_Half_Duplex_Last_Receive_Time;



#endif