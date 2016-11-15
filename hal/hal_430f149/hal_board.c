/***********************************************************************************
    Filename: hal_board.c

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#include "hal_types.h"
#include "hal_defs.h"
#include "hal_digio.h"
//#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_board.h"
//#include "hal_lcd.h"
#include "hal_spi.h"
#include "hal_uart.h"

//------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------

// The constants below define some of the I/O signals used by the board
// Port, pin number, pin bitmask, direction and initial value should be
// set in order to match the target hardware. Once defined, the pins are
// configured in halBoardInit() by calling halDigioConfig()

// normal state is: 1
const digioConfig pinGDO0   = {1, 2, BIT2, HAL_DIGIO_INPUT, 1};
const digioConfig pinGDO2   = {1, 3, BIT3, HAL_DIGIO_INPUT, 0};
const digioConfig pinTest =     {1, 0, BIT0, HAL_DIGIO_INPUT, 1};
const digioConfig pinLED      = {3, 7, BIT7, HAL_DIGIO_OUTPUT, 0};


//------------------------------------------------------------------------------
//  void halBoardInit(void)
//
//  DESCRIPTION:
//    Set up board. Initialize MCU, configure I/O pins and user interfaces
//------------------------------------------------------------------------------
void halBoardInit(void)
{
    // Configure MCU (set up clock system etc.)
    halMcuInit();

    /*下面六行程序关闭所有的IO口*/
    
    P1DIR = 0XFF;P1OUT = 0XFF;
    P2DIR = 0XFF;P2OUT = 0XFF;
    P3DIR = 0XFF;P3OUT = 0XFF;
    P4DIR = 0XFF;P4OUT = 0XFF;
    P5DIR = 0XFF;P5OUT = 0XFF;
    P6DIR = 0XFF;P6OUT = 0XFF;
    

    // Configure digital I/O pins.  Did I set GDO0 function in cc1101 config 
    // program ??
    halDigioConfig(&pinGDO0);
    //halDigioConfig(&pinGDO2);
    halDigioConfig(&pinLED);
    halDigioConfig(&pinTest);  // Test an IO interrupt

    //halDigioSet(&pinTest);

    // Optionally, you can configure I/O pins using these macros
    //MCU_IO_OUTPUT(1, 0, 1); // LED 1 (port 1, pin 0, initial state on)
    //MCU_IO_OUTPUT(1, 1, 0); // LED 2 (port 1, pin 1, initial state off)

	

    // Initialize SPI interface
    // remove by Spike Yang, later
    halSpiInit(0);
    
    // Add other initialization functions here

    // init UART0 for communication purpose with PC
    halUartInit(0, 0);




}

/*
void halToggleLED(){
    halDigioToggle(&pinLED);
}
*/


