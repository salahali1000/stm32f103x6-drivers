/*
 * GPIO.c
 *
 *  Created on: Sep 21, 2023
 *      Author: salah
 */

#include "GPIO.h"

uint8_t Get_CR_Position (uint16_t PinNumber)
{
	switch(PinNumber)
	{
	case GPIO_PIN_0 :
		return 0;
		break ;
	case GPIO_PIN_1 :
		return 4;
		break ;
	case GPIO_PIN_2 :
		return 8;
		break ;
	case GPIO_PIN_3 :
		return 12;
		break ;
	case GPIO_PIN_4 :
		return 16;
		break ;
	case GPIO_PIN_5 :
		return 20;
		break ;
	case GPIO_PIN_6 :
		return 24;
		break ;
	case GPIO_PIN_7 :
		return 28;
		break ;
	case GPIO_PIN_8 :
		return 0;
		break ;
	case GPIO_PIN_9 :
		return 4;
		break ;
	case GPIO_PIN_10 :
		return 8;
		break ;
	case GPIO_PIN_11 :
		return 12;
		break ;
	case GPIO_PIN_12 :
		return 16;
		break ;
	case GPIO_PIN_13 :
		return 20;
		break ;
	case GPIO_PIN_14 :
		return 24;
		break ;
	case GPIO_PIN_15 :
		return 28;
		break ;
	}
	return 0 ;
}

/**==============================================================================
 * @fn				-MCAL_GPIO_Init
 * @brief			-initializes the GPIOX PinY peripheral according to the specified parameter in PinConfig
 * @param [in]		-GPIOX : where x can be (A..E depending on device used) to select GPIO Peripheral
 * @param [in]		-PinConfig is a pinter to structure GPIO_PinConfig_t that contains the
 * 					  configuration of Pin (pin number , pin mode , output speed)
 * @retval			-none
 * @note			-stm32f103c6 MCU has GPIO A,B,C,D and E modules
 * 					 but LQFP48 has only GPIO A,B and part of C and D.
 */

void MCAL_GPIO_Init(GPIO_TypeDef *GPIOX , GPIO_PinConfig_t *PinConfig )
{
	//Port configuration register low (GPIOX_CRL) Configure pins from 0 >>> 7
	//Port configuration register high (GPIOX_CRا) Configure pins from 8 >>> 15
	vuint32 *ConfigRegister = 0 ;
	ConfigRegister = (PinConfig->GPIO_PinNumber < GPIO_PIN_8) ? &GPIOX->CRL : &GPIOX->CRH ;
	uint8_t Pin_Config = 0;
	//clear CNF[1:0]  MODE[1:0]
	(*ConfigRegister) &= ~(0xf << Get_CR_Position(PinConfig->GPIO_PinNumber)) ;

	//if pin is output
	if( (PinConfig->GPIO_MODE == GPIO_MODE_OUTPUT_AF_OD) || (PinConfig->GPIO_MODE == GPIO_MODE_OUTPUT_AF_PP) || (PinConfig->GPIO_MODE == GPIO_MODE_OUTPUT_OD) || (PinConfig->GPIO_MODE == GPIO_MODE_OUTPUT_PP) )
	{
		//set CNF[1:0]  MODE[1:0]
		Pin_Config = ((((PinConfig->GPIO_MODE - 4) << 2) |(PinConfig->GPIO_Output_Speed))& 0x0f ) ;
	}
	//if pin is input
	else //MODE = 00: Input mode (reset state)
	{
		if ( (PinConfig->GPIO_MODE == GPIO_MODE_INPUT_FLO) || (PinConfig->GPIO_MODE == GPIO_MODE_Analog) )
		{
			//set CNF[1:0]  MODE[1:0]00
			Pin_Config = (( ((PinConfig->GPIO_MODE) << 2) | 0x0 ) & 0x0f) ;
		}

		else if ((PinConfig->GPIO_MODE) == GPIO_MODE_AF_INPUT) //consider it as input floating
		{
			//set CNF[1:0]  MODE[1:0]00
			Pin_Config = (( ((GPIO_MODE_INPUT_FLO) << 2)| 0x0 )  & 0x0f) ;
		}
		else //pull up and pull down input
		{
			//set CNF[1:0]  MODE[1:0]00
			Pin_Config = (( ((GPIO_MODE_INPUT_PU) << 2)| 0x0 )  & 0x0f) ;
			if(PinConfig->GPIO_MODE == GPIO_MODE_INPUT_PU)
			{
				// set the pin in  (GPIOx_ODR) to select pull up
				GPIOX->ODR |= (PinConfig->GPIO_PinNumber) ;
			}
			else
			{
				// reset the pin in  (GPIOx_ODR) to select pull down
				GPIOX->ODR &= ~(PinConfig->GPIO_PinNumber) ;
			}
		}
	}
	//write on CRH or CRL
	(*ConfigRegister) |= ( (Pin_Config) <<  Get_CR_Position(PinConfig->GPIO_PinNumber) ) ;


}





