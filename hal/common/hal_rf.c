/******************************************************************************
    Filename: hal_rf.c

    This file contains functions for accessing the CC1100/CC1100 family
    of RF ICs from Texas Instruments.

    Copyright 2007 Texas Instruments, Inc.
******************************************************************************/

#include "hal_types.h"
#include "hal_board.h"
#include "hal_spi.h"
#include "hal_spi_config.h"
#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_rf.h"
#include "rf_config_setting.h"


// Include cc1100 IC
#include "cc1100.h"


//----------------------------------------------------------------------------------
//  void halRfResetChip(void)
//
//  DESCRIPTION:
//    Resets the chip using the procedure described in the datasheet.
//----------------------------------------------------------------------------------
void halRfResetChip(void)
{
    // Toggle chip select signal
    HAL_SPI_CS_DEASSERT;
    halMcuWaitUs(30);
    HAL_SPI_CS_ASSERT;
    halMcuWaitUs(30);
    HAL_SPI_CS_DEASSERT;
    halMcuWaitUs(45);

    // Send SRES command
    HAL_SPI_CS_ASSERT;
    while(HAL_SPI_SOMI_VAL);
    HAL_SPI_TXBUF_SET(CC1100_SRES);
    HAL_SPI_WAIT_TXFIN;

    // Wait for chip to finish internal reset
    while (HAL_SPI_SOMI_VAL);
    HAL_SPI_CS_DEASSERT;
}

//----------------------------------------------------------------------------------
//  void halRfConfig(const HAL_RF_CONFIG* rfConfig, const uint8* rfPaTable, uint8 rfPaTableLen)
//
//  DESCRIPTION:
//    Used to configure the CC1100/CC1100 registers with exported register
//    settings from SmartRF Studio.
//
//  ARGUMENTS:
//    rfConfig     - register settings (as exported from SmartRF Studio)
//    rfPaTable    - array of PA table values (from SmartRF Studio)
//    rfPaTableLen - length of PA table
//
//----------------------------------------------------------------------------------
void halRfConfig()
{
	uint8 writeByte;
  
	#ifdef PA_TABLE
  		//uint8 paTable[] = PA_TABLE;
	#endif

 	// write registers to radio
 	for(uint16 i = 0; i < ( sizeof(preferredSettings)/sizeof(registerSetting_t) ); i++) {
	    writeByte =  preferredSettings[i].data;
	    halRfWriteReg( preferredSettings[i].addr, writeByte);
	}

	#ifdef PA_TABLE
  		// write PA_TABLE
  		//halRfWriteReg(CC1101_PA_TABLE0,paTable, sizeof(paTable));
  		halSpiWrite(CC1100_PATABLE | CC1100_WRITE_BURST, myPaTable, myPaTableLen);
	#endif
}


//----------------------------------------------------------------------------------
//  void  halRfBurstConfig(const HAL_RF_BURST_CONFIG rfConfig, const uint8* rfPaTable, uint8 rfPaTableLen)
//
//  DESCRIPTION:
//    Used to configure all of the CC1100/CC1100 registers in one burst write.
//
//  ARGUMENTS:
//    rfConfig     - register settings
//    rfPaTable    - array of PA table values (from SmartRF Studio)
//    rfPaTableLen - length of PA table
//
//----------------------------------------------------------------------------------
void  halRfBurstConfig(const HAL_RF_BURST_CONFIG rfConfig, const uint8* rfPaTable, uint8 rfPaTableLen)
{
    halSpiWrite(CC1100_IOCFG2  | CC1100_WRITE_BURST, rfConfig, sizeof(rfConfig));
    halSpiWrite(CC1100_PATABLE | CC1100_WRITE_BURST, rfPaTable, rfPaTableLen);
}

//----------------------------------------------------------------------------------
//  uint8 halRfGetChipId(void)
//----------------------------------------------------------------------------------
uint8 halRfGetChipId(void)
{
    return(halRfReadStatusReg(CC1100_PARTNUM));
}

//----------------------------------------------------------------------------------
//  uint8 halRfGetChipVer(void)
//----------------------------------------------------------------------------------
uint8 halRfGetChipVer(void)
{
    return(halRfReadStatusReg(CC1100_VERSION));
}

