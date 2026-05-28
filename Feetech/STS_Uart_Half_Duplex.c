#include "main.h"


#include "STS_pepino_uart_tx.pio.h"
#include "STS_uart_rx.pio.h"



PIO STS_Pio_Uart_Half_Duplex = STS_PIO_UART_HALF;

static int sm_rx;
static int sm_tx;

static uint8_t Uart_Half_Transmit_Tab[STS_UART_HALF_BUFF_LENGTH];
static uint8_t Transmit_Goal = 0, Transmit_Ptr = 0; 
uint8_t STS_Uart_Half_Receive_Tab[STS_UART_HALF_BUFF_LENGTH];
static uint8_t Receive_Drop_Count;
uint8_t STS_Uart_Half_Receive_Ptr = 0;
uint32_t STS_Uart_Half_Duplex_Last_Receive_Time = 0;


void STS_Uart_Half_Duplex_Init(void) {

    // idealement, mieux gerer l'utilisation des SM du pio...
    sm_rx = pio_claim_unused_sm(STS_Pio_Uart_Half_Duplex, false);
    sm_tx = pio_claim_unused_sm(STS_Pio_Uart_Half_Duplex, false);

    int offset_rx = pio_add_program(STS_Pio_Uart_Half_Duplex, &uart_rx_program);
    int offset_tx = pio_add_program(STS_Pio_Uart_Half_Duplex, &uart_tx_program);


    // SM transmits 1 bit per 8 execution cycles.
    float div = (float)clock_get_hz(clk_sys) / (8 * STS_UART_HALF_DEFAULT_BAUD);

    pio_sm_set_consecutive_pindirs(STS_Pio_Uart_Half_Duplex, sm_rx, STS_UART_HALF_PIN, 1, false);
    pio_sm_set_consecutive_pindirs(STS_Pio_Uart_Half_Duplex, sm_rx, STS_UART_DIR, 1, true);
    pio_gpio_init(STS_Pio_Uart_Half_Duplex, STS_UART_HALF_PIN);
    pio_gpio_init(STS_Pio_Uart_Half_Duplex, STS_UART_DIR);
    gpio_pull_up(STS_UART_HALF_PIN);

    // pio_sm_set_pins_with_mask(pio0, sm_tx, 1u << PIO_FEETECH_PIN, 1u << PIO_FEETECH_PIN);
    // pio_sm_set_pindirs_with_mask(pio0, sm_tx, 1u << PIO_FEETECH_PIN, 1u << PIO_FEETECH_PIN);
    // pio_gpio_init(pio0, PIO_FEETECH_PIN);


    pio_sm_config c_rx = uart_rx_program_get_default_config(offset_rx);
    pio_sm_config c_tx = uart_tx_program_get_default_config(offset_tx);


    sm_config_set_in_pins(&c_rx, STS_UART_HALF_PIN); // for WAIT, IN
    sm_config_set_jmp_pin(&c_rx, STS_UART_HALF_PIN); // for JMP


    // OUT/SET drive the half-duplex data pin, side-set drives the level-shifter
    // direction pin.

    sm_config_set_out_pins(&c_tx, STS_UART_HALF_PIN, 1);
    sm_config_set_set_pins(&c_tx, STS_UART_HALF_PIN, 1);
    sm_config_set_sideset_pins(&c_tx, STS_UART_DIR);


    // Shift to right, autopush disabled
    sm_config_set_in_shift(&c_rx, true, false, 32);
    // OUT shifts to right, no autopull
    sm_config_set_out_shift(&c_tx, true, false, 32);

    // Deeper FIFO as we're not doing any change in both
    sm_config_set_fifo_join(&c_rx, PIO_FIFO_JOIN_RX);
    sm_config_set_fifo_join(&c_tx, PIO_FIFO_JOIN_TX);


    sm_config_set_clkdiv(&c_rx, div);
    sm_config_set_clkdiv(&c_tx, div);

    pio_sm_init(STS_Pio_Uart_Half_Duplex, sm_rx, offset_rx, &c_rx);
    pio_sm_init(STS_Pio_Uart_Half_Duplex, sm_tx, offset_tx, &c_tx);

    pio_sm_set_enabled(STS_Pio_Uart_Half_Duplex, sm_rx, true);
    pio_sm_set_enabled(STS_Pio_Uart_Half_Duplex, sm_tx, true);

}

