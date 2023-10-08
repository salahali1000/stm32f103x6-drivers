/*
 * UART.c
 *
 *  Created on: Oct 5, 2023
 *      Author: salah
 */


#include "UART.h"

/*
 * =============================================================================
 * 								Generic variables
 * =============================================================================
 */
UART_Config_t* Global_UART_Config = NULL ;



/*
 * =============================================================================
 * 								Generic Functions
 * =============================================================================
 */


/*
 * =============================================================================
 * 								APIS
 * =============================================================================
 */

/**==============================================================================
 * @fn				-MCAL_UART_Init
 * @brief			-initialize UART (support asynchronous mode only)
 * @param [in]		-UARTX : where x can be (1..3 depending on the used device)
 * @param [in]		-UART_Config which has all configuration
 * @retval			-none
 * @note			-support asynchronous mode only & clock 8MHZ
 */
void MCAL_UART_Init(USART_TypeDef* UARTX , UART_Config_t* UART_Config)
{
	uint32_t pclk ,Brr;
	Global_UART_Config = UART_Config ;
	//Enable clock for UART
	if(UARTX == USART1)
		RCC_USART1_CLK_EN()	;

	else if(UARTX == USART2)
		RCC_USART2_CLK_EN()	;

	else if (UARTX == USART3)
		RCC_USART3_CLK_EN() ;

	//Enable UART
	//Bit 13 UE: USART enable
	UARTX->CR1 |= (1<<13) ;

	//Enable UART TX/RX According to UART configuration item
	UARTX->CR1 |= UART_Config->USART_mode ;

	//configure PATLOAD WIDTH According to UART configuration item
	UARTX->CR1 |= UART_Config->LoadLength ;

	//configure Parity Bits According to UART configuration item
	UARTX->CR1 |= UART_Config->parity ;

	//configure the number of stop Bits According to UART configuration item
	UARTX->CR2 |= UART_Config->StopBits ;

	//UART hardware flow control
	UARTX->CR3 |= UART_Config->HWFlowCTL ;

	//configuration of Baud Rate
	//PCLK1 for UART2,UART3
	//PCLK2 for UART1
	if(UARTX == USART1)
	{
		pclk = MCAL_RCC_GetPCLK2Freq() ;
	}
	else
	{
		pclk = MCAL_RCC_GetPCLK1Freq() ;
	}
	Brr = UART_BRRRegister(pclk,UART_Config->BaudRte) ;
	UARTX->BRR = Brr ;

	//Enable/Disable interrupt
	if (UART_Config->IRQ_Enable != UART_IRQ_Enable_NONE)
	{
		UARTX->CR1 |= UART_Config->IRQ_Enable ;
		//Enable NVIC for IRQ interrupt
		if (UARTX == USART1)
		{
			NVIC_IRQ37_USART1_Enable ;
		}
		else if (UARTX == USART2)
		{
			NVIC_IRQ38_USART2_Enable ;

		}
		else if (UARTX == USART3)
		{
			NVIC_IRQ39_USART3_Enable ;
		}
	}

}






/**==============================================================================
 * @fn				-MCAL_UART_DeInit
 * @brief			-DEInitialize UART (support asynchronous mode only)
 * @param [in]		-UARTX : where x can be (1..3 depending on the used device)
 * @retval			-none
 * @note			-Reset the module by RCC
 */

void MCAL_UART_DeInit(USART_TypeDef* UARTX)
{
	if(UARTX == USART1)
	{
		RCC_USART1_RESET() ;
		NVIC_IRQ37_USART1_Disable ;
	}
	else if (UARTX == USART2)
	{
		RCC_USART2_RESET() ;
		NVIC_IRQ38_USART2_Disable ;
	}
	else if (UARTX == USART3)
	{
		RCC_USART3_RESET() ;
		NVIC_IRQ39_USART3_Disable ;
	}
}





