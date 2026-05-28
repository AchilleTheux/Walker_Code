#ifndef __USER_H_
#define __USER_H_

// initialize all things
void Init_All(void);

uint8_t Reboot_Func(void);
uint8_t Reboot_USB_Func(void);
uint8_t Print_Version_Func(void);

uint32_t time_ms_32(void);

#endif // _USER_H_
