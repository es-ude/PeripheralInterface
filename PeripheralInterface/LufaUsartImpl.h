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


void lufaUsart_writeByte(const uint8_t *data);
uint8_t lufaUsart_readByteBlocking();
void lufaUsart_periodicUsbTask(void);

#endif //PERIPHERALINTERFACE_LUFAUSARTIMPL_H
