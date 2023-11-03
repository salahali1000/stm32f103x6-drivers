/*
 * I2C.c
 *
 *  Created on: Oct 23, 2023
 *      Author: salah
 */
#include "I2C.h"

/*====================================================
 * 				GENERIC VARIABLES
 *====================================================
 */
I2C_Config_t Global_I2C_Config[2] = {NULL , NULL} ;




/*====================================================
 * 				GENERIC MACROS
 *====================================================
 */
#define I2C1_INDEX			0
#define I2C2_INDEX			1



/*====================================================
 * 				GENERIC FUNCTIONS
 *====================================================
 */
void I2C_GenerateStart(I2C_TypeDef* I2CX , FunctionalState NewState , Repeated_Start Start)
{
	if(Start != repeated_Start)
	{
		//check if the bus is idle
		while(I2C_GetFlagStatus(I2CX , I2C_FLAG_BUSY)) ;

	}
	if(NewState != Disable)
	{
		I2CX->CR1 |= I2C_CR1_START ;
	}
	else
	{
		I2CX->CR1 &= ~(I2C_CR1_START) ;

	}
}

flag_status I2C_GetFlagStatus(I2C_TypeDef* I2CX , Status flag)
{
	vuint32 Dummy_Read ;
	flag_status bitStatus = Reset ;
	uint32_t flag1 = 0 ,flag2 = 0 ;
	uint32_t LastEvent = 0;
	switch(flag)
	{
	case I2C_FLAG_BUSY :
	{
		//0: No communication on the bus
		//1: Communication ongoing on the bus
		if( (I2CX->SR2) & (I2C_SR2_BUSY))
			bitStatus = Set ;
		else
			bitStatus = Reset ;
		break ;
	}
	case EV5 :
	{
		if( (I2CX->SR1) & (I2C_SR1_SB))
			bitStatus = Set ;
		else
			bitStatus = Reset ;
		break;
	}
	case EV6 :
	{
		if( (I2CX->SR1) & (I2C_SR1_ADDR))
			bitStatus = Set ;
		else
			bitStatus = Reset ;
		//to clear ADDR you must reading SR1 then SR2
		Dummy_Read = I2CX->SR2 ;
		break;
	}
	case MASTER_EVENT_TRANSMIT_BYTE :
	{
		flag1 = I2CX->SR1 ;
		flag2 = I2CX->SR2 ;
		flag2 = flag2<<16 ;
		LastEvent = (flag1 | flag2) & ((uint32_t)0x00ffffff) ;
		if( (LastEvent & flag )== flag)
		{
			bitStatus = Set ;
		}
		else
		{
			bitStatus = Reset ;
		}
		break;
	}
	case EV7 :
	{
		if( (I2CX->SR1) & (I2C_SR1_RXNE))
			bitStatus = Set ;
		else
			bitStatus = Reset ;
		break ;
	}
	case EV8 :
	{
		if( (I2CX->SR1) & (I2C_SR1_TXE))
			bitStatus = Set ;
		else
			bitStatus = Reset ;
		break ;
	}
	}

	return bitStatus ;
}

void I2C_SetAddress(I2C_TypeDef* I2CX , uint16_t Address , I2C_Direction Direction)
{
	Address = Address << 1 ;
	if(Direction != I2C_Direction_Transmitter)
	{
		//set first bit to select read mode
		Address |= (1<<0) ;
	}
	else
	{
		//Reset first bit to select write mode
		Address &= ~(1<<0) ;
	}
	I2CX->DR = Address ;
}


void I2C_GenerateSTOP(I2C_TypeDef* I2CX , FunctionalState NewState)
{
	if(NewState != Disable)
	{
		//Generate stop condition
		I2CX->CR1 |= (I2C_CR1_STOP) ;
	}
	else
	{
		//Disable stop condition
		I2CX->CR1 &= ~(I2C_CR1_STOP) ;
	}
}

void I2C_AclnowledgeConfig(I2C_TypeDef* I2CX , FunctionalState NewState)
{
	if(NewState != Disable)
	{
		//Generate ACK
		I2CX->CR1 |= (I2C_CR1_ACK) ;
	}
	else
	{
		//Generate NACK
		I2CX->CR1 &= ~(I2C_CR1_ACK) ;
	}
}

