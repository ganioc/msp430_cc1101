/* Chipcon */
/* Product = CC1100 */
// Sync word qualifier mode = 30/32 sync word bits detected 
// CRC autoflush = false 
// Channel spacing = 199.951172 
// Data format = Normal mode 
// Data rate = 1.19948 
// RX filter BW = 58.035714 
// PA ramping = false 
// Preamble count = 4 
// Whitening = false 
// Address config = No address check 
// Carrier frequency = 867.999939 
// Device address = 0 
// TX power = 0 
// Manchester enable = false 
// CRC enable = true 
// Deviation = 5.157471 
// Packet length mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Packet length = 255 
// Modulation format = GFSK 
// Base frequency = 867.999939 
// Modulated = true 
// Channel number = 0 
// PA table 

#include "cc1100.h"

#define PA_TABLE {0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}

//CC1101,  GFSK, 250kbps, 0dBm, 434MHz, 0 channel
static const registerSetting_t preferredSettings[]= 
{
  {CC1100_IOCFG0,      0x06},    // osc/192
  {CC1100_PKTCTRL0,    0x05},  // varable packet length, determined by first 
							//byte after sync word
							// CRC enabled
							// packet format, use FIFO
							// white_data, 0
  // PKTCTRL1 ,  no address check
  {CC1100_FSCTRL1,     0x12},   // IF freq, 
  {CC1100_FREQ2,       0x10},    // 
  {CC1100_FREQ1,       0xB1},
  {CC1100_FREQ0,       0x3B},    // 0x10b13b, 433.999MHz
  {CC1100_MDMCFG4,     0x2D},
  {CC1100_MDMCFG3,     0x3B},
  {CC1100_MDMCFG2,     0x93},  // 30/32 sync work detect
  							// GFSK
  							// disable DC blocking before modulation
  {CC1100_DEVIATN,     0x62},
  {CC1100_MCSM0,       0x18},   
  {CC1100_FOCCFG,      0x1D},  
  {CC1100_BSCFG,       0x1C},    // 
  {CC1100_AGCCTRL2,    0xC7}, // 
  {CC1100_AGCCTRL1,    0x00},
  {CC1100_AGCCTRL0,    0xB0},
  {CC1100_WORCTRL,     0xFB},
  {CC1100_FREND1,      0xB6},  
  {CC1100_FSCAL3,      0xEA},  // 
  {CC1100_FSCAL2,      0x2A},
  {CC1100_FSCAL1,      0x00},
  {CC1100_FSCAL0,      0x1F},
  {CC1100_TEST0,       0x09}   //disable vco select calibration 

   /*
  {CC1100_IOCFG0,      0x06},
  {CC1100_FIFOTHR,     0x47},
  {CC1100_PKTCTRL0,    0x05},
  {CC1100_FSCTRL1,     0x06},
  {CC1100_FREQ2,       0x21},
  {CC1100_FREQ1,       0x62},
  {CC1100_FREQ0,       0x76},
  {CC1100_MDMCFG4,     0xF5},
  {CC1100_MDMCFG3,     0x83},
  {CC1100_MDMCFG2,     0x13},
  {CC1100_DEVIATN,     0x15},
  {CC1100_MCSM0,       0x18},
  {CC1100_FOCCFG,      0x16},
  {CC1100_WORCTRL,     0xFB},
  {CC1100_FSCAL3,      0xE9},
  {CC1100_FSCAL2,      0x2A},
  {CC1100_FSCAL1,      0x00},
  {CC1100_FSCAL0,      0x1F},
  {CC1100_TEST2,       0x81},
  {CC1100_TEST1,       0x35},
  {CC1100_TEST0,       0x09},
  */
};

