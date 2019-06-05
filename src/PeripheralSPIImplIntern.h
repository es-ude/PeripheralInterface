#ifndef COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H
#define COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H

#include <stdbool.h>
#include "PeripheralInterface/PeripheralSPIImpl.h"
#include "CException.h"
#include "PeripheralInterface/Exception.h"
#include "src/SpiPinNumbers.h"

static void setClockRateDividerBitValues(volatile uint8_t *control_register, uint8_t value);

static void configurePeripheral (SPISlave *device);
static void setClockRateDivider(PeripheralInterfaceSPIImpl *impl, uint8_t rate);
static void setSPIMode(volatile uint8_t *control_register, uint8_t mode);
static void setDataOrder(volatile uint8_t *control_register, uint8_t data_order);
static void enableDoubleSpeed(volatile uint8_t *status_register);
static void disableDoubleSpeed(volatile uint8_t *status_register);

static void becomeSPIMaster(PeripheralInterfaceSPIImpl *impl);
static void tearDownMaster(PeripheralInterfaceSPIImpl *impl);

static void selectPeripheral(PeripheralInterface *self, Peripheral *device);

static void deselectPeripheral(PeripheralInterface *self, Peripheral *device);

static void releaseInterface(PeripheralInterfaceSPIImpl *impl);

static void setInterfaceFunctionPointers(PeripheralInterface *self);

static void waitUntilByteTransmitted(volatile uint8_t *status_register);

static void activateSlaveSelectLine(SPISlave *spi_chip);

static void deactivateSlaveSelectLine(SPISlave *spi_chip);

static void setUpIOLines(const SPIConfig *config);

static void setUpControlRegister(volatile uint8_t *control_register);
static uint8_t transfer(PeripheralInterfaceSPIImpl *self, uint8_t byte);
static void writeByteBlocking(PeripheralInterface *impl, uint8_t byte);
static uint8_t readByteBlocking(PeripheralInterface *impl);

#endif //COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H