//----------------------------------------------------------------------------------
//  HAL_RF_STATUS halRfStrobe(uint8 cmd)
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfStrobe(uint8 cmd)
{
    return(halSpiStrobe(cmd));
}

//----------------------------------------------------------------------------------
//  uint8 halRfReadStatusReg(uint8 addr)
//
//  NOTE:
//      When reading a status register over the SPI interface while the register
//      is updated by the radio hardware, there is a small, but finite, probability
//      that the result is corrupt. The CC1100 and CC1100 errata notes explain the
//      problem and propose several workarounds.
//
//----------------------------------------------------------------------------------
uint8 halRfReadStatusReg(uint8 addr)
{
    uint8 reg;
    halSpiRead(addr | CC1100_READ_BURST, &reg, 1);
    return(reg);
}

//----------------------------------------------------------------------------------
//  uint8 halRfReadReg(uint8 addr)
//----------------------------------------------------------------------------------
uint8 halRfReadReg(uint8 addr)
{
    uint8 reg;
    halSpiRead(addr | CC1100_READ_SINGLE, &reg, 1);
    return(reg);
}

//----------------------------------------------------------------------------------
//  HAL_RF_STATUS halRfWriteReg(uint8 addr, uint8 data)
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfWriteReg(uint8 addr, uint8 data)
{
    uint8 rc;
    rc = halSpiWrite(addr, &data, 1);
    return(rc);
}

//----------------------------------------------------------------------------------
//  HAL_RF_STATUS halRfWriteFifo(uint8* data, uint8 length)
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfWriteFifo(const uint8* data, uint8 length)
{
    return(halSpiWrite(CC1100_TXFIFO | CC1100_WRITE_BURST, data, length));
}

//----------------------------------------------------------------------------------
//  HAL_RF_STATUS halRfReadFifo(uint8* data, uint8 length)
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfReadFifo(uint8* data, uint8 length)
{
    return(halSpiRead(CC1100_RXFIFO | CC1100_READ_BURST, data, length));
}

//----------------------------------------------------------------------------------
//  uint8 halRfGetTxStatus(void)
//
//  DESCRIPTION:
//      This function transmits a No Operation Strobe (SNOP) to get the status of
//      the radio and the number of free bytes in the TX FIFO
//
//      Status byte:
//
//      ---------------------------------------------------------------------------
//      |          |            |                                                 |
//      | CHIP_RDY | STATE[2:0] | FIFO_BYTES_AVAILABLE (free bytes in the TX FIFO |
//      |          |            |                                                 |
//      ---------------------------------------------------------------------------
//
//  NOTE:
//      When reading a status register over the SPI interface while the register
//      is updated by the radio hardware, there is a small, but finite, probability
//      that the result is corrupt. This also applies to the chip status byte. The
//      CC1100 and CC1100 errata notes explain the problem and propose several
//      workarounds.
//
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfGetTxStatus(void)
{
    return(halSpiStrobe(CC1100_SNOP));
}

//----------------------------------------------------------------------------------
//  uint8 halRfGetRxStatus(void)
//
//  DESCRIPTION:
//      This function transmits a No Operation Strobe (SNOP) with the read bit set
//      to get the status of the radio and the number of available bytes in the RX
//      FIFO.
//
//      Status byte:
//
//      --------------------------------------------------------------------------------
//      |          |            |                                                      |
//      | CHIP_RDY | STATE[2:0] | FIFO_BYTES_AVAILABLE (available bytes in the RX FIFO |
//      |          |            |                                                      |
//      --------------------------------------------------------------------------------
//
//  NOTE:
//      When reading a status register over the SPI interface while the register
//      is updated by the radio hardware, there is a small, but finite, probability
//      that the result is corrupt. This also applies to the chip status byte. The
//      CC1100 and CC1100 errata notes explain the problem and propose several
//      workarounds.
//
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfGetRxStatus(void)
{
    return(halSpiStrobe(CC1100_SNOP | CC1100_READ_SINGLE));
}


