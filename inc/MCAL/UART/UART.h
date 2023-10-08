/*
 * UART.h
 *
 *  Created on: Oct 5, 2023
 *      Author: salah
 */

#ifndef UART_H_
#define UART_H_
#include "stm32f103x6.h"
#include "GPIO.h"
#include "RCC.h"

//configuration structure
typedef struct {
	uint8_t			USART_mode ; //specifies TX/RX Enale/Disable
								//this parameter based on @ref UART_Mode_define

	uint32_t	    BaudRte ; //specifies the Baud Rate of UART1
							 //this parameter based on @ref UART_BaudRate_define

	uint8_t			LoadLength ; //specifies the length of effective data to be transmitted
								//this parameter based on @ref UART_LoadLength_define

	uint8_t			parity ; //specifies the parity mode
							//this parameter based on @ref UART_Parity_define

	uint8_t			StopBits ; //specifies the number of stop bits
							  //this parameter based on @ref UART_StopBits_define

	uint8_t			HWFlowCTL ; //specifies Enabling flow control or not
							   //this parameter based on @ref UART_FlowCTL_define

	uint8_t			IRQ_Enable ; //Enable or disable the IRQ TX/RX
								//@ref UART_IRQ_Enable_define,you can select two or three options

	void(* P_IRQ_CallBack)(void) ; //set C function() to be called once IRQ happen

}UART_Config_t;


//..................................................
//=============== reference macros =================
//..................................................

//@ref UART_Mode_define
#define  UART_Mode_RX				(uint32_t)(1<<2)
#define  UART_Mode_TX				(uint32_t)(1<<3)
#define  UART_Mode_TX_RX			(uint32_t)((1<<3) | (1<<2))


//@ref UART_BaudRate_define
#define UART_BaudRate_2400			2400
#define UART_BaudRate_9600			9600
#define UART_BaudRate_19200			19200
#define UART_BaudRate_57600			57600
#define UART_BaudRate_115200		115200
#define UART_BaudRate_230400		230400
#define UART_BaudRate_460800		460800
#define UART_BaudRate_921600		921600
#define UART_BaudRate_2250000		2250000
#define UART_BaudRate_4500000		4500000


//@ref UART_LoadLength_define
#define UART_LoadLength_8B		(uint32_t)(0)
#define UART_LoadLength_9B		(uint32_t)(1<<12)


//@ref UART_Parity_define
#define UART_Parity_NONE	(uint32_t)(0)
#define UART_Parity_EVEN	(uint32_t)(1<<10)
#define UART_Parity_ODD		(uint32_t)(1<<10 | 1<<9)


//@ref UART_StopBits_define
#define UART_StopBits_1				(uint32_t)(0)
#define UART_StopBits_half			(uint32_t)(1<<12)
#define UART_StopBits_2				(uint32_t)(2<<12)
#define UART_StopBits_1_half		(uint32_t)(3<<12)


//@ref UART_FlowCTL_define
#define UART_FlowCTL_NONE				(uint32_t)(0)
#define UART_FlowCTL_RTS				(uint32_t)(1<<10)
#define UART_FlowCTL_CTS				(uint32_t)(1<<9)
#define UART_FlowCTL_RTS_CTS			(uint32_t)(1<<9 | 1<<10)


//@ref UART_IRQ_Enable_define
#define UART_IRQ_Enable_NONE			(uint32_t)(0)
#define UART_IRQ_Enable_TXE				(uint32_t)(1<<7)//transmit data register Empty
#define UART_IRQ_Enable_TC				(uint32_t)(1<<6)//transmission complete
#define UART_IRQ_Enable_RXNEIE			(uint32_t)(1<<5)//received data register is ready to be read
#define UART_IRQ_Enable_PE				(uint32_t)(1<<8)//parity error

enum Polling_mechanism {
	enable ,
	disable
};

//BaudRate calculation
//USARTDIV = FCLK /(16 * BaudRate )
//USARTDIV_MUL100=( (100*FCLK) /(16 * BaudRate )) == ((25*FCLK) / (4*BaudRate) )
//DIV_Mantessa_MUL100= INTGER(USARTDIV) * 100
//DIV_Mantessa =  INTGER(USARTDIV)
//DIV_Fraction = ((USARTDIV_MUL100 - DIV_Mantessa_MUL100)* 16)/100

#define USARTDIV(_PCLK_,_BAUD_)						(uint32_t)(_PCLK_/(16*_BAUD_))
#define USARTDIV_MUL_100(_PCLK_,_BAUD_)				(uint32_t)((25*_PCLK_) / (4*_BAUD_))
#define Mantissa_MUL_100(_PCLK_,_BAUD_)				(uint32_t)(USARTDIV(_PCLK_,_BAUD_) * 100)
#define Mantissa(_PCLK_,_BAUD_)						(uint32_t)(USARTDIV(_PCLK_,_BAUD_))
#define DIV_Fraction(_PCLK_,_BAUD_)					(uint32_t)(((USARTDIV_MUL_100(_PCLK_,_BAUD_)-Mantissa_MUL_100(_PCLK_,_BAUD_))*16)/100)
#define UART_BRRRegister(_PCLK_,_BAUD_)				( ( Mantissa(_PCLK_,_BAUD_)<<4 ) )|( (DIV_Fraction(_PCLK_,_BAUD_) )& 0xf )



/*
 ===============================================================
 *			APIS Supported by "MCAL UART DRIVER"
 *==============================================================
 */
void MCAL_UART_Init(USART_TypeDef* UARTX , UART_Config_t* UART_Config) ;
void MCAL_UART_DeInit(USART_TypeDef* UARTX) ;
void MCAL_UART_GPIO_Set_Pins(USART_TypeDef* UARTX) ;
void MCAL_UART_SendData(USART_TypeDef* UARTX , uint16_t* PTxuffer , enum Polling_mechanism PollingEN) ;
void MCAL_UART_ReceiveData(USART_TypeDef* UARTX , uint16_t* PTxuffer , enum Polling_mechanism PollingEN) ;
void MCAL_UART_Wait_TC(USART_TypeDef* UARTX) ;



#endif /* UART_H_ */
