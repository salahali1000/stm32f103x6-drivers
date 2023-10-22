/*
 * SPI.c
 *
 *  Created on: Oct 12, 2023
 *      Author: salah
 */
#include "SPI.h"
/*
 * ======================================================
 * 			         GENERIC VARIABLES
 * ======================================================
 */
SPI_Config_t* Global_SPI_Config[2] = {NULL,NULL} ;
/*
 * ======================================================
 * 			         GENERIC MACROS
 * ======================================================
 */
#define SPI1_Index			0
#define SPI2_Index			1


#define SPI_SR_TXE			((uint8_t)0x02)   //Transmit buffer is empty
#define SPI_SR_RXNE			((uint8_t)0x01)   //receive buffer is not empty


/*
 * ======================================================
 * 			             APIS
 * ======================================================
 */

/**==============================================================================
 * @fn				-MCAL_SPI_Init
 * @brief			-initialize SPI according to specific configuration
 * @param [in]		-SPIX : where x can be (1 , 2) to select SPI Peripheral
 * @param [in]		-SPI_Config is a pinter to structure SPI_Config_t that contains the
 * 					  configuration of SPI
 * @retval			-none
 * @note			-support Full Duplex for (master and slave) only.
 *================================================================================*/
void MCAL_SPI_Init(SPI_TypeDef* SPIX , SPI_Config_t* SPI_Config)
{
	//safety for registers
	uint16_t Tempreg_CR1 = 0 ;
	uint16_t Tempreg_CR2 = 0 ;
	if (SPIX == SPI1)
	{
		Global_SPI_Config[SPI1_Index] = SPI_Config ;
		RCC_SPI1_CLK_EN() ;
	}
	else if (SPIX == SPI2)
	{
		Global_SPI_Config[SPI2_Index] = SPI_Config ;
		RCC_SPI2_CLK_EN() ;
	}
	//Enable SPI , CR1 .Bit 6 SPE: SPI enable
	Tempreg_CR1 |= (0x1U<<6) ;
	//===========================================================================
	//Master or Slave
	Tempreg_CR1 |= SPI_Config->Device_Mode ;
	//===========================================================================
	//SPI Communication Mode
	Tempreg_CR1 |= SPI_Config->Communication_Mode ;
	//===========================================================================
	//SPI_Data_Size
	Tempreg_CR1 |= SPI_Config->Data_Size ;
	//===========================================================================
	//SPI_Frame_Format
	Tempreg_CR1 |= SPI_Config->Frame_Format ;
	//===========================================================================
	//SPI_CLKPolarity
	Tempreg_CR1 |= SPI_Config->CLKPolarity ;
	//===========================================================================
	//SPI_CLKPhase
	Tempreg_CR1 |= SPI_Config->CLKPhase ;
	//==========================SPI_NSS===========================//
	if(SPI_Config->NSS == SPI_NSS_Hard_Master_Output_Enable)
	{
		Tempreg_CR2 |= SPI_Config->NSS ;
	}
	else if (SPI_Config->NSS == SPI_NSS_Hard_Master_Output_Disable)
	{
		Tempreg_CR2 &= SPI_Config->NSS ;
	}
	else
	{
		Tempreg_CR1 |= SPI_Config->NSS ;
	}
	//===========================================================================
	//SPI_BaudRate_Scaler
	Tempreg_CR1 |= SPI_Config->BaudRate_Scaler ;
	//===========================================================================
	//SPI_IRQ_Enable
	if(SPI_Config->IRQ_Enable != SPI_IRQ_Enable_NONE)
	{
		Tempreg_CR2 |= SPI_Config->IRQ_Enable ;

		if(SPIX == SPI1)
		{
			NVIC_IRQ35_SPI1_Enable ;
		}
		else if (SPIX == SPI2)
		{
			NVIC_IRQ36_SPI2_Enable ;
		}
	}

	//write on the actual registers
	SPIX->CR1 = Tempreg_CR1 ;
	SPIX->CR2 = Tempreg_CR2 ;

}





/**==============================================================================
 * @fn				-MCAL_SPI_DEInit
 * @brief			-DEinitialize SPI peripheral to default
 * @param [in]		-SPIX : where x can be (1 , 2) to select SPI Peripheral
 * @retval			-none
 * @note			-none
 *================================================================================*/
void MCAL_SPI_DEInit(SPI_TypeDef* SPIX)
{
	if (SPIX == SPI1)
	{
		NVIC_IRQ35_SPI1_Disable ;
		RCC_SPI1_RESET() ;
	}
	else if (SPIX == SPI2)
	{
		NVIC_IRQ36_SPI2_Disable ;
		RCC_SPI2_RESET() ;
	}
}





