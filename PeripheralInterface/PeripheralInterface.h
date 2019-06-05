#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include "Util/Callback.h"
#include "Util/Mutex.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*!
 * \file PeripheralInterface.h
 * The PeripheralInterface provides a software interface
 * for different peripheral interface drivers (USART, SPI).
 * The following example illustrates writing a string
 * to a peripheral device, the initialization of the
 * Peripheral struct (software representation of the peripheral
 * device you want to interact with) and the PeripheralInterface itself
 * are documented in the header file corresponding to their implementation:
 * ~~~.c
 * uint8_t string[] = "hello, world!";
 * PeripheralInterface_selectPeripheral(interface, peripheral);
 * PeripheralInterface_writeBlocking(interface, string, strlen(string));
 * PeripheralInterface_deselectPeripheral(interface, peripheral);
 * ~~~
 *
 * For now the support for non blocking io is not fully implemented.
 */

typedef void Peripheral;
typedef struct PeripheralInterface PeripheralInterface;
typedef GenericCallback PeripheralInterface_Callback;


void
PeripheralInterface_writeBlocking(PeripheralInterface *self,
                                  const uint8_t *buffer,
                                  size_t size);

void
PeripheralInterface_readBlocking(PeripheralInterface *self,
                                 uint8_t *buffer,
                                 size_t size);

/**
 * Runs the necessary setup steps to allow data transfers
 * to the specified peripheral device. Make sure the
 * specified peripheral matches the implementation used,
 * otherwise behaviour is undefined. Additionally
 * this locks the PeripheralInterface ADT for exclusive
 * access.
 * Throws a PERIPHERALINTERFACE_UNSUPPORTED_PERIPHERAL_SETUP_EXCEPTION
 */
void
PeripheralInterface_selectPeripheral(PeripheralInterface *self,
                                     Peripheral *device);

/**
 * Unlocks the PeripheralInterface ADT and performs clean
 * up steps where necessary, e.g. disabling slave select
 * lines for SPI.
 */
void
PeripheralInterface_deselectPeripheral(PeripheralInterface *self,
                                       Peripheral *device);


enum {
    PERIPHERALINTERFACE_NO_EXCEPTION = 0x00,
    PERIPHERALINTERFACE_UNSUPPORTED_PERIPHERAL_SETUP_EXCEPTION,
    PERIPHERALINTERFACE_UNSUPPORTED_INTERFACE_CONFIG_EXCEPTION,
};

static const uint8_t PERIPHERALINTERFACE_BUSY_EXCEPTION = 3;

/**
 * New implementations can be offered by setting up the struct below
 * and using it as first member of the struct holding the new implementation.
 */
struct PeripheralInterface {
    Mutex mutex;

    void
    (*init)(PeripheralInterface *self);

    void
    (*writeByteBlocking)(PeripheralInterface *self, uint8_t byte);

    uint8_t
    (*readByteBlocking)(PeripheralInterface *self);

    void
    (*readNonBlocking)(PeripheralInterface *self,
                       uint8_t *buffer,
                       uint16_t length);

    void
    (*selectPeripheral)(PeripheralInterface *self, Peripheral *device);

    void
    (*deselectPeripheral)(PeripheralInterface *self, Peripheral *device);

};

#endif /* PERIPHERALINTERFACE_H */