/**==============================================================================
 * @fn				-MCAL_GPIO_DeInit
 * @brief			-Reset ALL GPIO Registers.
 * @param [in]		-GPIOX : where x can be (A..E depending on device used) to select GPIO Peripheral
 * @retval			-none
 * @note			-none
 */

void MCAL_GPIO_DeInit(GPIO_TypeDef *GPIOX)
{
	//GPIOX->CRL =  0x44444444 ;
	//GPIOX->CRH =  0x44444444 ;
	//GPIOX->IDR =  0x0000XXXX; (Read only , where X is don't care) ;
	//GPIOX->ODR =   0x00000000 ;
	//GPIOX->BSRR =  0x00000000 ;
	//GPIOX->BRR =   0x00000000 ;
	//GPIOX->LCKR =  0x00000000 ;

	//or you can reset the peripheral by reset controller
	//ABP2 peripheral reset controller (RCC_APB2RSTR)
	//set and clear by software
	if (GPIOX == GPIOA)
	{
		RCC->APB2RSTR |= (1<<2) ; //Bit 2 IOPARST: I/O port A reset
		RCC->APB2RSTR &= ~(1<<2) ;
	}
	else if(GPIOX == GPIOB)
	{
		RCC->APB2RSTR |= (1<<3) ;//Bit 3 IOPBRST: IO port B reset
		RCC->APB2RSTR &= ~(1<<3) ;
	}
	else if(GPIOX == GPIOC)
	{
		RCC->APB2RSTR |= (1<<4) ;//Bit 4 IOPCRST: IO port C reset
		RCC->APB2RSTR &= ~(1<<4) ;
	}
	else if(GPIOX == GPIOD)
	{
		RCC->APB2RSTR |= (1<<5) ;//Bit 5 IOPDRST: I/O port D reset
		RCC->APB2RSTR &= ~(1<<5) ;
	}
	else if(GPIOX == GPIOE)
	{
		RCC->APB2RSTR |= (1<<6) ;//Bit 6 IOPERST: I/O port E reset
		RCC->APB2RSTR &= ~(1<<6) ;
	}
}





/**==============================================================================
 * @fn				-MCAL_GPIO_ReadPin
 * @brief			-read specific pin
 * @param [in]		-GPIOX : where x can be (A..E depending on device used) to select GPIO Peripheral
 * @param [in]		-PinNumber :set the pin number according to @ref GPIO_pin define
 * @retval			-the input pin value(two values based on @ref GPIO_PIN_STATE)
 * @note			-none
 */
uint8_t	MCAL_GPIO_ReadPin(GPIO_TypeDef *GPIOX , uint16_t PinNumber)
{
	uint8_t PinStatus ;
	if( ((GPIOX->IDR) & PinNumber) == (uint32_t)GPIO_PIN_RESET) //check if pin equal zero
	{
		PinStatus = GPIO_PIN_RESET ;
	}
	else //check if pin equal one
	{
		PinStatus = GPIO_PIN_SET ;
	}
	return PinStatus ;

}