/**==============================================================================
 * @fn				-MCAL_SPI_Send_Data
 * @brief			-send Data using SPI Protocol
 * @param [in]		-SPIX : where x can be (1 , 2) to select SPI Peripheral
 * @param [in]		-PTxPuffer is a pinter which point to the buffer which will be sent
 * @param [in]		-PollingEn : enum which enable/Disable polling mechanism
 * @retval			-none
 * @note			-none
 *================================================================================*/
void MCAL_SPI_Send_Data(SPI_TypeDef* SPIX , uint16_t* PTxPuffer , enum Polling_Mechanism PollingEn)
{
	if (PollingEn == enable_SPI) //check if polling mechanism is enable or not
		while(!( (SPIX->SR) & SPI_SR_TXE )) ; // wait until TXE Flag become 1(transmit buffer is empty)

	SPIX->DR = *PTxPuffer ;

}





/**==============================================================================
 * @fn				-MCAL_SPI_Receive_Data
 * @brief			-Receive Data using SPI Protocol
 * @param [in]		-SPIX : where x can be (1 , 2) to select SPI Peripheral
 * @param [in]		-PTxPuffer is a pinter which point to the buffer which will be sent
 * @param [in]		-PollingEn : enum which enable/Disable polling mechanism
 * @retval			-none
 * @note			-none
 *================================================================================*/
void MCAL_SPI_Receive_Data(SPI_TypeDef* SPIX , uint16_t* PTxPuffer , enum Polling_Mechanism PollingEn)
{
	if (PollingEn == enable_SPI) //check if polling mechanism is enable or not
		while(!( (SPIX->SR) & SPI_SR_RXNE )) ; // wait until RXNE Flag become 1(Receive buffer is not empty)

	*PTxPuffer = SPIX->DR ;
}





/**==============================================================================
 * @fn				-MCAL_SPI_TX_RX
 * @brief			-Send and Receive Data using SPI Protocol
 * @param [in]		-SPIX : where x can be (1 , 2) to select SPI Peripheral
 * @param [in]		-PTxPuffer is a pinter which point to the buffer which will be sent
 * @param [in]		-PollingEn : enum which enable/Disable polling mechanism
 * @retval			-none
 * @note			-none
 *================================================================================*/
void MCAL_SPI_TX_RX(SPI_TypeDef* SPIX , uint16_t* PTxPuffer , enum Polling_Mechanism PollingEn)
{
	if (PollingEn == enable_SPI) //check if polling mechanism is enable or not
		while(!( (SPIX->SR) & SPI_SR_TXE )) ; // wait until TXE Flag become 1(transmit buffer is empty)
	SPIX->DR = *PTxPuffer ;

	if (PollingEn == enable_SPI) //check if polling mechanism is enable or not
		while(!( (SPIX->SR) & SPI_SR_RXNE )) ; // wait until RXNE Flag become 1(Receive buffer is not empty)
	*PTxPuffer = SPIX->DR ;

}







/**==============================================================================
 * @fn				-MCAL_SPI_GPIO_Set_Pins
 * @brief			-Set Alternative GPIO Pins
 * @param [in]		-SPIX : where x can be (1 , 2) to select SPI Peripheral
 * @retval			-none
 * @note			-none
 *================================================================================*/
