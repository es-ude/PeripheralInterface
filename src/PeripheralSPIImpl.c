#include "src/PeripheralSPIImplIntern.h"
#include "Util/BitManipulation.h"


size_t
PeripheralInterfaceSPI_getADTSize(void)
{
  return sizeof(struct PeripheralInterfaceSPIImpl);
}

void
PeripheralInterfaceSPI_createNew(PeripheralInterface *memory,
                                 const SPIConfig *const spiConfig)
{
  PeripheralInterfaceSPIImpl *impl = (PeripheralInterfaceSPIImpl *) memory;
  impl->config = *spiConfig;
  initMutex(&memory->mutex);
  setInterfaceFunctionPointers(&impl->interface);
}

void
setInterfaceFunctionPointers(PeripheralInterface *self)
{
  self->selectPeripheral = selectPeripheral;
  self->deselectPeripheral = deselectPeripheral;

  self->writeByteBlocking = writeByteBlocking;
  self->readByteBlocking = readByteBlocking;
}


void
writeByteBlocking(
        PeripheralInterface *self,
        const uint8_t byte
)
{
  PeripheralInterfaceSPIImpl *impl = (PeripheralInterfaceSPIImpl *) self;
  transfer(impl, byte);
}

uint8_t
readByteBlocking(
        PeripheralInterface *self
)
{
  PeripheralInterfaceSPIImpl *impl = (PeripheralInterfaceSPIImpl *) self;
  return transfer(impl, 0);
}

static void
setUpControlRegister(volatile uint8_t *control_register)
{
  BitManipulation_setBit(control_register, spi_enable_bit);
  BitManipulation_setBit(control_register, master_slave_select_bit);
}

static void
setUpIOLines(const SPIConfig *config)
{
  *config->io_lines_data_direction_register |= (1 << config->slave_select_pin | 1 << config->mosi_pin |
                                                1 << config->clock_pin);
  *config->io_lines_data_direction_register &= ~(1 << config->miso_pin);
  *config->io_lines_data_register |= (1 << config->slave_select_pin);
}


static void
configurePeripheral(SPISlave *device)
{
  *device->data_direction_register |= (1 << device->slave_select_pin);
  deactivateSlaveSelectLine(device);
}


void
selectPeripheral(PeripheralInterface *self,
                 Peripheral *device)
{
  PeripheralInterfaceSPIImpl *impl = (PeripheralInterfaceSPIImpl *) self;
  SPISlave *spi_chip = (SPISlave *) device;
  volatile uint8_t *control_register = impl->config.control_register;
  CEXCEPTION_T e = CEXCEPTION_NONE;
  Try
      {
        configurePeripheral(spi_chip);
        becomeSPIMaster(impl);
        setClockRateDivider(impl, spi_chip->clock_rate_divider);
        setSPIMode(control_register, spi_chip->spi_mode);
        setDataOrder(control_register, spi_chip->data_order);
        activateSlaveSelectLine(spi_chip);
      }
      Catch(e)
  {
    tearDownMaster(impl);
    deactivateSlaveSelectLine(spi_chip);
  }
}

static void
becomeSPIMaster(PeripheralInterfaceSPIImpl *self)
{
  // Important: setup of the io lines has to happen before anything else
  setUpIOLines(&self->config);
  setUpControlRegister(self->config.control_register);
}

