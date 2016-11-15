/******************************************************************************
    Filename: hal_mcu.c

    Copyright 2007 Texas Instruments, Inc.
******************************************************************************/

#include "hal_types.h"
#include "hal_defs.h"
#include "hal_board.h"
#include "hal_mcu.h"

/*---------------------------------------------------------------------------------

	Spike yang
//---------------------------------------------------------------------------------
*/
void halClkInit(){

    uint8 i;
    BCSCTL1 &= ~XT2OFF;//打开XT振荡器8mhz
    
    BCSCTL2 |= SELM_2 + SELS + DIVS_3;  //MCLK 8M and SMCLK 8M divided by 8  = 
    // 1M
    
    do
    {
      IFG1 &= ~OFIFG;//清除振荡错误标志 
      for(i = 0; i < 0xff; i++)  _NOP();//延时等待
    }
    while ((IFG1 & OFIFG) != 0);//如果标志为1继续循环等待
    
    IFG1&=~OFIFG;
}
//----------------------------------------------------------------------------------
//  void halMcuInit(void)
//
//  DESCRIPTION:
//    Turn off watchdog and set up system clock. 
//----------------------------------------------------------------------------------
void halMcuInit(void)
{
    // Stop watchdog
    WDTCTL = WDTPW + WDTHOLD;

    // Set up clock system on MCU to fit your system
    // Target specific implementation
    halClkInit();
}


//-----------------------------------------------------------------------------
//  void halMcuWaitUs(uint16 usec)
//
//  DESCRIPTION:
//    Busy wait function. Waits the specified number of microseconds. Use
//    assumptions about number of clock cycles needed for the various instructions.
//    The duration of one cycle depends on the system clock. In this TEMPLATE HAL,
//    system clock is 1 MHz, thus 1 cycles per usec.
//
//    NB! This function is highly dependent on architecture and compiler!
//-----------------------------------------------------------------------------
void halMcuWaitUs(uint16 usec) // 5 cycles for calling
{
    // In this example, the least we can wait is 12 usec:
    // ~1 us for call, 1 us for first compare and 1 us for return

    while(usec > 12)      // 2 cycles for compare
    {                     // 2 cycles for jump
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        usec -= 10;       // 1 cycles for optimized decrement
    }
} 

// Added by Spike Yang
void halMcuWaitMs(uint16 msec){
	while(msec > 0){

	    halMcuWaitUs(8000);

	    msec--;
	}
}

//-----------------------------------------------------------------------------
//  void halMcuSetLowPowerMode(uint8 mode)
//
//  DESCRIPTION:
//    Sets the MCU in a low power mode. Will turn global interrupts on at
//    the same time as entering the LPM mode. The MCU must be waken from
//    an interrupt (status register on stack must be modified).
//-----------------------------------------------------------------------------
void halMcuSetLowPowerMode(uint8 mode)
{
    switch (mode)
    {
        case HAL_MCU_LPM_0:
            __low_power_mode_0();
            break;
        case HAL_MCU_LPM_1:
            __low_power_mode_1();
            break;
        case HAL_MCU_LPM_2:
            __low_power_mode_2();
            break;
        case HAL_MCU_LPM_3:
            __low_power_mode_3();
            break;
        case HAL_MCU_LPM_4:
            __low_power_mode_4();
            break;
    }
}
