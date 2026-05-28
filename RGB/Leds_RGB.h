#ifndef __LEDS_RGB_H_
#define __LEDS_RGB_H_


// 16 sur petit rubanc (8+8)
// 50 sur le grand (25+25)
#define MAX_STRIP_LENGTH  60

#define NB_RGB_STRIP 2

#define RGB_STRIP_SHORT 0
#define RGB_STRIP_SHORT_PIN 11
#define RGB_STRIP_SHORT_LEN 16
#define Strip_Short GRB_strip[RGB_STRIP_SHORT].data

#define RGB_STRIP_BIG 1
#define RGB_STRIP_BIG_PIN 10
#define RGB_STRIP_BIG_LEN 50
#define Strip_Big GRB_strip[RGB_STRIP_BIG].data

#define RGB_STRIPS_PIN {RGB_STRIP_SHORT_PIN, RGB_STRIP_BIG_PIN}
#define RGB_STRIPS_LEN {RGB_STRIP_SHORT_LEN, RGB_STRIP_BIG_LEN}

void RGB_Init(void);

void RGB_Loop(void);

typedef struct
{
    uint16_t n_leds;
    uint16_t pin;
    uint32_t data[MAX_STRIP_LENGTH]; 
} RGB_Strip;

extern RGB_Strip GRB_strip[NB_RGB_STRIP];


void Set_Led_Color(uint8_t Red, uint8_t Green, uint8_t Blue);

#endif // __LEDS_RGB_H_
