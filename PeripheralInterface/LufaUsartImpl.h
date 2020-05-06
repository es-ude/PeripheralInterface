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

#include <avr/power.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdint.h>
#include "Demos/Device/ClassDriver/VirtualSerial/Descriptors.h"
#include "Demos/Device/ClassDriver/VirtualSerial/Config/LUFAConfig.h"
//instead of #include "LUFAConfig.h"

//added
#include "LUFA/Drivers/USB/Class/Device/CDCClassDevice.h"
#include "LUFA/Drivers/USB/USB.h"

static FILE USBSerialStream;

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
        {
                .Config =
                        {
                                .ControlInterfaceNumber         = 0,
                                .DataINEndpoint                 =
                                        {
                                                .Address                = CDC_TX_EPADDR,
                                                .Size                   = CDC_TXRX_EPSIZE,
                                                .Banks                  = 1,
                                        },
                                .DataOUTEndpoint                =
                                        {
                                                .Address                = CDC_RX_EPADDR,
                                                .Size                   = CDC_TXRX_EPSIZE,
                                                .Banks                  = 1,
                                        },
                                .NotificationEndpoint           =
                                        {
                                                .Address                = CDC_NOTIFICATION_EPADDR,
                                                .Size                   = CDC_NOTIFICATION_EPSIZE,
                                                .Banks                  = 1,
                                        },
                        },
        };

void setUpUsbSerial(void) {
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
    clock_prescale_set(clock_div_1);
    USB_Init();
    wdt_reset();
    sei();
    CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
}

void lufaUsart_writeByte(const uint8_t *data);
uint8_t lufaUsart_readByteBlocking();
void lufaUsart_periodicUsbTask(void);

#endif //PERIPHERALINTERFACE_LUFAUSARTIMPL_H
