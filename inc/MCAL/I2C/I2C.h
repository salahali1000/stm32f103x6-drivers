/*
 * I2C.h
 *
 *  Created on: Oct 23, 2023
 *      Author: salah
 */

#ifndef I2C_H_
#define I2C_H_
#include "stm32f103x6.h"
#include "RCC.h"
#include "GPIO.h"


struct S_I2C_Slave_Device_Address {
	uint16_t 		Enable_DUAL_ADD ; //enable : 1 , disable : 0
	uint16_t 		Primary_Address ;
	uint16_t 		Secondary_Address ;
	uint32_t 		I2C_Adressing_Slave_Mode ; //@ref I2C_Addressing_Slave_Mode
};
typedef enum {
	I2C_SLAVE_STOP,
	I2C_ERROR_AF,
	I2C_EV_ADDR_MATCHED,
	I2C_EV_DATA_REQ, //the APP layer should send data
	I2C_EV_DATA_RCV  //the APP layer should read data

}Slave_State;

typedef struct {
	uint32_t              Clock_Speed ;    //specifies the clock frequency
	//this parameter must be set to @ref I2C_SCLK

	uint32_t		      Stretch_Mode ;   //Enable Clock Stretching clock or Not
	//this parameter must be set to @ref I2C_Stretch_Mode

	uint32_t			  I2C_Mode ;	   //specifies the peripheral Mode
	//this parameter must be set to @ref I2C_Mode

	struct S_I2C_Slave_Device_Address  I2C_Slave_Address ;

	uint32_t 			 I2C_ACL_Control ; //Enable or Disable ACK
	//this parameter must be set to @ref I2C_ACL_Control

	uint32_t			I2C_General_Call ; //Enable or Disable General call
	//this parameter must be set to @ref I2C_General_Call

	void (* P_Slave_Event_CallBack)(Slave_State State) ;
}I2C_Config_t;


/*
 *=============================================================
 * 					reference macros
 *=============================================================
 */

//@ref I2C_SCLK
//standard speed(up to 100KHZ)
#define I2C_SCLK_SM_50K					(50000U)
#define I2C_SCLK_SM_100K				(100000U)
#define I2C_SCLK_FM_200K				(200000U) //fast mode not supported yet
#define I2C_SCLK_FM_400K				(400000U)


//@ref I2C_Stretch_Mode
#define I2C_Stretch_Mode_Enable					0x00000000U
#define I2C_Stretch_Mode_Disable				I2C_CR1_NOSTRETCH


//@ref I2C_Mode
#define I2C_Mode_I2C							0x00000000U
#define I2C_Mode_SMBus							I2C_CR1_SMBUS


//@ref I2C_Addressing_Slave_Mode
#define I2C_Addressing_Slave_Mode_7B			0x00000000U
#define I2C_Addressing_Slave_Mode_10B			(uint16_t)(1<<15)

//@ref I2C_ACL_Control
#define I2C_ACL_Control_Disable					0x00000000U
#define I2C_ACL_Control_Enable					I2C_CR1_ACK

//@ref I2C_General_Call
#define I2C_General_Call_Enable					I2C_CR1_ENGC
#define I2C_General_Call_Disable				0x00000000U



#define I2C_MASTER_EVENT_TRANSMIT_BYTE					( (uint32_t)0x00070080 ) //check TRA,MSL,BUSY AND TXE Flags.


typedef enum  {
	with_stop,
	without_stop
}Stop_Condition;

typedef enum  {
	start,
	repeated_Start
}Repeated_Start;

typedef enum  {
	Disable,
	Enable
}FunctionalState;


typedef enum  {
	Reset,
	Set
}flag_status;

typedef enum  {
	I2C_Direction_Transmitter,
	I2C_Direction_Receiver
}I2C_Direction;

typedef enum  {
	I2C_FLAG_BUSY = 0 ,
	EV5 , //EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	EV6 , //EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
	EV7 , //EV7 : RXNE=1 Cleared by reading DR Register
	EV8 , //EV8: TxE=1, shift register not empty, d. ata register empty, cleared by writing DR register
	MASTER_EVENT_TRANSMIT_BYTE =( (uint32_t)0x00070080 ) //check TRA,MSL,BUSY AND TXE Flags.

}Status;

/*=========================================================
 * 			APIS SUPPORTED BY "MCAL I2C DRIVER"
 *=========================================================
 */

void MCAL_I2C_Init(I2C_TypeDef* I2CX , I2C_Config_t* I2C_Config) ;
void MCAL_I2C_DEInit(I2C_TypeDef* I2CX ) ;
void MCAL_I2C_GPIO_Set_Pins(I2C_TypeDef* I2CX) ;

//Master polling mechanism
void MCAL_I2C_Master_TX(I2C_TypeDef* I2CX , uint16_t Dev_Add , uint8_t* DataOut , uint32_t DataLen,Stop_Condition stop ,Repeated_Start Start) ;
void MCAL_I2C_Master_RX(I2C_TypeDef* I2CX , uint16_t Dev_Add , uint8_t* DataOut , uint32_t DataLen,Stop_Condition stop ,Repeated_Start Start) ;

//Slave interrupt mechanism
void MCAL_I2C_SlaveSendData(I2C_TypeDef* I2CX , uint8_t DataOut) ;
uint8_t MCAL_I2C_SlaveSReceiveData(I2C_TypeDef* I2CX ) ;



//Generic APIS
void I2C_GenerateStart(I2C_TypeDef* I2CX , FunctionalState NewState , Repeated_Start Start) ;
flag_status I2C_GetFlagStatus(I2C_TypeDef* I2CX , Status flag) ;
void I2C_SetAddress(I2C_TypeDef* I2CX , uint16_t Address , I2C_Direction Direction) ;
void I2C_GenerateSTOP(I2C_TypeDef* I2CX , FunctionalState NewState) ;

#endif /* I2C_H_ */
