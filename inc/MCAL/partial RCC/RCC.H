/*
 * RCC.h
 *
 *  Created on: Oct 7, 2023
 *      Author: salah
 */

#ifndef RCC_H_
#define RCC_H_
#include "stm32f103x6.h"

#define HSI_RCC_CLOCK		 (uint32_t)8000000
#define HSE_RCC_CLOCK		 (uint32_t)16000000


uint32_t MCAL_RCC_GetSCLKFreq() ;
uint32_t MCAL_RCC_GetHCLKFreq() ;
uint32_t MCAL_RCC_GetPCLK1Freq() ;
uint32_t MCAL_RCC_GetPCLK2Freq() ;

#endif /* RCC_H_ */
