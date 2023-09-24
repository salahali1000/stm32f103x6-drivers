/*
 * KEYPAD.c
 *
 * Created: 3/13/2023 4:35:26 PM
 *  Author: salah
 */ 
#include "KEYPAD.h"
uint32_t keypad_row[4] = {GPIO_PIN_0,GPIO_PIN_1,GPIO_PIN_3,GPIO_PIN_4} ;
uint32_t keypad_coulmn[4] = {GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_7,GPIO_PIN_8} ;
void KEYPAD_VINIT(void)
{
	//pin 0
	PinConfig.GPIO_PinNumber = GPIO_PIN_0 ;
	PinConfig.GPIO_MODE = GPIO_MODE_OUTPUT_PP ;
	PinConfig.GPIO_Output_Speed = GPIO_SPEED_10M ;
	MCAL_GPIO_Init(keypad_port, &PinConfig) ;

	//pin1
	PinConfig.GPIO_PinNumber = GPIO_PIN_1 ;
	PinConfig.GPIO_MODE = GPIO_MODE_OUTPUT_PP ;
	PinConfig.GPIO_Output_Speed = GPIO_SPEED_10M ;
	MCAL_GPIO_Init(keypad_port, &PinConfig) ;

	//pin2
	PinConfig.GPIO_PinNumber = GPIO_PIN_3 ;
	PinConfig.GPIO_MODE = GPIO_MODE_OUTPUT_PP ;
	PinConfig.GPIO_Output_Speed = GPIO_SPEED_10M ;
	MCAL_GPIO_Init(keypad_port, &PinConfig) ;

	//pin4
	PinConfig.GPIO_PinNumber = GPIO_PIN_4 ;
	PinConfig.GPIO_MODE = GPIO_MODE_OUTPUT_PP ;
	PinConfig.GPIO_Output_Speed = GPIO_SPEED_10M ;
	MCAL_GPIO_Init(keypad_port, &PinConfig) ;

	//pin5
	PinConfig.GPIO_PinNumber = GPIO_PIN_5 ;
	PinConfig.GPIO_MODE = GPIO_MODE_INPUT_PU ;
	MCAL_GPIO_Init(keypad_port, &PinConfig) ;

	//pin6
	PinConfig.GPIO_PinNumber = GPIO_PIN_6 ;
	PinConfig.GPIO_MODE = GPIO_MODE_INPUT_PU ;
	MCAL_GPIO_Init(keypad_port, &PinConfig) ;

	//pin7
	PinConfig.GPIO_PinNumber = GPIO_PIN_7 ;
	PinConfig.GPIO_MODE = GPIO_MODE_INPUT_PU ;
	MCAL_GPIO_Init(keypad_port, &PinConfig) ;

	//pin8
	PinConfig.GPIO_PinNumber = GPIO_PIN_8 ;
	PinConfig.GPIO_MODE = GPIO_MODE_INPUT_PU ;
	MCAL_GPIO_Init(keypad_port, &PinConfig) ;
}
uint8_t KEYPAD_VREAD(void)
{
	uint8_t row_count,coulmn_count,check,ret=NOT_PRESSED;
	uint8_t keypad_value [4][4]={{'7','8','9','/'},{'4','5','6','*'},{'1','2','3','-'},{'A','0','=','+'}};

	for(row_count=0;row_count<row_size;row_count++)
	{
		MCAL_GPIO_WritePin(keypad_port, keypad_row[0], GPIO_PIN_SET) ;
		MCAL_GPIO_WritePin(keypad_port, keypad_row[1], GPIO_PIN_SET) ;
		MCAL_GPIO_WritePin(keypad_port, keypad_row[2], GPIO_PIN_SET) ;
		MCAL_GPIO_WritePin(keypad_port, keypad_row[3], GPIO_PIN_SET) ;
		MCAL_GPIO_WritePin(keypad_port, keypad_row[row_count], GPIO_PIN_RESET) ;
		for(coulmn_count=0;coulmn_count<coulmn_size;coulmn_count++)
		{
			check=MCAL_GPIO_ReadPin(keypad_port, keypad_coulmn[coulmn_count]);
			if(check==GPIO_PIN_RESET)
			{
				ret=keypad_value[row_count][coulmn_count];
				break;
			}
		}
		if(check==GPIO_PIN_RESET)
		{
			break;
		}
	}
	return ret;
}
