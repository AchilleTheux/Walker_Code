#ifndef __MAIN_H
#define	__MAIN_H


#include <stdio.h>
#include <stdlib.h>
#include "pico/stdio_usb.h"
#include "pico/stdio/driver.h"

#include "pico/stdlib.h"

#include <stdint.h>
#include <string.h>
#include <math.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define Abs_Ternaire(a)   (((a)<0)?-(a):(a))
#define Min_Ternaire(a,b) (((a)<(b))?(a):(b))
#define Max_Ternaire(a,b) (((a)>(b))?(a):(b))
#define sizetab(a) sizeof(a)/sizeof(a[0])


#include "hardware/flash.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#include "pico/binary_info.h"
#include "pico/bootrom.h"


#include "hardware/structs/ssi.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/sync.h"


#include "Interpreteur.h"
#include "Test_Funcs.h"
#include "user.h"

#include "Flash/Log_File.h"
#include "Flash/Setup_File.h"
#include "Flash/Flash.h"
#include "Flash/Flash_Mem_Orga.h"
#include "Flash/Hex_Decodeur.h"


#include "RGB/Leds_RGB.h"
#include "RGB/Leds_RGB_Anim.h"
#include "ws2812.pio.h"


#include "AX/AX_Uart_Half_Duplex.h"
#include "AX/AX.h"
#include "AX/AX_Actions.h"
#include "AX/Registers_AX12.h"

#include "Feetech/STS_Uart_Half_Duplex.h"
#include "Feetech/STS.h"
#include "Feetech/STS_Actions.h"
#include "Feetech/Registers_Feetech.h"

#include "AU.h"
#include "action.h"

#include "Actionneurs/pompe.h"
#include "Actionneurs/bras_instances.h"
#include "Actionneurs/bras.h"


#include "Actionneurs/barillet.h"
#include "Actionneurs/retourne.h"
#include "Actionneurs/guide.h"
#include "Actionneurs/bras_curseur.h"

#include "Gestion_Expander_IO.h"
#include "Gestion_I2C.h"

extern uint32_t Timer_ms1;

extern uint32_t Last_Timer_Led;
extern uint8_t LED_State;

#define LED_PIN 25

#define Timer_us1  (uint32_t)(timer_hw->timelr)

#endif	/* MAIN_H */
