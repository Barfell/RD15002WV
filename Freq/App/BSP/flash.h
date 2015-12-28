#ifndef __FLASH_H_
#define __FLASH_H_
void MCUFlashUnlock(void);
unsigned char MCUFlashErase(unsigned char sector);
unsigned char MCUFlashWrite(int FlashAddress, int Data);

#endif

