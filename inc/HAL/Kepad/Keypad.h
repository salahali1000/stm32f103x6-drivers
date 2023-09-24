*
 * KEYPAD.h
 *
 * Created: 3/13/2023 4:35:43 PM
 *  Author: salah
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "GPIO.h"
#include "stm32f103x6.h"
#include "STD_TYPES.h"

#define keypad_port  GPIOB
#define row_size      4
#define coulmn_size   4
#define NOT_PRESSED   0xff


GPIO_PinConfig_t PinConfig ;
void KEYPAD_VINIT(void);
uint8_t KEYPAD_VREAD(void);


#endif /* KEYPAD_H_ */
