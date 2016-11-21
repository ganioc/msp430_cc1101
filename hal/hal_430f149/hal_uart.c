/***********************************************************************************
    Filename: hal_uart.c

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#include "hal_types.h"
#include "hal_uart.h"
#include "hal_board.h"

#include "string.h"

static char uart_rx_buffer[UART_RX_BUFFER_LEN];
static short uart_rx_buffer_index = 0;

//----------------------------------------------------------------------------------
//  void halUartInit(uint8 baudrate, uint8 options)
//----------------------------------------------------------------------------------
void halUartInit(uint8 baudrate, uint8 options)
{
    // For the moment, this UART implementation only
    // supports communication settings 115200 8N1
    // i.e. ignore baudrate and options arguments.

    /***
    UCA0CTL1 |= UCSWRST;               // Keep USCI in reset state
    UCA0CTL1 |= UCSSEL_2;              // SMCLK
    UCA0BR0  = 0x22;                   // 4MHz 115200
    UCA0BR1  = 0x00;                   // 4MHz 115200
    UCA0MCTL = 0x08;                   // 4Mhz Modulation
    
    // Set up pins used by peripheral unit (USCI_A0)
    P2SEL |= BIT4;    // P2.4 = TXD

    UCA0CTL1 &= ~UCSWRST;              // Initialize USCI state machine
    ***/

    #ifdef  HAL_USART_INTERFACE_USART0
       P3SEL |= (BIT5 | BIT4);         // 选择P3.4和P3.5做UART通信端口
	ME1 |=  UTXE0 + URXE0;
	UCTL0 |= CHAR;			// 8位字符
	UTCTL0 |= SSEL0;                          // UCLK = ACLK
       UBR00 = 0x03;                             // 波特率9600
       UBR10 = 0x00;                             //
       UMCTL0 = 0x4A;                            // Modulation
       UCTL0 &= ~SWRST;                          // 初始化UART状态机
       IE1 |= URXIE0;			// + UTXIE0;     //URXIE0; // ;      
	
	
    #endif
    
}

//----------------------------------------------------------------------------------
//  void halUartWrite(const uint8* buf, uint16 length)
//----------------------------------------------------------------------------------
void halUartWrite(const char* buf, uint16 length)
{
/**
    uint16 i;
    for(i = 0; i < length; i++)
    {
        while (!(IFG2 & UCA0TXIFG));   // Wait for TX buffer ready to receive new byte
        UCA0TXBUF = buf[i];            // Output character
    }
**/
    #ifdef  HAL_USART_INTERFACE_USART0
        uint16 i;
        for(i=0; i<length; i++){
		while (!(IFG1 & UTXIFG0));             //等待以前的字符发送完毕
  		TXBUF0 = buf[i];
        }

    #endif
    
}

//----------------------------------------------------------------------------------
//  int halUartRead(uint8* buf, uint16 length)
//----------------------------------------------------------------------------------
/**
  return:
  	TRUE   -  Received a valid message
  	FALSE  -  Received nothing or not the end of a message
*/
int halUartRead()
{
	if( uart_rx_buffer_index == 0 ){
		return FALSE;
	}
	else if( uart_rx_buffer[uart_rx_buffer_index -1 ] == '\r' || 
	      uart_rx_buffer[uart_rx_buffer_index -1] == '\n'){

	      halUartWrite( uart_rx_buffer,  uart_rx_buffer_index -1 );
	      halUartWrite( "\n", 1);
		  
	      uart_rx_buffer[uart_rx_buffer_index ] = '\0';
    		//Send_String( uart_rx_buffer );
    		//Send_String( "\n");
			
    		//uart_rx_buffer_index = 0;
    		return TRUE;
  	}
	else{
    		//Send_String( ".");   
    		return FALSE;
  	}
}

void halUartSend(const char* buf){

	uint16 len = strlen(buf);
	
	halUartWrite( buf, len);
}

void halUartSendInt(const uint8 n){
  
  //uint8 iIn, 
  uint8 iHi, iLo;
  
  char str[10];
  
  
  iHi = (n>>4)&0x0f;
  
  iLo = (n)&0x0f;
  
  str[0] = (iHi>9)?(97 - 10 + iHi):(48 + iHi);
  
  str[1] = (iLo>9)?(97 - 10 + iLo):(48 + iLo);
  
  str[2] = '\0';

//  for(i=0;(number/10)==0;i++){
//    remainder=number%10;
//    ch=(back*pow(10,i)+48);
//    str[i]=ch;
//  }

  halUartSend(str);
}

void flushUartBuffer(){
	uart_rx_buffer_index = 0;

}


/*******************************************
function name：UART0_RXISR
function：UART0 rx interrupt service 
input：none
return  ：none
********************************************/
#pragma vector = UART0RX_VECTOR
__interrupt void UART0_RXISR(void)
{
	 //LPM1_EXIT;                 //Exit Low Power MOde 1
	uart_rx_buffer[uart_rx_buffer_index++] = RXBUF0;
	  //Send_Byte(RXBUF0);
}


