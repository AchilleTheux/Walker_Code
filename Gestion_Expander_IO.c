#include "main.h"

ExpIO ExpIO_Power = {.I2C_Addr = EXPANDER_IO_POWER_ADDR, .Conf_Reg = 0xFFFF, .Out_Reg = 0} ;


// uint8_t ExpIO_En_5V;
// uint8_t ExpIO_AU_Btn;

uint8_t ExpIO_Pompe1 = 0;
uint8_t ExpIO_Pompe2 = 0;
uint8_t ExpIO_Vanne1 = 0;
uint8_t ExpIO_Vanne2 = 0;
uint8_t ExpIO_vbat_detect = 0;
uint8_t temp_ExpIo_vbat_detect = 0; 

//uint8_t ExpIO_Laisse;



void Expander_IO_Init(void){
    //on init tout a 1 surtout pour les switchs qui doivent avoir 1 pour etre lisible
    //uint8_t IO_State_init[2] = {0b11111111,0b11111111};
    //Add_I2C_command(EXPANDER2_IO_ADDR, 2, 0 , &IO_State[0], NULL, &Expander_IO_Done);

    ExpIO_Power.Conf_Reg = ~ (EXPIO_POMPE1 + EXPIO_VANNE1 + EXPIO_LED1); 

    ExpIO_Power.Loop_State = 0;
    ExpIO_Power.I2C_Done = 1;
    ExpIO_Power.Last_Timer = 0;

}

// wr out + conf puis Read

void Gestion_Expandeur_Loop(void)
{
    if (ExpIO_Power.I2C_Done) {
        switch (ExpIO_Power.Loop_State) {
            case 0:
                ExpIO_Power.Conf_Reg = ~ ( EXPIO_POMPE1 + EXPIO_VANNE1 + EXPIO_POMPE2 + EXPIO_VANNE2 + EXPIO_LED1);  
                                            
                ExpIO_Power.Out_Reg = 0;

               
                if (ExpIO_Pompe1) ExpIO_Power.Out_Reg |= EXPIO_POMPE1;
                if (ExpIO_Pompe2) ExpIO_Power.Out_Reg |= EXPIO_POMPE2;

                if (ExpIO_Vanne1) ExpIO_Power.Out_Reg |= EXPIO_VANNE1;
                if (ExpIO_Vanne2) ExpIO_Power.Out_Reg |= EXPIO_VANNE2;

                //if (Timer_ms1/1000 % 2) ExpIO_Power.Out_Reg |= EXPIO_LED1; // clignote la LED1 pour montrer que le code tourne
                

                ExpIO_Power.I2C_Buff[0] = EXPIO_OUT_REG;
                ExpIO_Power.I2C_Buff[1] = ExpIO_Power.Out_Reg;
                ExpIO_Power.I2C_Buff[2] = ExpIO_Power.Out_Reg>>8;
                ExpIO_Power.I2C_Done = 0;
                Add_I2C_command(ExpIO_Power.I2C_Addr, 3, 0 , &(ExpIO_Power.I2C_Buff[0]), NULL, &(ExpIO_Power.I2C_Done));
                ExpIO_Power.Loop_State ++;
                break;
            case 1:
                ExpIO_Power.I2C_Buff[0] = EXPIO_CONF_REG;
                ExpIO_Power.I2C_Buff[1] = ExpIO_Power.Conf_Reg;
                ExpIO_Power.I2C_Buff[2] = ExpIO_Power.Conf_Reg>>8;
                ExpIO_Power.I2C_Done = 0;
                Add_I2C_command(ExpIO_Power.I2C_Addr, 3, 0 , &(ExpIO_Power.I2C_Buff[0]), NULL, &(ExpIO_Power.I2C_Done));
                ExpIO_Power.Loop_State ++;
                break;
            case 2:
                ExpIO_Power.I2C_Buff[0] = EXPIO_IN_REG;
                ExpIO_Power.I2C_Done = 0;
                Add_I2C_command(ExpIO_Power.I2C_Addr, 1, 2 , &(ExpIO_Power.I2C_Buff[0]), &(ExpIO_Power.I2C_Buff[1]), &(ExpIO_Power.I2C_Done));
                ExpIO_Power.Loop_State ++;
                break;
            case 3:
                ExpIO_Power.In_Reg = (((uint16_t)(ExpIO_Power.I2C_Buff[2])) << 8) + ExpIO_Power.I2C_Buff[1];

                //ExpIO_AU_Btn = ((ExpIO_Power.In_Reg & EXPIO_AU_BTN) != 0);
                //ExpIO_En_5V = ((ExpIO_Power.In_Reg & EXPIO_EN_5V) != 0);
                
                // ExpIO_Laisse = ((ExpIO_Power.In_Reg & EXPIO_LAISSE) != 0);
                // ExpIO_Team = ((ExpIO_Power.In_Reg & EXPIO_TEAM_BTN) != 0);
                temp_ExpIo_vbat_detect = ExpIO_vbat_detect;

                ExpIO_vbat_detect = ((ExpIO_Power.In_Reg & EXPIO_VBAT_DETECT) != 0);
                if (!ExpIO_vbat_detect && temp_ExpIo_vbat_detect) {
                    printf("VBAT DETECTED\n");
                }


                ExpIO_Power.Loop_State ++; 
                ExpIO_Power.Last_Timer = Timer_ms1;
                //Switch_Decode_Loop();

                break;
            case 4:
                if ((Timer_ms1 - ExpIO_Power.Last_Timer) > 5) {
                    ExpIO_Power.Last_Timer += 5;
                    ExpIO_Power.Loop_State = 0;
                }
                break;
            default:
                ExpIO_Power.Loop_State = 0;
                break;
        }
    }
}


