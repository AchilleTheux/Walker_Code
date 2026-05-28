
#include "main.h"


uint32_t Timer_Test = 0;
uint32_t Param_Test = 0;


void Test_Loop(void)
{
    if ((Timer_ms1 - Timer_Test) > 10)  {
        Timer_Test += 10;
        // todo something if needed
    }
}

void Debug_RGB(void);

u8 Test_Func (void)
{
    /*u32 val32;
    if (Get_Param_u32(&val32))
        return PARAM_ERROR_CODE;

    if (val32 > 1000)
        return PARAM_OUT_OF_RANGE_ERROR_CODE;

    Param_Test = val32;
    */
    //printf("Param_Test is %lu\r\n", Param_Test);
    
    Debug_RGB();

    return 0;
}


u8  Ping_Func (void)
{
    printf("Pong\r\n");
    return 0;
}

uint8_t Timer_Func(void)
{
    printf("%lu\r\n", Timer_ms1);
    return 0;
}


uint8_t Debug_En = 0;

uint8_t Debug_En_Func(void)
{
    u32 val32;
    if (Get_Param_u32(&val32)) {    // si y a param, on le prend, sinon on inverse l'etat actuel
        val32 = !Debug_En;
    }
    
    Debug_En = (val32 != 0);
    
    printf("Debug_En is %d\r\n", Debug_En);
    

    return 0;
}


