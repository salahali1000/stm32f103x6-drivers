/*
 * EEPROM.c
 *
 *  Created on: Oct 26, 2023
 *      Author: salah
 */
#include "EEPROM.h"
void EEPROM_Init(void)
{
	/*=================I2C INIT===================*/
	//PB6 : I2C1_SCL
	//PB7 : I2C1_SDA

	I2C_Config_t I2C1CFG ;
	I2C1CFG.I2C_General_Call = I2C_General_Call_Enable ;
	I2C1CFG.Clock_Speed = I2C_SCLK_SM_100K ;
	I2C1CFG.I2C_ACL_Control = I2C_ACL_Control_Enable ;
	I2C1CFG.I2C_Mode  = I2C_Mode_I2C ;
	I2C1CFG.Stretch_Mode = I2C_Stretch_Mode_Enable ;
	I2C1CFG.P_Slave_Event_CallBack = NULL ;
	MCAL_I2C_GPIO_Set_Pins(I2C1) ;
	MCAL_I2C_Init(I2C1, &I2C1CFG) ;

}


uint8_t EEPROM_Write_Nbyte(uint32_t Memory_Address , uint8_t* Bytes , uint8_t Data_Len)
{
	uint8_t i = 0;
	uint8_t buffer[256] ;
	buffer[0] = (uint8_t)(Memory_Address >> 8) ; //upper memory address
	buffer[1] = (uint8_t)(Memory_Address) ; //lower memory address

	for(i=2 ; i<(Data_Len+2) ; i++)
	{
		buffer[i] = Bytes[i-2] ;
	}
	MCAL_I2C_Master_TX(I2C1, EEPROM_Slave_Address, buffer, Data_Len+2, with_stop, start) ;
	return 0;
}


uint8_t EEPROM_Read_byte(uint32_t Memory_Address , uint8_t* Bytes , uint8_t Data_Len)
{
	uint8_t buffer[2] ;
	buffer[0] = (uint8_t)(Memory_Address >> 8) ; //upper memory address
	buffer[1] = (uint8_t)(Memory_Address) ; //lower memory address
	MCAL_I2C_Master_TX(I2C1, EEPROM_Slave_Address, buffer, 2, without_stop, start) ; //write address only
	MCAL_I2C_Master_RX(I2C1, EEPROM_Slave_Address, Bytes, Data_Len, with_stop, repeated_Start) ; //read data

return 0;
}
