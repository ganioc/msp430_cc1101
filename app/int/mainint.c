#include "hal_board.h"
#include "hal_digio.h"
#include "hal_mcu.h"
#include "hal_led.h"
#include "hal_uart.h"


static void txISR(void)
{
    halUartSend("i\r\n");
    // halMcuWaitUs(50);
}

void main(void){        
    // board init
    halBoardInit();

    
    halDigioIntSetEdge(&pinTest, HAL_DIGIO_INT_RISING_EDGE);
    halDigioIntConnect(&pinTest, &txISR); 
    halDigioIntEnable(&pinTest);


    halUartSend("App start ...\r\n");


    //_EINT();
     __enable_interrupt();
      
    while(1){
	// Read UART0 
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

