/*
 * EXTI.c
 *
 *  Created on: Sep 25, 2023
 *      Author: salah
 */
#include "EXTI.h"

/*==============================================
 * 			GENERIC variables
 *===============================================
 */

void(*GP_IRQ_CallBck[16])(void) ;

/*==============================================
 * 			GENERIC MACRO
 *===============================================
 */

#define AFIO_GPIO_EXTI_Mapping(x)	(	(x==GPIOA)?0:\
		(x==GPIOB)?1:\
				(x==GPIOC)?2:\
						(x==GPIOD)?3:0   )




/*==============================================
 * 			GENERIC FUNCTION
 *===============================================
 */

void Enable_NVIC (uint16_t IRQ)
{
	switch(IRQ)
	{
	case EXTI0 :
		NVIC_IRQ6_EXTI0_Enable ;
		break ;
	case EXTI1 :
		NVIC_IRQ7_EXTI1_Enable ;
		break ;
	case EXTI2 :
		NVIC_IRQ8_EXTI2_Enable ;
		break ;
	case EXTI3 :
		NVIC_IRQ9_EXTI3_Enable ;
		break ;
	case EXTI4 :
		NVIC_IRQ10_EXTI4_Enable ;
		break ;
	case EXTI5 :
	case EXTI6 :
	case EXTI7 :
	case EXTI8 :
	case EXTI9 :
		NVIC_IRQ23_EXTI5_9_Enable ;
		break ;
	case EXTI10 :
	case EXTI11 :
	case EXTI12 :
	case EXTI13 :
	case EXTI14 :
	case EXTI15 :
		NVIC_IRQ40_EXTI10_15_Enable;
		break ;
	}
}

void Disable_NVIC (uint16_t IRQ)
{
	switch(IRQ)
	{
	case EXTI0 :
		NVIC_IRQ6_EXTI0_Disable ;
		break ;
	case EXTI1 :
		NVIC_IRQ7_EXTI1_Disable ;
		break ;
	case EXTI2 :
		NVIC_IRQ8_EXTI2_Disable ;
		break ;
	case EXTI3 :
		NVIC_IRQ9_EXTI3_Disable ;
		break ;
	case EXTI4 :
		NVIC_IRQ10_EXTI4_Disable ;
		break ;
	case EXTI5 :
	case EXTI6 :
	case EXTI7 :
	case EXTI8 :
	case EXTI9 :
		NVIC_IRQ23_EXTI5_9_Disable ;
		break ;
	case EXTI10 :
	case EXTI11 :
	case EXTI12 :
	case EXTI13 :
	case EXTI14 :
	case EXTI15 :
		NVIC_IRQ40_EXTI10_15_Disable;
		break ;
	}
}

void Update_EXTI(EXTI_PinConfig_t* EXTI_config)
{
	//1-configure Pin to be AF input (floating input)
	GPIO_PinConfig_t GPIO_Config ;
	GPIO_Config.GPIO_MODE = GPIO_MODE_INPUT_FLO ;
	GPIO_Config.GPIO_PinNumber = EXTI_config->EXTI_Pin.GPIO_Pin ;
	MCAL_GPIO_Init(EXTI_config->EXTI_Pin.GPIO_Port, &GPIO_Config ) ;
	//===================================================================================================================
	//2-Update AFIO to route between EXTI Lines to be Port A , B ,C , D
	uint8_t AFIO_EXTICR_Index = EXTI_config->EXTI_Pin.EXTI_InputLineNumber / 4 ;
	uint8_t AFIO_EXTICR_Position = (EXTI_config->EXTI_Pin.EXTI_InputLineNumber % 4 ) * 4 ;

	//clear The FOUR BitS
	AFIO->EXTICR[AFIO_EXTICR_Index] &= ~(0xf<<AFIO_EXTICR_Position) ;

	//set the four BITS according to GPIO Ports
	AFIO->EXTICR[AFIO_EXTICR_Index] |= ((AFIO_GPIO_EXTI_Mapping(EXTI_config->EXTI_Pin.GPIO_Port)&0x0f)<<AFIO_EXTICR_Position) ;
	//=====================================================================================================================
	//3-Update Rising and Falling Edge
	EXTI->RTSR &= ~(1<<EXTI_config->EXTI_Pin.EXTI_InputLineNumber) ;
	EXTI->FTSR &= ~(1<<EXTI_config->EXTI_Pin.EXTI_InputLineNumber) ;
	if(EXTI_config->Trigger_Case == EXTI_Trigger_RISING)
	{
		EXTI->RTSR |= (1<<EXTI_config->EXTI_Pin.EXTI_InputLineNumber) ;
	}
	else if (EXTI_config->Trigger_Case == EXTI_Trigger_FALLING)
	{
		EXTI->FTSR |= (1<<EXTI_config->EXTI_Pin.EXTI_InputLineNumber) ;
	}
	else if (EXTI_config->Trigger_Case == EXTI_Trigger_RisingAndFalling)
	{
		EXTI->RTSR |= (1<<EXTI_config->EXTI_Pin.EXTI_InputLineNumber) ;
		EXTI->FTSR |= (1<<EXTI_config->EXTI_Pin.EXTI_InputLineNumber) ;
	}
	//=====================================================================================================================
	//4-Update IRQ Handling CALLBACK
	GP_IRQ_CallBck[EXTI_config->EXTI_Pin.EXTI_InputLineNumber] = EXTI_config->P_IRQ_CallBck ;
	//=====================================================================================================================
	//5-Enable or Disable IRQ & NVIC
	if(EXTI_config->IRQ_EN == EXTI_IRQ_Enabled)
	{
		EXTI->IMR |= (1<<EXTI_config->EXTI_Pin.EXTI_InputLineNumber) ;
		Enable_NVIC(EXTI_config->EXTI_Pin.EXTI_InputLineNumber) ;
	}
	else
	{
		EXTI->IMR &= ~(1<<EXTI_config->EXTI_Pin.EXTI_InputLineNumber) ;
		Disable_NVIC(EXTI_config->EXTI_Pin.EXTI_InputLineNumber) ;
	}

}

