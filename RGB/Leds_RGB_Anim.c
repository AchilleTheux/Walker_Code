
#include "main.h"
#define POLICE 1
#define ORANGE 2
#define LICORNE 3
#define RGB_AU 4

uint8_t State_Led_police;
u32 timer_RGB_police;

uint8_t RGB_En = 1;
u8 state_RGB_Strip = 0;

uint32_t timer_RGB = 0;

uint32_t vit_rgb = 1;


void RGB_Anim_Loop(void)
{
    if ((Timer_ms1 - timer_RGB) > 20)
    {
        timer_RGB += 20; // Timer_ms1;
        if (RGB_En)
        {
            // GESTION DU STRIP
            switch (state_RGB_Strip)
            {
            case POLICE:
                Police();
                break;
            case ORANGE:
                RGB_Anim_Continius_Orange();
                break;
            case LICORNE:
                anime_zdc1();
                break;
            case RGB_AU:
                RGB_Anim_AU();
                break;
            default:
                RGB_Anim_OFF();
                break;
            }
        }
        else
        {
            RGB_Anim_OFF();
        }
    }
}

void Police(void)
{
    if ((Timer_ms1 - timer_RGB_police) > 500)
    {
        timer_RGB_police = Timer_ms1;
        if (State_Led_police == 1)
        {
            State_Led_police = 0;
        }
        else
        {
            State_Led_police++;
        }
    }
    if (State_Led_police == 0)
    {

        for (int i = 0; i < NB_RGB_STRIP ; i++)
        {
            for (int k = 0; k < MAX_STRIP_LENGTH; k++)
            {
                GRB_strip[i].data[k] = 0x32;
            }
        }
    }

    if (State_Led_police == 1)
    {
        for (int i = 0; i < (NB_RGB_STRIP ); i++)
        {
            for (int k = 0; k < MAX_STRIP_LENGTH; k++)
            {
                GRB_strip[i].data[k] = 0x3200;
            }
        }
    }
}


void RGB_Anim_Continius_Orange(void)
{
    for (int i = 0; i < (NB_RGB_STRIP ); i++)
    {
        for (int k = 0; k < MAX_STRIP_LENGTH; k++)
        {
            GRB_strip[i].data[k] = 0x002020;
        }
    }
}

void RGB_Anim_AU(void)
{
    static uint32_t last_update = 0;
    static uint8_t blink_state = 0;

    if ((Timer_ms1 - last_update) > 250) {
        last_update = Timer_ms1;
        blink_state = !blink_state;
    }

    uint32_t color = blink_state ? 0x00FF00 : 0x000000;

    for (int i = 0; i < NB_RGB_STRIP; i++) {
        for (int k = 0; k < MAX_STRIP_LENGTH; k++) {
            GRB_strip[i].data[k] = color;
        }
    }
}









static uint32_t Wheel(uint8_t pos) {
    if (pos < 85) {
        // du vert au rouge
        uint8_t green = 255 - pos * 3;
        uint8_t red   = pos * 3;
        return ((uint32_t)green << 16) | ((uint32_t)red << 8) | 0;
    } else if (pos < 170) {
        // du rouge au bleu
        pos -= 85;
        uint8_t red  = 255 - pos * 3;
        uint8_t blue = pos * 3;
        return ((uint32_t)0 << 16) | ((uint32_t)red << 8) | blue;
    } else {
        // du bleu au vert
        pos -= 170;
        uint8_t blue  = 255 - pos * 3;
        uint8_t green = pos * 3;
        return ((uint32_t)green << 16) | ((uint32_t)0 << 8) | blue;
    }
}

// Effet arc‑en‑ciel défilant
void anime_zdc1(void)
{
    static uint32_t last_update = 0;
    static uint8_t  rainbow_pos = 0;

    
    if ((Timer_ms1 - last_update) > 1) {
        last_update = Timer_ms1;
        rainbow_pos++;  // décalage global
    }

    // Remplit chaque LED avec la couleur décalée
    for (int k = 0; k < MAX_STRIP_LENGTH; k++) {
        // on décale la position dans la roue de couleurs
        uint8_t idx = k * (256 / MAX_STRIP_LENGTH) + rainbow_pos;
        for(int j=0; j<NB_RGB_STRIP ;j++)
        {
        GRB_strip[j].data[k] = Wheel(idx);
    }
}
}




void RGB_Anim_OFF(void)
{
    for (int i = 0; i < (NB_RGB_STRIP - 1); i++)
    {
        for (int k = 0; k < MAX_STRIP_LENGTH; k++)
        {
            GRB_strip[i].data[k] = 0x000000; // e04000
        }
    }
}





uint8_t RGB_Func(void)
{
    uint32_t val32;
    if (Get_Param_x32(&val32))
        return PARAM_ERROR_CODE;

    RGB_En = (val32 != 0);

    return 0;
}


uint8_t vitessse_RGB(void) {
    uint32_t val32;
    if (Get_Param_u32(&val32)){
        return PARAM_ERROR_CODE;
    }
    vit_rgb=val32;
    return 0;
}

uint8_t Set_RGB_State_AU(void) {
    state_RGB_Strip = RGB_AU;
    return 0;
}