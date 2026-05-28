#ifndef __MAIN_H
#define	__MAIN_H


#include <stdio.h>
#include <stdlib.h>
#include "pico/stdio_usb.h"

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
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/bootrom.h"

#include "Interpreteur.h"
#include "user.h"

#include "Feetech/STS_Uart_Half_Duplex.h"
#include "Feetech/STS.h"
#include "Feetech/STS_Actions.h"
#include "Feetech/Registers_Feetech.h"

extern uint32_t Timer_ms1;

extern uint32_t Last_Timer_Led;
extern uint8_t LED_State;

#define LED_PIN 25

#define Timer_us1  (uint32_t)(timer_hw->timelr)

#endif	/* MAIN_H */
