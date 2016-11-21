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



static uint8 data[64];

//----------------------------------------------------------------------------------
//  Variables used in this file
//----------------------------------------------------------------------------------
static volatile uint8 packetSent;


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
    halUartSend("i\r\n");
}


void main(void){        
    // board init
    halBoardInit();

    halUartSend("App start ...\r\n");
    
    halDigioIntSetEdge(&pinTest, HAL_DIGIO_INT_FALLING_EDGE);
    halDigioIntConnect(&pinTest, &txISR); 
    halDigioIntEnable(&pinTest);

    txISR();
	
    while(1){
	 // Read UART0 
	 if( TRUE == halUartRead() ){
	 	flushUartBuffer();
		
	 }
	
        //toggle the LED P3.7
        halLedToggle(3);


	 // wait for some time
        halMcuWaitMs(500);
	 
    }
}


