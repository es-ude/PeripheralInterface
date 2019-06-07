#include "unity.h"

#include "PeripheralInterface/PeripheralSPIImpl.h"
#include "src/SpiPinNumbers.h"
#include "PeripheralInterface/Exception.h"
#include "Util/MockMutex.h"

#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_MASTER_REGISTERS 5
#define NUMBER_OF_SLAVE_REGISTERS 2

static uint8_t registers_master[NUMBER_OF_MASTER_REGISTERS];
static uint8_t registers_slave[NUMBER_OF_SLAVE_REGISTERS];
static SPIConfig config;
static SPISlave peripheral = {
  .slave_select_pin        = 1,
  .data_register           = registers_slave,
  .data_direction_register = registers_slave + 1,
  .spi_mode                = SPI_MODE_0,
  .idle_signal             = SPI_IDLE_SIGNAL_HIGH,
  .clock_rate_divider      = SPI_CLOCK_RATE_DIVIDER_64,
  .data_order              = SPI_DATA_ORDER_LSB_FIRST,
};
static PeripheralInterface *interface = NULL;

static bool callback_called = false;

void
setUp(void)
{
  uint8_t *registers_local_copy           = registers_master;
  config.control_register                 = registers_local_copy++;
  config.data_register                    = registers_local_copy++;
  config.status_register                  = registers_local_copy++;
  config.io_lines_data_direction_register = registers_local_copy++;
  config.io_lines_data_register           = registers_local_copy;
  config.clock_pin                        = 0;
  config.miso_pin                         = 1;
  config.mosi_pin                         = 2;
  memset(registers_master, 0, NUMBER_OF_MASTER_REGISTERS);
  interface = malloc(PeripheralInterfaceSPI_getADTSize());
  initMutex_Expect(&interface->mutex);
  PeripheralInterfaceSPI_createNew(interface, &config);
  callback_called = false;
}

void
tearDown(void)
{
  free(interface);
}

void
test_selectSetsSPIEnableBit(void)
{
  CEXCEPTION_T e;
  Try { interface->selectPeripheral(interface, &peripheral); }
  Catch(e) { TEST_FAIL(); }
  TEST_ASSERT_BIT_HIGH(spi_enable_bit, *config.control_register);
}

void
test_deselectSetsControlRegisterToDefault(void)
{
  CEXCEPTION_T e;
  Try
  {
    interface->selectPeripheral(interface, &peripheral);
    interface->deselectPeripheral(interface, &peripheral);
  }
  Catch(e) { TEST_FAIL(); }

  TEST_ASSERT_BITS_LOW(0xFF, *config.control_register);
}

void
test_failedSetupEndsWithDefaultControlRegisterValues(void)
{
  SPISlave invalid_peripheral = peripheral;
  invalid_peripheral.spi_mode = 30;
  CEXCEPTION_T e;
  Try { interface->selectPeripheral(interface, &invalid_peripheral); }
  Catch(e) { TEST_ASSERT_EQUAL_UINT8(PERIPHERAL_SELECT_EXCEPTION, e); }
  TEST_ASSERT_BITS_LOW(0xFF, *config.control_register);
}

void
test_selectConfiguresInterfaceAsMaster(void)
{
  interface->selectPeripheral(interface, &peripheral);

  TEST_ASSERT_BIT_HIGH(master_slave_select_bit, *config.control_register);
}