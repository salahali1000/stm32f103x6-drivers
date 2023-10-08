/*
 * RCC.c
 *
 *  Created on: Oct 7, 2023
 *      Author: salah
 */


#include "RCC.h"

const uint8_t APBrescTable[8U] = {0,0,0,0,1,2,3,4} ;
const uint8_t AHBrescTable[16U] = {0,0,0,0,0,0,0,0,1,2,3,4,6,7,8,9} ;
uint32_t MCAL_RCC_GetSCLKFreq()
{
	//Bits 3:2 SWS[1:0]: System clock switch status
	//Set and cleared by hardware to indicate which clock source is used as system clock.
	//00: HSI oscillator used as system clock
	//01: HSE oscillator used as system clock
	//10: PLL used as system clock
	//11: Not applicable
	switch( (RCC->CFGR>>2) & 0b11 )
	{
	case 0 :
		return HSI_RCC_CLOCK ;
		break ;
	case 1 :
		//TODO need to calculate it //you should specify HSE .
		return HSE_RCC_CLOCK ;
		break ;
	case 2 :
		return 16000000 ;
		break ;

	}

}
uint32_t MCAL_RCC_GetHCLKFreq()
{
	//Bits 7:4 HPRE[3:0]: AHB prescaler
	return (MCAL_RCC_GetSCLKFreq() >> AHBrescTable[ (RCC->CFGR>>4) & 0xf ]) ;
}
uint32_t MCAL_RCC_GetPCLK1Freq()
{
	//Bits 10:8 PPRE1[2:0]: APB Low-speed prescaler (APB1)
	return (MCAL_RCC_GetHCLKFreq() >> APBrescTable[ (RCC->CFGR>>8) & 0b111 ]) ;
}
uint32_t MCAL_RCC_GetPCLK2Freq()
{
	//Bits 13:11 PPRE2[2:0]: APB high-speed prescaler (APB2)
	return (MCAL_RCC_GetHCLKFreq() >> APBrescTable[ (RCC->CFGR>>11) & 0b111 ]) ;

}