void STS_Uart_Half_Duplex_Loop(void) {
    if (Transmit_Goal != Transmit_Ptr) {
        //if (!pio_sm_is_tx_fifo_full(STS_Pio_Uart_Half_Duplex, sm_tx)) {
        if (pio_sm_is_tx_fifo_empty(STS_Pio_Uart_Half_Duplex, sm_tx)) {
            //printf("Send %d\n", Uart_Half_Transmit_Tab[Transmit_Ptr]);
            pio_sm_put(STS_Pio_Uart_Half_Duplex, sm_tx, Uart_Half_Transmit_Tab[Transmit_Ptr]);
            Transmit_Ptr ++;
        }
    }
    while(!pio_sm_is_rx_fifo_empty(STS_Pio_Uart_Half_Duplex, sm_rx)) {
        uint32_t Data = (pio_sm_get(STS_Pio_Uart_Half_Duplex, sm_rx)) >> 24;
        if (Receive_Drop_Count) {
            Receive_Drop_Count --;
        } else if (STS_Uart_Half_Receive_Ptr < STS_UART_HALF_BUFF_LENGTH) {
            STS_Uart_Half_Receive_Tab[STS_Uart_Half_Receive_Ptr] = Data;
            STS_Uart_Half_Receive_Ptr ++;
        }
        STS_Uart_Half_Duplex_Last_Receive_Time = Timer_ms1;
    }
}

void STS_Uart_Half_Duplex_Send(uint8_t Tab[], uint8_t nb_to_send) {
    if (nb_to_send < STS_UART_HALF_BUFF_LENGTH) {
        int i;
        for (i = 0; i < nb_to_send; i++) {
            Uart_Half_Transmit_Tab[i] = Tab[i];
        }
        for (i = 0; i < STS_UART_HALF_BUFF_LENGTH; i++) {
            STS_Uart_Half_Receive_Tab[i] = 0;
        }
        Transmit_Goal = nb_to_send;
        Transmit_Ptr = 0;
        STS_Uart_Half_Receive_Ptr = 0;
        Receive_Drop_Count = nb_to_send;
    }
    pio_sm_clear_fifos(STS_Pio_Uart_Half_Duplex, sm_rx);
}


bool STS_Uart_Half_Duplex_Send_Done(void) {
    return (Transmit_Goal == Transmit_Ptr);
}

void STS_Uart_Half_Duplex_Set_Freq(float Wanted_Freq) {

    // SM transmits 1 bit per 8 execution cycles.
    float div = (float)clock_get_hz(clk_sys) / (8 * Wanted_Freq);

    pio_sm_set_enabled(STS_Pio_Uart_Half_Duplex, sm_rx, false);
    pio_sm_set_enabled(STS_Pio_Uart_Half_Duplex, sm_tx, false);

    pio_sm_set_clkdiv(STS_Pio_Uart_Half_Duplex, sm_rx, div);
    pio_sm_set_clkdiv(STS_Pio_Uart_Half_Duplex, sm_tx, div);

    pio_sm_set_enabled(STS_Pio_Uart_Half_Duplex, sm_rx, true);
    pio_sm_set_enabled(STS_Pio_Uart_Half_Duplex, sm_tx, true);

}

uint8_t STS_Uart_Half_Duplex_Set_Freq_Func (void)
{
    float Wanted_Freq;
    if (Get_Param_Float(&Wanted_Freq))
        return PARAM_ERROR_CODE;

    STS_Uart_Half_Duplex_Set_Freq(Wanted_Freq);

    return 0;
}
