/*
 * EEPROM.h
 *
 *  Created on: Oct 26, 2023
 *      Author: salah
 */

#ifndef EEPROM_H_
#define EEPROM_H_
#include "I2C.h"

//EEPROM Act as a slave transmitter or slave receiver
//MCU Act As master transmitter or Master receiver

#define EEPROM_Slave_Address			0x2A

void EEPROM_Init(void) ;
uint8_t EEPROM_Write_Nbyte(uint32_t Memory_Address , uint8_t* Bytes , uint8_t Data_Len) ;
uint8_t EEPROM_Read_byte(uint32_t Memory_Address , uint8_t* Bytes , uint8_t Data_Len) ;



#endif /* EEPROM_H_ */
