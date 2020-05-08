#include "PeripheralInterface/LufaUsartImpl.h"
#include <avr/power.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>

/**
 * TODO:
 * we include LUFAConfig.h here only temporary,
 * in the long run we should think about wether
 * to add a copy with the configuration we need
 * into this repo, or let the user define a new
 * one on application level. Maybe combine both
 * approaches by creating different builds
 */

/**
 * USB.h has to be included before Descriptors.h
 * otherwise the include directive from Descriptors.h
 * will be execetuted which in turn uses a wrong path.
 */
#include "LUFA/Drivers/USB/USB.h"
#include "Demos/Device/ClassDriver/VirtualSerial/Descriptors.h"
//added
#include "LUFA/Drivers/USB/Class/Device/CDCClassDevice.h"

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