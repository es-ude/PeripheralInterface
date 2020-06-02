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
#include <LUFA/Drivers/Peripheral/Serial.h>
#include <LUFA/Drivers/Misc/RingBuffer.h>
#include <LUFA/Drivers/USB/USB.h>

/** Circular buffer to hold data from the host before it is sent to the device via the serial port. */
static RingBuffer_t inputRingBuffer;

/** Underlying data buffer for \ref USBtoUSART_Buffer, where the stored bytes are located. */
static uint8_t      inputBuffer[512];

/** Circular buffer to hold data from the serial port before it is sent to the host. */
static RingBuffer_t outputRingBuffer;

/** Underlying data buffer for \ref USARTtoUSB_Buffer, where the stored bytes are located. */
static uint8_t      outputBuffer[512];

static FILE USBSerialStream;
/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
        {
                .Config =
                        {
                                .ControlInterfaceNumber         = INTERFACE_ID_CDC_CCI,
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
void initLufa(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    USB_Init();

    RingBuffer_InitBuffer(&inputRingBuffer, inputBuffer, sizeof(inputBuffer));
    RingBuffer_InitBuffer(&outputRingBuffer, outputBuffer, sizeof(outputBuffer));
    wdt_reset();
    sei();
}
uint8_t lufaAvailable(void)
{
    return (USB_DeviceState == DEVICE_STATE_Configured);
}

uint8_t lufaOutputAvailable(void)
{
    return (lufaAvailable() && !(RingBuffer_IsFull(&outputRingBuffer)));
}


void lufaWriteByte(uint8_t c)
{
    if (lufaOutputAvailable())
        RingBuffer_Insert(&outputRingBuffer, c);
//    if (!(RingBuffer_IsFull(&inputRingBuffer)))
//    {
//        int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
//
//        /* Store received byte into the USART transmit buffer */
//        if (!(ReceivedByte < 0))
//            RingBuffer_Insert(&inputRingBuffer, ReceivedByte);
//    }
}

void lufaWriteStringLength(char *s, uint16_t length)
{
    char *ptr = s;
    for(uint16_t i = 0; i < length; i++)
    {
        lufaWriteByte(*ptr++);
    }
}

void lufaWriteString(char *s)
{
    char *ptr = s;
    while(*ptr)
    {
        lufaWriteByte(*ptr++);
    }
}
void lufaWaitUntilDone(void)
{
    while(!RingBuffer_IsEmpty(&outputRingBuffer))
    {
        // keep doing usb until done
        lufaTask();
    }
}

uint16_t lufaNumInputAvailable(void)
{
    return RingBuffer_GetCount(&inputRingBuffer);
}

uint8_t lufaReadAvailable(void)
{
    return !RingBuffer_IsEmpty(&inputRingBuffer);
}

uint8_t lufaGetChar()
{
    return RingBuffer_Remove(&inputRingBuffer);
}

int16_t lufaReadByte(void)
{
    if(!RingBuffer_IsEmpty(&inputRingBuffer))
    {
        return RingBuffer_Remove(&inputRingBuffer);
    }
    return -1;
}

uint16_t lufaReadByteBlocking(void)
{
    while(RingBuffer_IsEmpty(&inputRingBuffer))
    {
        lufaTask();
    }
    return lufaGetChar();
}

void lufaTask(void)
{
    /* Only try to read in bytes from the CDC interface if the transmit buffer is not full */
    if (!(RingBuffer_IsFull(&inputRingBuffer)))
    {
        int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

        /* Store received byte into the USART transmit buffer */
        if (!(ReceivedByte < 0)) {
            RingBuffer_Insert(&inputRingBuffer, ReceivedByte);
        }
    }

    uint16_t BufferCount = RingBuffer_GetCount(&outputRingBuffer);
    if (BufferCount)
    {
        Endpoint_SelectEndpoint(VirtualSerial_CDC_Interface.Config.DataINEndpoint.Address);

        /* Check if a packet is already enqueued to the host - if so, we shouldn't try to send more data
         * until it completes as there is a chance nothing is listening and a lengthy timeout could occur */
        if (Endpoint_IsINReady())
        {
            /* Never send more than one bank size less one byte to the host at a time, so that we don't block
             * while a Zero Length Packet (ZLP) to terminate the transfer is sent if the host isn't listening */
            uint8_t BytesToSend = MIN(BufferCount, (CDC_TXRX_EPSIZE - 1));

            /* Read bytes from the USART receive buffer into the USB IN endpoint */
            while (BytesToSend--)
            {
                /* Try to send the next byte of data to the host, abort if there is an error without dequeuing */
                if (CDC_Device_SendByte(&VirtualSerial_CDC_Interface,
                                        RingBuffer_Peek(&outputRingBuffer)) != ENDPOINT_READYWAIT_NoError)
                {
                    break;
                }

                /* Dequeue the already sent byte from the buffer now we have confirmed that no transmission error occurred */
                RingBuffer_Remove(&outputRingBuffer);
            }
        }
    }

    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
//    LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
//    LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
    bool ConfigSuccess = true;

    ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

//    LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
    CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}


/** Event handler for the CDC Class driver Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
    uint8_t ConfigMask = 0;

    switch (CDCInterfaceInfo->State.LineEncoding.ParityType)
    {
        case CDC_PARITY_Odd:
            ConfigMask = ((1 << UPM11) | (1 << UPM10));
            break;
        case CDC_PARITY_Even:
            ConfigMask = (1 << UPM11);
            break;
    }

    if (CDCInterfaceInfo->State.LineEncoding.CharFormat == CDC_LINEENCODING_TwoStopBits)
        ConfigMask |= (1 << USBS1);

    switch (CDCInterfaceInfo->State.LineEncoding.DataBits)
    {
        case 6:
            ConfigMask |= (1 << UCSZ10);
            break;
        case 7:
            ConfigMask |= (1 << UCSZ11);
            break;
        case 8:
            ConfigMask |= ((1 << UCSZ11) | (1 << UCSZ10));
            break;
    }
}