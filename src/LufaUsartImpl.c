#include "PeripheralInterface/LufaUsartImpl.h"

void lufaUsart_writeByte(const uint8_t *data) {
    putc(*(char *)data, &USBSerialStream);
    fflush(&USBSerialStream);
    lufaUsart_periodicUsbTask();
}

uint8_t lufaUsart_readByteBlocking() {
    uint8_t* data = (uint8_t*)getc(&USBSerialStream);
    lufaUsart_periodicUsbTask();
    return *data;
}

void lufaUsart_periodicUsbTask(void) {
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();
}