/**==============================================================================
 * @fn				-MCAL_GPIO_ReadPort
 * @brief			-read the input port value
 * @param [in]		-GPIOX : where x can be (A..E depending on device used) to select GPIO Peripheral
 * @retval			-the input port value
 * @note				-none
 */
uint16_t MCAL_GPIO_ReadPort(GPIO_TypeDef *GPIOX)
{
	uint16_t PortValue ;
	PortValue = (uint16_t)(GPIOX->IDR) ; //read IDR
	return PortValue ;
}





/**==============================================================================
 * @fn				-MCAL_GPIO_WritePin
 * @brief			-write on specific pin
 * @param [in]		-GPIOX : where x can be (A..E depending on device used) to select GPIO Peripheral
 * @param [in]		-PinNumber : set the pin number according to @ref GPIO_pin define
 * @param [in]		-value : Pin value
 * @retval			-none
 * @note			-none
 */
void MCAL_GPIO_WritePin(GPIO_TypeDef *GPIOX , uint16_t PinNumber , uint8_t value)
{
	if( value == (uint8_t)GPIO_PIN_RESET) //check the value to be set
	{
		GPIOX->ODR &= (uint32_t)(~(PinNumber)) ; // write zero on pin
	}
	else
	{
		GPIOX->ODR |= (uint32_t)(PinNumber) ; //write one on pin
	}
}





/**==============================================================================
 * @fn				-MCAL_GPIO_WritePort
 * @brief			-write on output port
 * @param [in]		-GPIOX : where x can be (A..E depending on device used) to select GPIO Peripheral
 * @param [in]		-value : port value
 * @retval			-none
 * @note				-none
 */
void MCAL_GPIO_WritePort(GPIO_TypeDef *GPIOX , uint16_t value)
{
	GPIOX->ODR =(uint32_t)value ;
}




/**==============================================================================
 * @fn				-MCAL_GPIO_TogglePin
 * @brief			-toggle specific pin
 * @param [in]		-GPIOX : where x can be (A..E depending on device used) to select GPIO Peripheral
 * @param [in]		-PinNumber : set the pin number according to @ref GPIO_pin define
 * @retval			-none
 * @note			-none
 */
void MCAL_GPIO_TogglePin(GPIO_TypeDef *GPIOX , uint16_t PinNumber)
{
	GPIOX->ODR ^= (uint32_t)(PinNumber) ;
}



/**==============================================================================
 * @fn				-MCAL_GPIO_LockPin
 * @brief			-lock specific pin according to locking mechanism
 * @param [in]		-GPIOX : where x can be (A..E depending on device used) to select GPIO Peripheral
 * @param [in]		-PinNumber : set the pin number according to @ref GPIO_pin define
 * @retval			-ok if pin config is locked or ERROR  if pin not locked (ok and error are defined @ref GPIO_RETURN_LOCK)
 * @note				-none
 */
uint8_t MCAL_GPIO_LockPin(GPIO_TypeDef *GPIOX , uint16_t PinNumber)
{
	//GPIOx_LCKR :This register is used to lock the configuration of the port bits when a correct write sequence
	//is applied to bit 16 (LCKK). The value of bits [15:0] is used to lock the configuration of the
	//GPIO. During the write sequence, the value of LCKR[15:0] must not change. When the
	//LOCK sequence has been applied on a port bit it is no longer possible to modify the value of
	//the port bit until the next reset
	vuint32 tmp = 1<<16 ;
	//set the LCKy
	tmp |= PinNumber ;
	//Write 1
	GPIOX->LCKR = tmp ;
	//Write 0
	GPIOX->LCKR = PinNumber ;
	//Write 1
	GPIOX->LCKR = tmp ;
	//Read 0
	tmp = GPIOX->LCKR ;
	//Read 1 (this read is optional but confirms that the lock is active)
	if ( (uint32_t)( GPIOX->LCKR & 1<<16 ))
	{
		return GPIO_RETURN_LOCK_Enabled ;
	}
	else
	{
		return GPIO_RETURN_LOCK_Error ;
	}
}
