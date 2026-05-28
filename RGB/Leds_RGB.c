#include "main.h"

#include "ws2812.pio.h"

#include "hardware/regs/dreq.h"

uint8_t WS2812_rgb1_SM = 0;
uint8_t WS2812_rgb1_DMA = 0;


uint32_t RGB_Send_Timer = 0;
uint8_t RGB_Send_State = 0;
uint8_t RGB_strip_num = 0;


RGB_Strip GRB_strip[NB_RGB_STRIP];
uint16_t NB_LEDS[NB_RGB_STRIP] = RGB_STRIPS_LEN;
uint8_t GPIO_PIN_LED[NB_RGB_STRIP] = RGB_STRIPS_PIN;


void RGB_Init(void)
{
     /// INIT STRIP ///
    for (int i = 0; i < NB_RGB_STRIP; i++) {
        GRB_strip[i].pin = GPIO_PIN_LED[i];
        GRB_strip[i].n_leds = NB_LEDS[i];
        for (int k = 0; k < MAX_STRIP_LENGTH; k++){
            GRB_strip[i].data[k] = 0;
        }
        // mise de toutes les pins des rubancs en mode PIO
        pio_gpio_init(pio1, GRB_strip[i].pin);
    }

    uint8_t Dma_Req_from_pio_sm;
    dma_channel_config Dma_Conf;

    // on charge une seule fois le programme, toutes les SM doivent pouvoir tourner sur le meme
	int offset = pio_add_program(pio1, &ws2812_program);

    
    // Prog PIO
	WS2812_rgb1_SM = pio_claim_unused_sm(pio1, true);
    ws2812_program_init_pio(pio1, WS2812_rgb1_SM, offset, 800000, false);

    // Prog DMA
    WS2812_rgb1_DMA = dma_claim_unused_channel(true);
    Dma_Conf = dma_channel_get_default_config(WS2812_rgb1_DMA);
    channel_config_set_transfer_data_size(&Dma_Conf, DMA_SIZE_32);
    channel_config_set_read_increment(&Dma_Conf, true);
    channel_config_set_write_increment(&Dma_Conf, false);
    
    // voir "hardware/regs/dreq.h"
    // mais les TX du pio0, et les data_request DMA c'est la meme chose
    // pour les TX du pio1, data_request DMA = SM + 8 
    Dma_Req_from_pio_sm = WS2812_rgb1_SM + 8;
    channel_config_set_dreq(&Dma_Conf, Dma_Req_from_pio_sm);

    dma_channel_configure(
        WS2812_rgb1_DMA,// Channel to be configured
        &Dma_Conf,                 // The configuration we just created
        &(pio1->txf[WS2812_rgb1_SM]),                // The initial write address
        &GRB_strip[0].data[0],     // The initial read address
        1,        // Number of transfers; in this case each is 1 byte.
        false               // Start immediately.
    );
}

void RGB_Loop(void)
{
    switch (RGB_Send_State) {
        case 0:
            //29us*nb_led + un peu de temps pour faire le changement de pio
            if ((Timer_ms1 - RGB_Send_Timer) > 5) { // max un envoi toutes les 5 ms
                RGB_Send_Timer += 5;
                RGB_Send_State ++;
                RGB_strip_num = 0;
                RGB_Anim_Loop();
            }
            break;
        case 1:
            ws2812_program_set_pin(pio1, WS2812_rgb1_SM, GRB_strip[RGB_strip_num].pin);
            for (uint16_t i = 0; i < GRB_strip[RGB_strip_num].n_leds; i++) {    // shift avant envoi
                GRB_strip[RGB_strip_num].data[i] = GRB_strip[RGB_strip_num].data[i] << 8;
            }
            //dma_channel_start(DMA_Chan_For_WS2812);
            dma_channel_transfer_from_buffer_now(WS2812_rgb1_DMA, &(GRB_strip[RGB_strip_num].data[0]), GRB_strip[RGB_strip_num].n_leds);
            RGB_Send_State ++;

            break;
        case 2:
            // attente que le transfert DMA soit fini :
            if (!dma_channel_is_busy(WS2812_rgb1_DMA) ) {
                for (uint16_t i = 0; i < GRB_strip[RGB_strip_num].n_leds; i++) {    // deshift apres (pour pouvoir jouer plus facilement)
                    GRB_strip[RGB_strip_num].data[i] = GRB_strip[RGB_strip_num].data[i] >> 8;
                }
                RGB_Send_State ++;
                // clear le bit de flag de stall de la State Machine
                pio1->fdebug = (1u << PIO_FDEBUG_TXSTALL_LSB) << WS2812_rgb1_SM;
            }
            break;
        case 3:
            // attente que la State Machine ai bloque sur une lecture de fifo (cad que tout soit bien parti)
            if (pio1->fdebug & (1u << PIO_FDEBUG_TXSTALL_LSB) << WS2812_rgb1_SM) {

                RGB_strip_num ++;
                
                if (RGB_strip_num >= NB_RGB_STRIP){
                    RGB_Send_State = 0;
                } else {
                    RGB_Send_State = 1;
                }
            }
            break;
        default:
            RGB_Send_State = 0;
            break;
    }
}



// special formation 
void Set_Led_Color(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    uint32_t val32 = Green;
    val32 = (val32 << 8) + Red;
    val32 = (val32 << 8) + Blue;

    int i;
    for (i = 0; i < RGB_STRIP_SHORT_LEN; i++) {
        Strip_Short[i] = val32;
    }
    for (i = 0; i < RGB_STRIP_BIG_LEN; i++) {
        Strip_Big[i] = val32;
    }
}


void Debug_RGB(void)
{
    printf("SM : %d\n", (int)WS2812_rgb1_SM);
    printf("DMA %d\n", (int)WS2812_rgb1_DMA);
    printf("Send State %d\n", (int)RGB_Send_State);


}