/**==============================================================================
 * @fn				-MCAL_UART_SendData
 * @brief			-send buffer on UART
 * @param [in]		-UARTX : where x can be (1..3 depending on the used device)
 * @param [in]		-PTxuffer
 * @param [in]		-PollingEN to enable or disable polling mechansim
 * @retval			-none
 * @note			-you should initialize UART first.
 * When transmitting with the parity enabled (PCE bit set to 1 in the USART_CR1 register),
 * the value written in the MSB (bit 7 or bit 8 depending on the data length) has no effect
 * because it is replaced by the parity.
 * because it is replaced by the parity.
 * bit
 */
void MCAL_UART_SendData(USART_TypeDef* UARTX , uint16_t* PTxuffer , enum Polling_mechanism PollingEN)
{
	//wait until TXE Flag is set
	if (PollingEN == enable)
		while( !(UARTX->SR & 1<<7) ) ;

	//check the UART word length is 8 Bit or 9 Bit in the frame .
	if (Global_UART_Config->LoadLength == UART_LoadLength_9B)
	{
		UARTX->DR = (*PTxuffer & (uint16_t)0x01ff) ;
	}
	else if (Global_UART_Config->LoadLength == UART_LoadLength_8B)
	{
		UARTX->DR = (*PTxuffer & (uint8_t)0xff) ;
	}

}





void MCAL_UART_Wait_TC(USART_TypeDef* UARTX)
{
	//wait until TXE Flag is set
	while( !(UARTX->SR & 1<<7) ) ;
}





/**==============================================================================
 * @fn				-MCAL_UART_ReceiveData
 * @brief			-receive the data on UART
 * @param [in]		-UARTX : where x can be (1..3 depending on the used device)
 * @param [in]		-PTxuffer
 * @param [in]		-PollingEN to enable or disable polling mechansim
 * @retval			-none
 * @note			-none
 */
void MCAL_UART_ReceiveData(USART_TypeDef* UARTX , uint16_t* PTxuffer , enum Polling_mechanism PollingEN)
{
	//wait until RXNE is set in SR register
	if (PollingEN == enable)
		while ( !(UARTX->SR & 1<<5) ) ;

	//check the UART word length is 8 Bit or 9 Bit in the frame .
	if (Global_UART_Config->LoadLength == UART_LoadLength_9B)
	{
		if (Global_UART_Config->parity == UART_Parity_NONE)
		{
			//no parity so all 9Bits are data
			*( (uint16_t*) PTxuffer) = UARTX->DR ;
		}
		else
		{
			//there is parity , so just 8Bits are data , the 9th bit is parity.
			*( (uint16_t*) PTxuffer) = (UARTX->DR & (uint8_t)0xff) ;
		}
	}
	else
	{
		if (Global_UART_Config->parity == UART_Parity_NONE)
		{
			//no parity , so first 8Bits are data
			*( (uint16_t*) PTxuffer) = (UARTX->DR & (uint8_t)0xff);
		}
		else
		{
			//there is parity , so just 7Bits are data , the 8th bit is parity
			*( (uint16_t*) PTxuffer) = (UARTX->DR & (uint8_t)0x7f) ;
		}

	}
}





/**==============================================================================
 * @fn				-MCAL_UART_GPIO_Set_Pins
 * @brief			-initialize GPIO Pins
 * @param [in]		-UARTX : where x can be (1..3 depending on the used device)
 * @retval			-none
 * @note			-should enable the ALT & GPIO in RCC after called MCAL_UART_Init()
 */