void
activateSlaveSelectLine(SPISlave *spi_chip)
{
  if (spi_chip->idle_signal == SPI_IDLE_SIGNAL_LOW)
  {
    *spi_chip->data_register |= (1 << spi_chip->slave_select_pin);
  } else if (spi_chip->idle_signal == SPI_IDLE_SIGNAL_HIGH)
  {
    BitManipulation_clearBit(spi_chip->data_register, spi_chip->slave_select_pin);
  } else
  {
    Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
}

static void
deactivateSlaveSelectLine(SPISlave *spi_chip)
{
  if (spi_chip->idle_signal == SPI_IDLE_SIGNAL_LOW)
  {
    BitManipulation_clearBit(spi_chip->data_register, spi_chip->slave_select_pin);
  } else if (spi_chip->idle_signal == SPI_IDLE_SIGNAL_HIGH)
  {
    BitManipulation_setBit(spi_chip->data_register, spi_chip->slave_select_pin);
  } else
  {
    Throw(PERIPHERAL_DESELECT_EXCEPTION);
  }
}

uint8_t
transfer(PeripheralInterfaceSPIImpl *self,
         uint8_t data)
{
  *self->config.data_register = data;
  waitUntilByteTransmitted(self->config.status_register);
  return *self->config.data_register;
}

void
waitUntilByteTransmitted(volatile uint8_t *status_register)
{
  while (!(*status_register & (1 << spi_interrupt_flag_bit))) {}
}

void
releaseInterface(PeripheralInterfaceSPIImpl *impl)
{
}

static void
deselectPeripheral(PeripheralInterface *self,
                   Peripheral *device)
{
  PeripheralInterfaceSPIImpl *impl = (PeripheralInterfaceSPIImpl *) self;

  deactivateSlaveSelectLine((SPISlave *) device);
  releaseInterface(impl);
  tearDownMaster(impl);
}

static void
tearDownMaster(PeripheralInterfaceSPIImpl *self)
{
  *self->config.control_register = 0;
}

static void
setClockRateDivider(PeripheralInterfaceSPIImpl *impl,
                    uint8_t rate_divider)
{
  volatile uint8_t *control_register = impl->config.control_register;
  volatile uint8_t *status_register = impl->config.status_register;
  switch (rate_divider)
  {

    case SPI_CLOCK_RATE_DIVIDER_4:
      setClockRateDividerBitValues(control_register, 0b00);
      disableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_8:
      setClockRateDividerBitValues(control_register, 0b00);
      enableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_16:
      setClockRateDividerBitValues(control_register, 0b01);
      disableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_32:
      setClockRateDividerBitValues(control_register, 0b01);
      enableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_64:
      setClockRateDividerBitValues(control_register, 0b10);
      disableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_128:
      setClockRateDividerBitValues(control_register, 0b11);
      disableDoubleSpeed(status_register);
      break;

    default:
      Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
}

void
setClockRateDividerBitValues(volatile uint8_t *control_register,
                             uint8_t values)
{
  uint8_t bit_mask = 0b11;
  BitManipulation_setByte(control_register, bit_mask, values);
}

void
enableDoubleSpeed(volatile uint8_t *status_register)
{
  BitManipulation_setBit(status_register, double_spi_speed_bit);
}

void
disableDoubleSpeed(volatile uint8_t *status_register)
{
  BitManipulation_clearBit(status_register, double_spi_speed_bit);
}

void
setSPIMode(volatile uint8_t *control_register,
           uint8_t spi_mode)
{
  void (*set_clock_polarity)(volatile uint8_t *,
                             uint8_t) = BitManipulation_clearBit;
  void (*set_clock_phase)(volatile uint8_t *,
                          uint8_t) = BitManipulation_clearBit;

  switch (spi_mode)
  {

    case SPI_MODE_0:
      set_clock_polarity = BitManipulation_clearBit;
      set_clock_phase = BitManipulation_clearBit;
      break;

    case SPI_MODE_1:
      set_clock_polarity = BitManipulation_clearBit;
      set_clock_phase = BitManipulation_setBit;
      break;

    case SPI_MODE_2:
      set_clock_polarity = BitManipulation_setBit;
      set_clock_phase = BitManipulation_clearBit;
      break;

    case SPI_MODE_3:
      set_clock_polarity = BitManipulation_setBit;
      set_clock_phase = BitManipulation_setBit;
      break;

    default:
      Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
  set_clock_phase(control_register, clock_phase_bit);
  set_clock_polarity(control_register, clock_polarity_bit);
}

void
setDataOrder(volatile uint8_t *control_register,
             uint8_t data_order)
{
  switch (data_order)
  {

    case SPI_DATA_ORDER_LSB_FIRST:
      BitManipulation_setBit(control_register, data_order_bit);
      break;

    case SPI_DATA_ORDER_MSB_FIRST:
      BitManipulation_clearBit(control_register, data_order_bit);
      break;

    default:
      Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
}