// cette fonction est appellee une fois toutes les 2 ms
// une fois que l'expandeur d'IO a mis a jour les variables correspondant au switchs de la carte power

//void Switch_Decode_Loop(void)
//{
/*temps de charge = 10ms*/

// static int nb_appel = 0;
// uint32_t temps_decharge;
// int pin1_sw_i = ExpIO_SW_i & 1;
// int pin2_sw_i = ExpIO_SW_i & 2;
// static int nb_appel_pin1;
// static int nb_appel_pin2;
// if(nb_appel <=5)
// {
//     ExpIO_SW_o = 0b11; /*écriture binaire de 3*/
//     nb_appel +=1;   
//     nb_appel_pin1 = 0;
//     nb_appel_pin2 = 0;
// }
// else if(nb_appel <=100)
// {
//     ExpIO_SW_o = 0b00;
//     nb_appel ++;
//     if(pin1_sw_i == 0){
//         if(nb_appel_pin1 == 0)
//         {
//             nb_appel_pin1 = nb_appel;
//             //printf("Nombre d'appels pin 1 %d \n", nb_appel_pin1);
//         }
//     }
//     if(pin2_sw_i == 0){
//         if(nb_appel_pin2 == 0)
//         {
//             nb_appel_pin2 = nb_appel;
//             //printf("Nombre d'appels pin 2 %d \n", nb_appel_pin2);    
//         }   
//     }
// }else{

//     nb_appel = 0;   
//     val_SW = 0;

//     if (nb_appel_pin2 > 17 && nb_appel_pin2 < 25){ //Pin droit de la partie droite
//         val_SW = val_SW + 1;  
//     }
//     else if (nb_appel_pin2 < 18 && nb_appel_pin2 > 13){ //Pin gauche de la partie droite
//         val_SW = val_SW + 2;
//     }
//     else if (nb_appel_pin2 < 14 && nb_appel_pin2 > 5){ //Pin droit et gauche de la partie droite
//         val_SW = val_SW + 3;
//     }
//     if (nb_appel_pin1 > 17 && nb_appel_pin1 < 25){ //Pin droit de la partie gauche
//         val_SW = val_SW + 4;
//     }
//     else if (nb_appel_pin1 < 18 && nb_appel_pin1 > 13){ //Pin gauche de la partie gauche
//         val_SW = val_SW + 8;
//     }
//     else if (nb_appel_pin1 < 14 && nb_appel_pin1 > 5){ //Pin droit et gauche de la partie gauche
//         val_SW = val_SW + 12;
//     }
//     // printf("Valeur SW : %d \n", val_SW);
//     //printf("Nombre d'appels pin 1 %d \n", nb_appel_pin1);
//     //printf("Nombre d'appels pin 2 %d \n", nb_appel_pin2);
// }
    /*if(ExpIO_SW_i == 0) mesure passage 2 pins à 0
    {
        printf("Nombre d'appels %d \n",nb_appel);
        nb_appel = 0;
    }*/

/* quand décharge finie, on prend fin decharge et print la diff, lecture pin avec sw i*/
/*objectif mesure de temps de décharge de la capa liée aux pins

si capa déchargée,   sw_o = 1 implique pin en sortie et sw_o = 0 pin entrée, pin en sortie pour charger, entrée pour décharger
dès que sw_o passe à 1 on mesure le temps de décharge (timer_dc), on l'arrête quand elle repasse à 0*/

//}