/*====================================================
 * 				APIS
 *====================================================
 */

/**==============================================================================
 * @fn				-MCAL_I2C_Init
 * @brief			-initialize I2C according to specific configuration
 * @param [in]		-I2CX : where x can be (1 , 2) to select SPI Peripheral
 * @param [in]		-I2C_Config is a pinter to structure I2C_Config_t that contains the
 * 					  configuration of I2C
 * @retval			-none
 * @note			-support SM only(standard mode).
 *================================================================================*/
void MCAL_I2C_Init(I2C_TypeDef* I2CX , I2C_Config_t* I2C_Config)
{
	uint16_t tempreg = 0,freqrange = 0 ;
	uint32_t PCLK1 = 8000000 ;
	uint16_t result = 0;
	//Enable RCC Clock
	if(I2CX == I2C1)
	{
		Global_I2C_Config[I2C1_INDEX] = *I2C_Config ;
		RCC_I2C1_CLK_EN() ;
	}
	else if(I2CX == I2C2)
	{
		Global_I2C_Config[I2C2_INDEX] = *I2C_Config ;
		RCC_I2C2_CLK_EN() ;
	}
	if(I2C_Config->I2C_Mode == I2C_Mode_I2C)
	{
		/*=============================Init Timing ============================*/
		//put frequency of APB1 BUS
		tempreg = I2CX->CR2 ;
		//clear frequency FREQ[5:0] BITS
		tempreg &= ~(I2C_CR2_FREQ) ;
		//Get PCLK1
		PCLK1 = MCAL_RCC_GetPCLK1Freq() ;
		freqrange = (uint16_t)(PCLK1/1000000) ;
		tempreg |= freqrange ;
		//write to I2CX CR2
		I2CX->CR2 = tempreg ;

		//configuration of clock control register
		//Disable the selected I2C peripheral to configure the time
		I2CX->CR1 &= ~(I2C_CR1_PE) ;
		tempreg = 0 ;
		if( (I2C_Config->Clock_Speed == I2C_SCLK_SM_50K) || (I2C_Config->Clock_Speed == I2C_SCLK_SM_100K))
		{
			/*=====standard mode speed calculates====*/
			//TCLK/2 = CRR *Tpclk1
			//CRR = (TCLK/2*Tpclk1)
			//CRR = (PCLK1/2*I2C_CLOCK_frequency)
			result = (uint16_t)(PCLK1 / ( I2C_Config->Clock_Speed << 1) ) ;
			tempreg |= result ;
			I2CX->CCR = tempreg ;

			/*.........................I2C TRISE CONFIGURATION.....................*/
			I2CX->TRISE = freqrange + 1 ;

		}
		else
		{
			//fast mode not supported
		}

		/*......................I2CX CR1 CONFIGURATION..........................*/
		tempreg = I2CX->CR1 ;
		tempreg |= (uint16_t)(I2C_Config->I2C_ACL_Control | I2C_Config->I2C_General_Call | I2C_Config->Stretch_Mode | I2C_Config->I2C_Mode ) ;
		//write to I2CX CR1
		I2CX->CR1 = tempreg ;

		/*......................I2CX ORA1 & ORA2 CONFIGURATION..........................*/
		tempreg = 0 ;
		if(I2C_Config->I2C_Slave_Address.Enable_DUAL_ADD == 1)
		{
			tempreg |= I2C_OAR2_ENDUAL ;
			tempreg |= (I2C_Config->I2C_Slave_Address.Secondary_Address ) << I2C_OAR2_ADD2_Pos ;
			I2CX->OAR2 = tempreg ;
		}
		tempreg = 0 ;
		tempreg |= (I2C_Config->I2C_Slave_Address.I2C_Adressing_Slave_Mode ) ;
		I2CX->OAR1 = tempreg ;


		//enable interrupt(slave mode), check callback if != NULL
		if(I2C_Config->P_Slave_Event_CallBack != NULL)
		{
			//Enable IRQ
			I2CX->CR2 |= I2C_CR2_ITERREN ;
			I2CX->CR2 |= I2C_CR2_ITEVTEN ;
			I2CX->CR2 |= I2C_CR2_ITBUFEN ;
			if(I2CX == I2C1)
			{
				NVIC_IRQ31_I2C1_EV_Enable ;
				NVIC_IRQ32_I2C1_ER_Enable ;
			}
			else if(I2CX == I2C2)
			{
				NVIC_IRQ33_I2C2_EV_Enable ;
				NVIC_IRQ34_I2C2_ER_Enable ;
			}
			I2CX->SR1 = 0 ;
			I2CX->SR2 = 0 ;
		}

	}
	//Enable the selected I2C Peripheral
	I2CX->CR1 |= I2C_CR1_PE ;

}





