#ifndef __HEX_DECODEUR_H__
#define __HEX_DECODEUR_H__



void Hex_Decoder_Flash_Buff_Init(void);
void Hex_Decoder_Flash_Buff_Purge(void);
void Hex_Decoder_Flash_Buff_Add(uint32_t Addr, uint8_t Data);


void HEX_decodeur(char Current_Cmd[]);
uint32_t Hex_To_Decimal(char HEX[], uint8_t len_tab);
uint8_t HEX_Decodeur_Start_Func(void);
uint8_t HEX_Decodeur_Stop_Func(void);


uint8_t HEX_Decodeur_Reprog_Func(void);


#endif