//=================================================================



/*==============================================
 			APIS Function definitions
 *===============================================
 */



/**==============================================================================
 * @fn				-MCAL_EXTI_GPIO_Deinit
 * @brief			-Reset EXTI registers and IRQ corresponding IRQ Request
 * @retval			-none
 * @note			-none
 */
void MCAL_EXTI_GPIO_Deinit(void)
{
	EXTI->EMR   =   0x00000000 ;
	EXTI->RTSR  =   0x00000000 ;
	EXTI->FTSR  =   0x00000000 ;
	EXTI->SWIER =   0x00000000 ;
	//(rc-w) pin it is cleared by writing one to it .
	EXTI->PR    =   0xffffffff ;
	//Disable EXTI IRQ from NVIC
	NVIC_IRQ6_EXTI0_Disable ;
	NVIC_IRQ9_EXTI3_Disable ;
	NVIC_IRQ8_EXTI2_Disable ;
	NVIC_IRQ7_EXTI1_Disable ;
	NVIC_IRQ10_EXTI4_Disable ;
	NVIC_IRQ23_EXTI5_9_Disable ;
	NVIC_IRQ40_EXTI10_15_Disable ;
}





/**==============================================================================
 * @fn				-MCAL_EXTI_GPIO_Init
 * @brief			-this is used to initialize EXTI from specific GPIO Pin
 * @param [in]		-EXTI_config is set by EXTI_define , @EXTI_Trigger_define and @EXTI_IRQ_define
 * @retval			-none
 * @note			-you must set the clock for AFIO and GPIO
 */
void MCAL_EXTI_GPIO_Init(EXTI_PinConfig_t* EXTI_config)
{
	Update_EXTI(EXTI_config) ;
}




/**==============================================================================
 * @fn				-MCAL_EXTI_GPIO_Update
 * @brief			-this is used to initialize EXTI from specific GPIO Pin
 * @param [in]		-EXTI_config is set by EXTI_define , @EXTI_Trigger_define and @EXTI_IRQ_define
 * @retval			-none
 * @note			-you must set the clock for AFIO and GPIO
 */
void MCAL_EXTI_GPIO_Update(EXTI_PinConfig_t* EXTI_config)
{
	Update_EXTI(EXTI_config) ;
}







/*==============================================
 * 			ISR FUNCTIONS
 *===============================================
 */
void EXTI0_IRQHandler (void)
{
	//This bit is cleared by writing a ‘1’ into the bit in Pending register (EXTI_PR)
	EXTI->PR |= (1<<0) ;
	GP_IRQ_CallBck[0]() ;
}
void EXTI1_IRQHandler (void)
{
	//This bit is cleared by writing a ‘1’ into the bit in Pending register (EXTI_PR)
	EXTI->PR |= (1<<1) ;
	GP_IRQ_CallBck[1]() ;
}
void EXTI2_IRQHandler (void)
{
	//This bit is cleared by writing a ‘1’ into the bit in Pending register (EXTI_PR)
	EXTI->PR |= (1<<2) ;
	GP_IRQ_CallBck[2]() ;
}
void EXTI3_IRQHandler (void)
{
	//This bit is cleared by writing a ‘1’ into the bit in Pending register (EXTI_PR)
	EXTI->PR |= (1<<3) ;
	GP_IRQ_CallBck[3]() ;
}
void EXTI4_IRQHandler (void)
{
	//This bit is cleared by writing a ‘1’ into the bit in Pending register (EXTI_PR)
	EXTI->PR |= (1<<4) ;
	GP_IRQ_CallBck[4]() ;
}
void EXTI9_5_IRQHandler (void)
{
	//This bit is cleared by writing a ‘1’ into the bit in Pending register (EXTI_PR)
	if(EXTI->PR & (1<<5)) {EXTI->PR |= (1<<5)    ;   GP_IRQ_CallBck[5]() ;}
	if(EXTI->PR & (1<<6)) {EXTI->PR |= (1<<6)    ;   GP_IRQ_CallBck[6]() ;}
	if(EXTI->PR & (1<<7)) {EXTI->PR |= (1<<7)    ;   GP_IRQ_CallBck[7]() ;}
	if(EXTI->PR & (1<<8)) {EXTI->PR |= (1<<8)    ;   GP_IRQ_CallBck[8]() ;}
	if(EXTI->PR & (1<<9)) {EXTI->PR |= (1<<9)    ;   GP_IRQ_CallBck[9]() ;}

}
void EXTI15_10_IRQHandler (void)
{
	if(EXTI->PR & (1<<10)) {EXTI->PR |= (1<<10)    ;   GP_IRQ_CallBck[10]() ;}
	if(EXTI->PR & (1<<11)) {EXTI->PR |= (1<<11)    ;   GP_IRQ_CallBck[11]() ;}
	if(EXTI->PR & (1<<12)) {EXTI->PR |= (1<<12)    ;   GP_IRQ_CallBck[12]() ;}
	if(EXTI->PR & (1<<13)) {EXTI->PR |= (1<<13)    ;   GP_IRQ_CallBck[13]() ;}
	if(EXTI->PR & (1<<14)) {EXTI->PR |= (1<<14)    ;   GP_IRQ_CallBck[14]() ;}
	if(EXTI->PR & (1<<15)) {EXTI->PR |= (1<<15)    ;   GP_IRQ_CallBck[15]() ;}

}