const uint8 myPaTable[] = {0x60, 0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
// 0x60    0dBm for 433
// 0x50  0dBm for 800MHz, 

const uint8 myPaTableLen = 8;

/** footnote
434MHz, 
250bps (249.939 Baud)
channel spacing : 199.95kHz
Xtal: 26MHz
BW fiter: 541.666kHz
Modulation: MSK
TX power: 0dBm
Phase transition time: 0

static const registerSetting_t preferredSettings[]= 
{
  {CC1100_IOCFG0,      0x06},
  {CC1100_PKTCTRL0,    0x05},
  {CC1100_FSCTRL1,     0x0A},
  {CC1100_FREQ2,       0x10},
  {CC1100_FREQ1,       0xB1},
  {CC1100_FREQ0,       0x3B},
  {CC1100_MDMCFG4,     0x2D},
  {CC1100_MDMCFG3,     0x3B},
  {CC1100_MDMCFG2,     0x73},
  {CC1100_DEVIATN,     0x00},
  {CC1100_MCSM0,       0x18},
  {CC1100_FOCCFG,      0x1D},
  {CC1100_BSCFG,       0x1C},
  {CC1100_AGCCTRL2,    0xC7},
  {CC1100_AGCCTRL1,    0x00},
  {CC1100_AGCCTRL0,    0xB0},
  {CC1100_FREND1,      0xB6},
  {CC1100_FSCAL3,      0xEA},
  {CC1100_FSCAL2,      0x2A},
  {CC1100_FSCAL1,      0x00},
  {CC1100_FSCAL0,      0x1F},
  {CC1100_TEST0,       0x09},
};

433MHz
GFSK
250kHz
deviation 126.9kHz


static const registerSetting_t preferredSettings[]= 
{
  {CC1101_IOCFG0,      0x06},
  {CC1101_PKTCTRL0,    0x05},
  {CC1101_FSCTRL1,     0x12},
  {CC1101_FREQ2,       0x10},
  {CC1101_FREQ1,       0xB1},
  {CC1101_FREQ0,       0x3B},
  {CC1101_MDMCFG4,     0x2D},
  {CC1101_MDMCFG3,     0x3B},
  {CC1101_MDMCFG2,     0x93},
  {CC1101_DEVIATN,     0x62},
  {CC1101_MCSM0,       0x18},
  {CC1101_FOCCFG,      0x1D},
  {CC1101_BSCFG,       0x1C},
  {CC1101_AGCCTRL2,    0xC7},
  {CC1101_AGCCTRL1,    0x00},
  {CC1101_AGCCTRL0,    0xB0},
  {CC1101_WORCTRL,     0xFB},
  {CC1101_FREND1,      0xB6},
  {CC1101_FSCAL3,      0xEA},
  {CC1101_FSCAL2,      0x2A},
  {CC1101_FSCAL1,      0x00},
  {CC1101_FSCAL0,      0x1F},
  {CC1101_TEST0,       0x09},
};

MSK

static const registerSetting_t preferredSettings[]= 
{
  {CC1101_IOCFG0,      0x06},
  {CC1101_PKTCTRL0,    0x05},
  {CC1101_FSCTRL1,     0x12},
  {CC1101_FREQ2,       0x10},
  {CC1101_FREQ1,       0xB1},
  {CC1101_FREQ0,       0x3B},
  {CC1101_MDMCFG4,     0x2D},
  {CC1101_MDMCFG3,     0x3B},
  {CC1101_MDMCFG2,     0xF3},
  {CC1101_DEVIATN,     0x60},
  {CC1101_MCSM0,       0x18},
  {CC1101_FOCCFG,      0x1D},
  {CC1101_BSCFG,       0x1C},
  {CC1101_AGCCTRL2,    0xC7},
  {CC1101_AGCCTRL1,    0x00},
  {CC1101_AGCCTRL0,    0xB0},
  {CC1101_WORCTRL,     0xFB},
  {CC1101_FREND1,      0xB6},
  {CC1101_FSCAL3,      0xEA},
  {CC1101_FSCAL2,      0x2A},
  {CC1101_FSCAL1,      0x00},
  {CC1101_FSCAL0,      0x1F},
  {CC1101_TEST0,       0x09},
};

*/
