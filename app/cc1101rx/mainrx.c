/***
* main.c
*
* Spike Yang 2016 Nov 11
*
***/

#include "hal_board.h"
#include "hal_digio.h"
#include "hal_mcu.h"
#include "hal_led.h"
#include "hal_uart.h"
#include "hal_rf.h"
#include "cc1100.h"

#define RX_OK                 0
#define RX_LENGTH_VIOLATION   1
#define RX_CRC_MISMATCH       2
#define RX_FIFO_OVERFLOW      3

static char data[64];
static uint8 data_len;


//----------------------------------------------------------------------------------
//  Variables used in this file
//----------------------------------------------------------------------------------
static volatile uint8 packetReceived;

//----------------------------------------------------------------------------------
//  void rxISR(void)
//
//  DESCRIPTION:
//    This function is called (in interrupt context) every time a packet has been
//    revceived.
//----------------------------------------------------------------------------------
static void rxISR(void)
{
    packetReceived = TRUE;
    halUartSend("I\r\n");
}

//----------------------------------------------------------------------------------
//  uint8 rxRecvPacket(uint8* data, uint8* length)
//
//  DESCRIPTION:
//    Receive a packet that is smaller than the size of the FIFO, i.e. wait for the
//    complete packet to be received before reading from the FIFO. This function sets
//    the CC1100/CC2500 in RX and waits for the chip to signal that a packet is received.
//
//  ARGUMENTS:
//    data   - Where to write incoming data.
//    length - Length of payload.
//
//  RETURNS:
//    0 if a packet was received successfully.
//    1 if chip is in overflow state (packet longer than FIFO).
//    2 if the length of the packet is illegal (0 or > 61).
//    3 if the CRC of the packet is not OK.
//----------------------------------------------------------------------------------
static uint8 rxRecvPacket(char* data, uint8* length)
{
    uint8 packet_status[2];
    uint8 status;
    // uint16 key;

    packetReceived = FALSE;
    status = RX_OK;

    // Set radio in RX mode
    halRfStrobe(CC1100_SRX);

    // Wait for incoming packet
    //key = halIntLock();
    while(!packetReceived)
    {
        ;//halMcuSetLowPowerMode(HAL_MCU_LPM_3);
        //key = halIntLock();
    }
    //halIntUnlock(key);

    // Read first element of packet from the RX FIFO
    // First byte is length
    status = halRfReadFifo( length, 1);

    if ((status & CC1100_STATUS_STATE_BM) == CC1100_STATE_RX_OVERFLOW)
    {
        halRfStrobe(CC1100_SIDLE);
        halRfStrobe(CC1100_SFRX);
        status = RX_FIFO_OVERFLOW;
    }
    else if (*length == 0 || *length > 61)
    {
        halRfStrobe(CC1100_SIDLE);
        halRfStrobe(CC1100_SFRX);
        status = RX_LENGTH_VIOLATION;
    }
    else
    {
        // Get payload
        halRfReadFifo((uint8 *)data, *length);
	 data_len = *length;

        // Get the packet status bytes [RSSI, LQI]
        halRfReadFifo(packet_status, 2);

        // Check CRC
        if ((packet_status[1] & CC1100_LQI_CRC_OK_BM) != CC1100_LQI_CRC_OK_BM)
        {
            status = RX_CRC_MISMATCH;
        }
        else
        {
            status = RX_OK;
        }
    }
    return(status);

}

void main(void){
    uint8   id;
    uint8   ver;
    //uint16  counter = 1;     // this is as packet id
    uint8 payloadLength;;
        
    // board init
    halBoardInit();

    halUartSend("Rx App start ...\r\n");

    // Init cc1101 IC
    halRfResetChip();

    id  = halRfGetChipId();
    ver = halRfGetChipVer();

    // try to send out a packet when it receive command from UART
    halUartSend("Id:");
    halUartSendInt(id);
    halUartSend("\r\nVer:");
    halUartSendInt(ver);
    halUartSend("\r\n");
    
    // Setup chip with register settings from SmartRF Studio
    halRfConfig();

     // Connect RX interrupt to event on GDO0
    halDigioIntSetEdge(&pinGDO0, HAL_DIGIO_INT_FALLING_EDGE);
    halDigioIntConnect(&pinGDO0, &rxISR);
    halDigioIntEnable(&pinGDO0);

    id = halRfReadReg(CC1100_MCSM0);
    halUartSend("MCSM0:");
    halUartSendInt(id);
    halUartSend("\r\n");

    id = halRfReadReg(CC1100_FREQ2);
    halUartSend("FREQ2:");
    halUartSendInt(id);
    halUartSend("\r\n");

    id = halRfReadReg(CC1100_PTEST);
    halUartSend("PTEST:");
    halUartSendInt(id);
    halUartSend("\r\n");


    //_EINT();
     __enable_interrupt();
    
    // Infinite RX loop
    while(1){
	    if (rxRecvPacket(data, &payloadLength) == 0)
            {
                halUartSend(data);
		  data[data_len] = '\0';
		  halUartSend("\r\nRx OK\r\n");
            }
            else
            {	
                halUartSend("\r\nRx invalid\r\n");
        	//toggle the LED P3.7
        	halLedToggle(3);

		 // wait for some time
        	halMcuWaitMs(500);
            }
	 
    }
}