/**==============================================================================
 * @fn				-MCAL_I2C_DEInit
 * @brief			-DEinitialize I2C according to specific configuration
 * @param [in]		-I2CX : where x can be (1 , 2) to select SPI Peripheral
 * @retval			-none
 * @note			-none
 *================================================================================*/
void MCAL_I2C_DEInit(I2C_TypeDef* I2CX )
{
	if(I2CX == I2C1)
	{
		NVIC_IRQ31_I2C1_EV_Disable ;
		NVIC_IRQ32_I2C1_ER_Disable ;
		RCC_I2C1_RESET() ;
	}
	else if(I2CX == I2C2)
	{
		NVIC_IRQ33_I2C2_EV_Disable ;
		NVIC_IRQ34_I2C2_ER_Disable ;
		RCC_I2C2_RESET() ;
	}
}






/**==============================================================================
 * @fn				-MCAL_I2C_GPIO_Set_Pins
 * @brief			-Set GPIO Pins for I2C Peripheral
 * @param [in]		-I2CX : where x can be (1 , 2) to select SPI Peripheral
 * @retval			-none
 * @note			-none
 *================================================================================*/
void MCAL_I2C_GPIO_Set_Pins(I2C_TypeDef* I2CX)
{
	//I2C1 : PB6: SCL , PB7 : SDA (Alternative function open drain)
	//I2C2 : PB10: SCL , PB11 : SDA (Alternative function open drain)
	GPIO_PinConfig_t Pin_Confg ;
	if(I2CX == I2C1)
	{
		//PB6 AF Open Drain SCL
		Pin_Confg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD ;
		Pin_Confg.GPIO_Output_Speed = GPIO_SPEED_10M ;
		Pin_Confg.GPIO_PinNumber = GPIO_PIN_6 ;
		MCAL_GPIO_Init(GPIOB, &Pin_Confg) ;

		//PB7 AF Open Drain SDA
		Pin_Confg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD ;
		Pin_Confg.GPIO_Output_Speed = GPIO_SPEED_10M ;
		Pin_Confg.GPIO_PinNumber = GPIO_PIN_7 ;
		MCAL_GPIO_Init(GPIOB, &Pin_Confg) ;
	}
	else if(I2CX == I2C2)
	{
		//PB10 AF Open Drain SCL
		Pin_Confg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD ;
		Pin_Confg.GPIO_Output_Speed = GPIO_SPEED_10M ;
		Pin_Confg.GPIO_PinNumber = GPIO_PIN_10 ;
		MCAL_GPIO_Init(GPIOB, &Pin_Confg) ;

		//PB11 AF Open Drain SDA
		Pin_Confg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD ;
		Pin_Confg.GPIO_Output_Speed = GPIO_SPEED_10M ;
		Pin_Confg.GPIO_PinNumber = GPIO_PIN_11 ;
		MCAL_GPIO_Init(GPIOB, &Pin_Confg) ;
	}
}






/**==============================================================================
 * @fn				-MCAL_I2C_Master_TX
 * @brief			-send data from master to slave
 * @param [in]		-I2CX : where x can be (1 , 2) to select SPI Peripheral
 * @param [in]		-Dev_Add : slave address of device which master will send to it.
 * @param [in]		-DataOut : pointer to data buffer
 * @param [in]		-DataLen : data length
 * @param [in]		-stop : enum that allows to select sending stop condition or not
 * @param [in]		-Start : enum that allows to select sending repeated start or not
 * @retval			-none
 * @note			-none
 *================================================================================*/
