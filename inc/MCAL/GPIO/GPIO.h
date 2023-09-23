/*
 * GPIO.h
 *
 *  Created on: Sep 21, 2023
 *      Author: salah
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

//includes
#include "stm32f103x6.h"

//......................................


//config structure
typedef struct {
	uint16_t GPIO_PinNumber ; //specifies the GPIO pin to be configured
						//this parameter must be based on @ref GPIO_PIN define

	uint8_t  GPIO_MODE ;	//specifies the GPIO pin to be configured
						// this parameter must be based on @ref GPIO_MODE define

	uint8_t GPIO_Output_Speed ; //specifies the GPIO pin to be configured
							// this parameter must be based on @ref GPIO_Speed define

}GPIO_PinConfig_t;


//@ref GPIO_PIN_STATE
#define GPIO_PIN_SET		1
#define GPIO_PIN_RESET		0


//@ref GPIO_RETURN_LOCK
#define GPIO_RETURN_LOCK_Enabled		1
#define GPIO_RETURN_LOCK_Error			0


//=======================================
//macros configuration references
//=======================================

//@ref GPIO_pin define

#define GPIO_PIN_0						((uint16_t)0x0001)  /* pin 0 is selected */
#define GPIO_PIN_1						((uint16_t)0x0002)  /* pin 1 is selected */
#define GPIO_PIN_2						((uint16_t)0x0004)  /* pin 2 is selected */
#define GPIO_PIN_3						((uint16_t)0x0008)  /* pin 3 is selected */
#define GPIO_PIN_4						((uint16_t)0x0010)  /* pin 4 is selected */
#define GPIO_PIN_5						((uint16_t)0x0020)  /* pin 5 is selected */
#define GPIO_PIN_6						((uint16_t)0x0040)  /* pin 6 is selected */
#define GPIO_PIN_7						((uint16_t)0x0080)  /* pin 7 is selected */
#define GPIO_PIN_8						((uint16_t)0x0100)  /* pin 8 is selected */
#define GPIO_PIN_9						((uint16_t)0x0200)  /* pin 9 is selected */
#define GPIO_PIN_10						((uint16_t)0x0400)  /* pin 10 is selected */
#define GPIO_PIN_11						((uint16_t)0x0800)  /* pin 11 is selected */
#define GPIO_PIN_12						((uint16_t)0x1000)  /* pin 12 is selected */
#define GPIO_PIN_13						((uint16_t)0x2000)  /* pin 13 is selected */
#define GPIO_PIN_14						((uint16_t)0x4000)  /* pin 14 is selected */
#define GPIO_PIN_15						((uint16_t)0x8000)  /* pin 15 is selected */
#define GPIO_PIN_ALL					((uint16_t)0xffff)  /* ALL pins are selected */


#define GPIO_PIN_MASK					0x0000ffffu /* pin mask for assert test */


//@ref GPIO_MODE define
#define GPIO_MODE_Analog						0x00000000u    //Analog input
#define GPIO_MODE_INPUT_FLO						0x00000001u    //floating input
#define GPIO_MODE_INPUT_PU						0x00000002u    //input with pull up
#define GPIO_MODE_INPUT_PD						0x00000003u    //input with pull down
#define GPIO_MODE_OUTPUT_PP						0x00000004u    //general purpose output push pull
#define GPIO_MODE_OUTPUT_OD						0x00000005u    //general purpose output open drain
#define GPIO_MODE_OUTPUT_AF_PP					0x00000006u    //alternate function output push pull
#define GPIO_MODE_OUTPUT_AF_OD					0x00000007u    //alternate function output open drain
#define GPIO_MODE_AF_INPUT						0x00000008u    //alternate function input


//@ref GPIO_Speed define
#define GPIO_SPEED_10M  						0x00000001u	   //output mode , max speed 10 MHZ
#define GPIO_SPEED_2M  							0x00000002u	   //output mode , max speed 2M MHZ
#define GPIO_SPEED_50M  						0x00000003u	   //output mode , max speed 50 MHZ


/*
 ===============================================================
 *			APIS Supported by "MCAL GPIO DRIVER"
 *==============================================================
 */
void MCAL_GPIO_Init(GPIO_TypeDef *GPIOX , GPIO_PinConfig_t *PinConfig );
void MCAL_GPIO_DeInit(GPIO_TypeDef *GPIOX);
uint8_t	MCAL_GPIO_ReadPin(GPIO_TypeDef *GPIOX , uint16_t PinNumber);
uint16_t MCAL_GPIO_ReadPort(GPIO_TypeDef *GPIOX);
void MCAL_GPIO_WritePin(GPIO_TypeDef *GPIOX , uint16_t PinNumber , uint8_t value);
void MCAL_GPIO_WritePort(GPIO_TypeDef *GPIOX , uint16_t value);
void MCAL_GPIO_TogglePin(GPIO_TypeDef *GPIOX , uint16_t PinNumber);
uint8_t MCAL_GPIO_LockPin(GPIO_TypeDef *GPIOX , uint16_t PinNumber);
#endif /* INC_GPIO_H_ */
