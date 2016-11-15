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
#include "string.h"

static uint8 data[64];

//----------------------------------------------------------------------------------
//  Variables used in this file
//----------------------------------------------------------------------------------
static volatile uint8 packetSent;

//---------------------------------------------------------------------------
//  void preparePacket(uint8 id, uint8* data, uint8* length)
//
//  DESCRIPTION:
//    Set up a dummy packet, where the first byte contains the length of the payload
//    and the first byte of the payload contains a packet id.
//---------------------------------------------------------------------------
static void preparePacket(uint8 id, uint8* data, uint8* length)
{
    uint8 i;
    
    char packet[] = "address:contents123456789";
    uint8 payloadLength = strlen(packet);

    //payloadLength++;
	
    //if (payloadLength > 61)
    //    payloadLength = 1;

    // First byte of packet contains the length of the payload
    data[0] = payloadLength;

    // First byte of payload contains an id
    //data[1] = ':';

    // Fill rest of packet with dummy data
    //for (i = 2; i <= payloadLength; i++)
    //    data[i] = i;
   // data[2] = 'h';
    //data[3] = 'i';
    for( i = 1; i<= payloadLength; i++){
	data[i]= packet[i-1];

    }

    // Packet length is payload + length byte
    *length = payloadLength + 1;
}

//----------------------------------------------------------------------------------
//  uint8 txSendPacket(uint8* data, uint8 length)
//
//  DESCRIPTION:
//    Send a packet that is smaller than the size of the FIFO, making it possible
//    to write the whole packet at once. Wait for the radio to signal that the packet
//    has been transmitted.
//
//  ARGUMENTS:
//    data   - Data to send. First byte contains length byte
//    length - Total length of packet to send
//
//  RETURNS:
//    This function always returns 0.
//----------------------------------------------------------------------------------
static uint8 txSendPacket(uint8* data, uint8 length)
{
    //uint16 key;
    packetSent = FALSE;

    // Write data to FIFO
    halRfWriteFifo(data, length);

    //halUartSend("s1\r\n");

    // Set radio in transmit mode
    halRfStrobe(CC1100_STX);

    //halUartSend("s2\r\n");

    // Wait for packet to be sent 
    //key = halIntLock();
    while(!packetSent)
    {
    	;
        //halMcuSetLowPowerMode(HAL_MCU_LPM_3);
        //key = halIntLock();
    }
    //halIntUnlock(key);
    return(0);
}

//----------------------------------------------------------------------------------
//  void txISR(void)
//
//  DESCRIPTION:
//    This function is called (in interrupt context) every time a packet has been
//    transmitted.
//----------------------------------------------------------------------------------
static void txISR(void)
{
    packetSent = TRUE;
    halUartSend("I\r\n");
}


void main(void){
    uint8   id;
    uint8   ver;
    uint16  counter = 1;     // this is as packet id
    uint8   packetLength;
        
    // board init
    halBoardInit();

    halUartSend("Tx  App start ...\r\n");
    
    // Init cc1101 IC
    halRfResetChip();

    id  = halRfGetChipId();
    ver = halRfGetChipVer();
    
    halUartSend("Id:");
    halUartSendInt(id);
    halUartSend("\r\nVer:");
    halUartSendInt(ver);
    halUartSend("\r\n");

    // try to send out a packet when it receive command from UART

    
    // Setup chip with register settings from SmartRF Studio
    halRfConfig();

    // Additional chip configuration for this example
    //halRfWriteReg(CC1100_MCSM0,    0x18);   // Calibration from IDLE to TX/RX
    //halRfWriteReg(CC1100_MCSM1,    0x00);   // No CCA, IDLE after TX and RX
    //halRfWriteReg(CC1100_PKTCTRL0, 0x45);   // Enable data whitening and CRC
    //halRfWriteReg(CC1100_PKTCTRL1, 0x04);   // Enable append mode
    //halRfWriteReg(CC1100_IOCFG0,   0x06);   // Set GDO0 to be packet received signal
    /**
	Page 56, 
    */

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

	
    // Connect TX interrupt to event on GDO0
    halDigioIntSetEdge(&pinGDO0, HAL_DIGIO_INT_FALLING_EDGE);
    halDigioIntConnect(&pinGDO0, &txISR);
    halDigioIntEnable(&pinGDO0);
    
    halDigioIntSetEdge(&pinTest, HAL_DIGIO_INT_FALLING_EDGE);
    halDigioIntConnect(&pinTest, &txISR); 
    halDigioIntEnable(&pinTest);


    //_EINT();
     __enable_interrupt();
		
    while(1){
	 // Read UART0 
	 halUartRead();
	
        //toggle the LED P3.7
        halLedToggle(3);


        // Send out a packet
        // Create a dummy packet
        // id is 1, content is "hi"
        preparePacket(counter, data, &packetLength);
        
        // Send packet
        txSendPacket(data, packetLength);

	 // wait for some time
        halMcuWaitMs(1000);
	 
    }
}


