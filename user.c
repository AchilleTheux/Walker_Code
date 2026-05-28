
#include "main.h"

void Init_All(void) {
    // initialize all things    
	stdio_init_all();

    // Feetech
    STS_Uart_Half_Duplex_Init();
    Init_Com_STS();

    // "purge" du buffer USB RX ?
	getchar_timeout_us(0);

}


uint8_t Reboot_Func(void) {
    //watchdog_hw->ctrl = WATCHDOG_CTRL_TRIGGER_BITS;
    watchdog_reboot(0,0,1);
    return 0;
}

uint8_t Reboot_USB_Func(void) {
    reset_usb_boot(0,0);
    return 0;
}

uint8_t Print_Version_Func(void) {
    printf("version,walker\n");
    printf("Compiled_on,");
    printf(__DATE__);
    printf(" @ ");
    printf(__TIME__);
    printf("\n");
    return 0;
}


// must be reworked !
uint32_t time_ms_32(void) {
    
    // // Read the lower 32 bits
    // uint64_t lo = timer_hw->timelr;
    // // Now read the upper 32 bits 
    // uint64_t hi = timer_hw->timehr;

    // uint64_t loc_time_us = (hi << 32) + lo;
    
    // loc_time_us = loc_time_us / 1000;

    return ((time_us_64())/1000);
    //return loc_time_us;
}