void MCAL_UART_GPIO_Set_Pins(USART_TypeDef* UARTX)
{
	GPIO_PinConfig_t PinCnfg ;
	if (UARTX == USART1)
	{
		//PA9 TX
		PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
		PinCnfg.GPIO_PinNumber = GPIO_PIN_9 ;
		PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
		MCAL_GPIO_Init(GPIOA, &PinCnfg) ;

		//PA10 RX
		PinCnfg.GPIO_MODE = GPIO_MODE_AF_INPUT ;
		PinCnfg.GPIO_PinNumber = GPIO_PIN_10 ;
		MCAL_GPIO_Init(GPIOA, &PinCnfg) ;

		//PA11 CTS
		if (Global_UART_Config->HWFlowCTL == UART_FlowCTL_CTS || Global_UART_Config->HWFlowCTL == UART_FlowCTL_RTS_CTS)
		{
			PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
			PinCnfg.GPIO_PinNumber = GPIO_PIN_11 ;
			MCAL_GPIO_Init(GPIOA, &PinCnfg) ;
		}

		//PA12 RTS
		if (Global_UART_Config->HWFlowCTL == UART_FlowCTL_RTS || Global_UART_Config->HWFlowCTL == UART_FlowCTL_RTS_CTS)
		{
			PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
			PinCnfg.GPIO_PinNumber = GPIO_PIN_12 ;
			PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
			MCAL_GPIO_Init(GPIOA, &PinCnfg) ;
		}
	}
	if (UARTX == USART2)
	{
		//PA2 TX
		PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
		PinCnfg.GPIO_PinNumber = GPIO_PIN_2 ;
		PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
		MCAL_GPIO_Init(GPIOA, &PinCnfg) ;

		//PA3 RX
		PinCnfg.GPIO_MODE = GPIO_MODE_AF_INPUT ;
		PinCnfg.GPIO_PinNumber = GPIO_PIN_3 ;
		MCAL_GPIO_Init(GPIOA, &PinCnfg) ;

		//PA0 CTS
		if (Global_UART_Config->HWFlowCTL == UART_FlowCTL_CTS || Global_UART_Config->HWFlowCTL == UART_FlowCTL_RTS_CTS)
		{
			PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
			PinCnfg.GPIO_PinNumber = GPIO_PIN_0 ;
			MCAL_GPIO_Init(GPIOA, &PinCnfg) ;
		}

		//PA1 RTS
		if (Global_UART_Config->HWFlowCTL == UART_FlowCTL_RTS || Global_UART_Config->HWFlowCTL == UART_FlowCTL_RTS_CTS)
		{
			PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
			PinCnfg.GPIO_PinNumber = GPIO_PIN_1 ;
			PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
			MCAL_GPIO_Init(GPIOA, &PinCnfg) ;
		}
	}
	if (UARTX == USART3)
	{
		//PB10 TX
		PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
		PinCnfg.GPIO_PinNumber = GPIO_PIN_10 ;
		PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
		MCAL_GPIO_Init(GPIOB, &PinCnfg) ;

		//PB11 RX
		PinCnfg.GPIO_MODE = GPIO_MODE_AF_INPUT ;
		PinCnfg.GPIO_PinNumber = GPIO_PIN_11 ;
		MCAL_GPIO_Init(GPIOB, &PinCnfg) ;

		//PB13 CTS
		if (Global_UART_Config->HWFlowCTL == UART_FlowCTL_CTS || Global_UART_Config->HWFlowCTL == UART_FlowCTL_RTS_CTS)
		{
			PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
			PinCnfg.GPIO_PinNumber = GPIO_PIN_13 ;
			MCAL_GPIO_Init(GPIOB, &PinCnfg) ;
		}

		//PB14 RTS
		if (Global_UART_Config->HWFlowCTL == UART_FlowCTL_RTS || Global_UART_Config->HWFlowCTL == UART_FlowCTL_RTS_CTS)
		{
			PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
			PinCnfg.GPIO_PinNumber = GPIO_PIN_14 ;
			PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
			MCAL_GPIO_Init(GPIOB, &PinCnfg) ;
		}
	}
}



//ISR
void USART1_IRQHandler(void)
{
	Global_UART_Config->P_IRQ_CallBack() ;
}

void USART2_IRQHandler(void)
{
	Global_UART_Config->P_IRQ_CallBack() ;
}

void USART3_IRQHandler(void)
{
	Global_UART_Config->P_IRQ_CallBack() ;
}