void MCAL_I2C_Master_TX(I2C_TypeDef* I2CX , uint16_t Dev_Add , uint8_t* DataOut , uint32_t DataLen,Stop_Condition stop ,Repeated_Start Start)
{
	sint32_t i ;
	//send Start Bit by I2C_CR1 Register
	I2C_GenerateStart(I2CX, Enable,Start);

	//wait for EV5 (StartBit is one)
	while(!(I2C_GetFlagStatus(I2CX, EV5))) ;

	//send Address
	I2C_SetAddress(I2CX, Dev_Add, I2C_Direction_Transmitter) ;

	//wait for EV6 , EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
	while(!(I2C_GetFlagStatus(I2CX, EV6))) ;

	//check TRA,MSL,BUSY AND TXE Flags.
	while(!(I2C_GetFlagStatus(I2CX, MASTER_EVENT_TRANSMIT_BYTE))) ;

	//write on Data register
	for(i=0 ; i<DataLen ; i++)
	{
		I2CX->DR = DataOut[i] ;
		//wait for EV8 , EV8: TxE=1, shift register not empty, d. ata register empty, cleared by writing DR register
		while(!(I2C_GetFlagStatus(I2CX, EV8))) ;
	}
	if(stop == with_stop)
	{
		//send stop condition
		I2C_GenerateSTOP(I2CX, Enable) ;
	}
}

void MCAL_I2C_SlaveSendData(I2C_TypeDef* I2CX , uint8_t DataOut)
{
	I2CX->DR = DataOut ;
}


uint8_t MCAL_I2C_SlaveSReceiveData(I2C_TypeDef* I2CX )
{
	return (uint8_t) I2CX->DR ;
}






/**==============================================================================
 * @fn				-MCAL_I2C_Master_RX
 * @brief			-receive data from slave (you act as a Master)
 * @param [in]		-I2CX : where x can be (1 , 2) to select SPI Peripheral
 * @param [in]		-Dev_Add : slave address of device which master will send to it.
 * @param [in]		-DataOut : pointer to data buffer
 * @param [in]		-DataLen : data length
 * @param [in]		-stop : enum that allows to select sending stop condition or not
 * @param [in]		-Start : enum that allows to select sending repeated start or not
 * @retval			-none
 * @note			-none
 *================================================================================*/

void MCAL_I2C_Master_RX(I2C_TypeDef* I2CX , uint16_t Dev_Add , uint8_t* DataOut , uint32_t DataLen,Stop_Condition stop ,Repeated_Start Start)
{
	uint8_t index = I2CX == I2C1 ? I2C1_INDEX : I2C2_INDEX ;
	sint32_t i ;
	//send Start Bit by I2C_CR1 Register
	I2C_GenerateStart(I2CX, Enable,Start);

	//wait for EV5 (StartBit is one)
	while(!(I2C_GetFlagStatus(I2CX, EV5))) ;

	//send Address
	I2C_SetAddress(I2CX, Dev_Add, I2C_Direction_Receiver) ;

	//wait for EV6 , EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
	while(!(I2C_GetFlagStatus(I2CX, EV6))) ;
	I2C_AclnowledgeConfig(I2CX , Enable) ;

	if( DataLen )
	{
		//read data from Data register to the buffer
		for(i=DataLen ; i>1 ; i--)
		{
			//wait for EV7 , EV7 : RXNE=1 Cleared by reading DR Register
			while(!(I2C_GetFlagStatus(I2CX, EV7))) ;

			//read data
			*DataOut = I2CX->DR ;
			//increment the buffer
			DataOut++ ;
		}
		//reply with NACK
		I2C_AclnowledgeConfig(I2CX , Disable) ;
	}
	if(stop == with_stop)
	{
		//send stop condition
		I2C_GenerateSTOP(I2CX, Enable) ;
	}
	if(Global_I2C_Config[index].I2C_ACL_Control == I2C_ACL_Control_Enable)
	{
		I2C_AclnowledgeConfig(I2CX , Enable) ;
	}
}