void MCAL_SPI_GPIO_Set_Pins(SPI_TypeDef* SPIX)
{
	GPIO_PinConfig_t PinCnfg ;
	if (SPIX == SPI1)
	{
		//PA4 : NSS
		//PA5 : SCLK
		//PA6 : MISO
		//PA7 : MOSI
		if(Global_SPI_Config[SPI1_Index]->Device_Mode == SPI_Device_Mode_Master) //Master
		{
			//PA4 : NSS
			switch(Global_SPI_Config[SPI1_Index]->NSS)
			{
			case SPI_NSS_Hard_Master_Output_Disable:
				//Hardware master /slave , input floating
				PinCnfg.GPIO_PinNumber = GPIO_PIN_4 ;
				PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
				MCAL_GPIO_Init(GPIOA, &PinCnfg) ;
				break ;

			case SPI_NSS_Hard_Master_Output_Enable :
				//Hardware master/ NSS output enabled , alternative function push pull
				PinCnfg.GPIO_PinNumber = GPIO_PIN_4 ;
				PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
				PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
				MCAL_GPIO_Init(GPIOA , &PinCnfg) ;
				break ;
			}
			//PA5 : SCLK
			PinCnfg.GPIO_PinNumber = GPIO_PIN_5 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
			PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
			MCAL_GPIO_Init(GPIOA , &PinCnfg) ;

			//PA6 : MISO
			PinCnfg.GPIO_PinNumber = GPIO_PIN_6 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
			MCAL_GPIO_Init(GPIOA, &PinCnfg) ;

			//PA7 : MOSI
			PinCnfg.GPIO_PinNumber = GPIO_PIN_7 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
			PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
			MCAL_GPIO_Init(GPIOA , &PinCnfg) ;
		}
		else //Slave
		{
			if(Global_SPI_Config[SPI1_Index]->NSS == SPI_NSS_Hard_Slave)
			{
				//Hardware master /slave , input floating , PA4
				PinCnfg.GPIO_PinNumber = GPIO_PIN_4 ;
				PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
				MCAL_GPIO_Init(GPIOA, &PinCnfg) ;
			}

			//PA5
			PinCnfg.GPIO_PinNumber = GPIO_PIN_5 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
			MCAL_GPIO_Init(GPIOA, &PinCnfg) ;

			//PA6
			PinCnfg.GPIO_PinNumber = GPIO_PIN_6 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
			PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
			MCAL_GPIO_Init(GPIOA , &PinCnfg) ;

			//PA7
			PinCnfg.GPIO_PinNumber = GPIO_PIN_7 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
			MCAL_GPIO_Init(GPIOA, &PinCnfg) ;
		}
	}
	if(SPIX == SPI2)
	{
		//PB12 : NSS
		//PB13 : SCLK
		//PB14 : MISO
		//PB15 : MOSI
		if(Global_SPI_Config[SPI2_Index]->Device_Mode == SPI_Device_Mode_Master) //Master
		{
			//PB12 : NSS
			switch(Global_SPI_Config[SPI2_Index]->NSS)
			{
			case SPI_NSS_Hard_Master_Output_Disable :
				//Hardware master /slave , input floating
				PinCnfg.GPIO_PinNumber = GPIO_PIN_12 ;
				PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
				MCAL_GPIO_Init(GPIOB, &PinCnfg) ;
				break ;

			case SPI_NSS_Hard_Master_Output_Enable :
				//Hardware master/ NSS output enabled , alternative function push pull
				PinCnfg.GPIO_PinNumber = GPIO_PIN_12 ;
				PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
				PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
				MCAL_GPIO_Init(GPIOB , &PinCnfg) ;
				break ;
			}
			//PB13 : SCLK
			PinCnfg.GPIO_PinNumber = GPIO_PIN_13 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
			PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
			MCAL_GPIO_Init(GPIOB , &PinCnfg) ;

			//PB14 : MISO
			PinCnfg.GPIO_PinNumber = GPIO_PIN_14 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
			MCAL_GPIO_Init(GPIOB, &PinCnfg) ;

			//PB15 : MOSI
			PinCnfg.GPIO_PinNumber = GPIO_PIN_15 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
			PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
			MCAL_GPIO_Init(GPIOB , &PinCnfg) ;
		}
		else //Slave
		{
			if(Global_SPI_Config[SPI2_Index]->NSS == SPI_NSS_Hard_Slave)
			{
				//Hardware master /slave , input floating , PB12
				PinCnfg.GPIO_PinNumber = GPIO_PIN_12 ;
				PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
				MCAL_GPIO_Init(GPIOB, &PinCnfg) ;
			}

			//PB13
			PinCnfg.GPIO_PinNumber = GPIO_PIN_13 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
			MCAL_GPIO_Init(GPIOB, &PinCnfg) ;

			//PB14
			PinCnfg.GPIO_PinNumber = GPIO_PIN_14 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP ;
			PinCnfg.GPIO_Output_Speed = GPIO_SPEED_10M ;
			MCAL_GPIO_Init(GPIOB , &PinCnfg) ;

			//PB15
			PinCnfg.GPIO_PinNumber = GPIO_PIN_15 ;
			PinCnfg.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
			MCAL_GPIO_Init(GPIOB, &PinCnfg) ;
		}
	}
}






/*===========================================================
 *                      IRQ HANDLER
 *===========================================================
 */

void SPI1_IRQHandler(void)
{
	struct S_IRQ_SRC irq_src ;
	irq_src.TXE = ((SPI1->SR) & (1<<1) >> 1) ;
	irq_src.RXNE = ((SPI1->SR) & (1<<0) >> 0) ;
	irq_src.ERRT = ((SPI1->SR) & (1<<4) >> 4) ;
	Global_SPI_Config[SPI1_Index]->P_IRQ_CallBack(irq_src) ;

}

void SPI2_IRQHandler(void)
{
	struct S_IRQ_SRC irq_src ;
	irq_src.TXE = ((SPI2->SR) & (1<<1) >> 1) ;
	irq_src.RXNE = ((SPI2->SR) & (1<<0) >> 0) ;
	irq_src.ERRT = ((SPI2->SR) & (1<<4) >> 4) ;
	Global_SPI_Config[SPI2_Index]->P_IRQ_CallBack(irq_src) ;

}

