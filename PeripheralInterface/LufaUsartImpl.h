#ifndef PERIPHERALINTERFACE_LUFAUSARTIMPL_H
#define PERIPHERALINTERFACE_LUFAUSARTIMPL_H

/*
 * We use the LUFA library to send debugging messages
 * to a virtual serial port over usb (CDC-ACM).
 * This is taken from the VirtualSerial demo
 * that ships with the LUFA library.
 * You can find it under
 * LUFA/Demos/Device/ClassDriver/VirtualSerial/VirtualSerial.c
 */



#include <stdint.h>

void setUpUsbSerial(void);
void initLufa(void);
uint8_t lufaOutputAvailable(void);
void lufaWriteByte(uint8_t c);
void lufaWriteStringLength(char *s, uint16_t length);
void lufaWriteString(char *s);
void lufaWaitUntilDone(void);
uint16_t lufaNumInputAvailable(void);
uint8_t lufaReadAvailable(void);
uint8_t lufaGetChar();
int16_t lufaReadByte(void);
uint16_t lufaReadByteBlocking(void);
void lufaTask(void);

#endif //PERIPHERALINTERFACE_LUFAUSARTIMPL_H
