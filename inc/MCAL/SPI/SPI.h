/*
 * SPI.h
 *
 *  Created on: Oct 12, 2023
 *      Author: salah
 */

#ifndef SPI_H_
#define SPI_H_
#include "GPIO.h"
#include "stm32f103x6.h"

struct S_IRQ_SRC {
	uint8_t TXE:1 ; 	//TX Buffer is empty
	uint8_t RXNE:1 ;	//RX Buffer is not empty
	uint8_t ERRT:1 ;	//Error interrupt
	uint8_t Reserved:5 ;
};

//configuration structure
typedef struct {
	uint16_t 	Device_Mode ;	    //specifies the SPI Mode @ref SPI_Device_Mode

	uint16_t  	Communication_Mode ;//specifies the bidirectional mode @ref SPI_Communication_Mode

	uint16_t 	Frame_Format ; 		//specifies the LSB or MSB first @ref SPI_Frame_Foramt

	uint16_t 	Data_Size ; 		//specifies Data size @ref SPI_Data_Size

	uint16_t	CLKPolarity ;       //specifies the Idle case of clock @ref SPI_CLKPolarity

	uint16_t	CLKPhase ;          //specifies the phase of the clock @ref SPI_CLKPhase

	uint16_t	NSS ;               //specifies the NSS is managed by Hardware or Software @ref SPI_NSS

	uint16_t    BaudRate_Scaler ;   //specifies the BaudRate prescaler ex(FCLK/2,FCLK/4,...)
									//based on @ref SPI_BaudRate_Scaler
									//you have to configure the RCC clock to enter the correct clock to BUS

	uint16_t	IRQ_Enable ;		//Enable or Disable interrupt @reg SPI_IRQ_Enable

	void(*P_IRQ_CallBack)(struct S_IRQ_SRC irq_src) ; //you should set C function() which will be called by ISR

}SPI_Config_t;

enum Polling_Mechanism {
	enable_SPI ,
	disable_SPI
};
//===================================================
//reference macros
//===================================================

//@ref SPI_Device_Mode
#define SPI_Device_Mode_Slave			(0x00000000U)
#define SPI_Device_Mode_Master			(0x1U<<2)  //CR1.Bit 2 MSTR: Master selection

//@ref SPI_Communication_Mode
#define SPI_DIRECTION_2LINES					(0x00000000U)
#define SPI_DIRECTION_2LINES_RXONLY				(0x1U<<10) //Bit 10 RXONLY: Receive only
#define SPI_DIRECTION_1LINE_Receive_only		(0x1U<<15) //Bit 15 BIDIMODE: Bidirectional data mode enable
#define SPI_DIRECTION_1LINE_Transmit_only		( (0x1U<<15) | (0x1U<<14) )

//@ref SPI_Data_Size
#define SPI_DataSize_8B							(0x00000000U)
#define SPI_DataSize_16B						(0x1U<<11) //CR1.Bit 11 DFF: Data frame format

//@ref SPI_Frame_Format
#define SPI_Frame_Format_MSB_FIRST				(0x00000000U)
#define SPI_Frame_Format_LSB_FIRST				(0x1U<<7) //CR1.Bit 7 LSBFIRST: Frame format

//@ref SPI_CLKPolarity
#define SPI_CLKPolarity_IDLE0					(0x00000000U)
#define SPI_CLKPolarity_IDLE1					(0x1U<<1) //CR1.Bit1 CPOL: Clock polarity

//@ref SPI_CLKPhase
#define SPI_CLKPhase_1EDGE_FIRST				(0x00000000U)
#define SPI_CLKPhase_2EDGE_FIRST				(0x1U<<0) //CR1.Bit 0 CPHA: Clock phase

//@ref SPI_NSS
//Hardware
#define SPI_NSS_Hard_Slave									(0x00000000U)
#define SPI_NSS_Hard_Master_Output_Enable					(0x1U<<2) //CR2.Bit 2 SSOE: SS output enable
#define SPI_NSS_Hard_Master_Output_Disable					~(0x1U<<2)

//NSS is driver by software (master or slave)
#define SPI_NSS_Soft_Reset									(0x1U<<9)	//Bit 9 SSM: Software slave management
#define SPI_NSS_Soft_Set									( (0x1U<<8) | (0x1U<<9) ) //Bit 8 SSI: Internal slave select&Bit 9 SSM: Software slave management


//@ref SPI_BaudRate_Scaler
#define SPI_BaudRate_Scaler_2					(0x00000000U)
#define SPI_BaudRate_Scaler_4					(0b001U<<3)
#define SPI_BaudRate_Scaler_8					(0b010U<<3)
#define SPI_BaudRate_Scaler_16					(0b011U<<3)
#define SPI_BaudRate_Scaler_32					(0b100U<<3)
#define SPI_BaudRate_Scaler_64					(0b101U<<3)
#define SPI_BaudRate_Scaler_128					(0b110U<<3)
#define SPI_BaudRate_Scaler_256					(0b111U<<3)

//@reg SPI_IRQ_Enable
#define SPI_IRQ_Enable_NONE						(0x00000000U)
#define SPI_IRQ_Enable_TXIE						(0x1U<<7) //Bit 7 TXEIE: Tx buffer empty interrupt enable
#define SPI_IRQ_Enable_RXNEIE					(0x1U<<6) //Bit 6 RXNEIE: RX buffer not empty interrupt enable
#define SPI_IRQ_Enable_ERRIE					(0x1U<<5) //Bit 5 ERRIE: Error interrupt enable


/*
 * =======================================================
 * 			APIS SUPPORTED BY "MCAL SPI DRIVER"
 * =======================================================
 */
void MCAL_SPI_Init(SPI_TypeDef* SPIX , SPI_Config_t* SPI_Config) ;
void MCAL_SPI_DEInit(SPI_TypeDef* SPIX) ;
void MCAL_SPI_GPIO_Set_Pins(SPI_TypeDef* SPIX) ;
void MCAL_SPI_Send_Data(SPI_TypeDef* SPIX , uint16_t* PTxPuffer , enum Polling_Mechanism PollingEn) ;
void MCAL_SPI_Receive_Data(SPI_TypeDef* SPIX , uint16_t* PTxPuffer , enum Polling_Mechanism PollingEn) ;
void MCAL_SPI_TX_RX(SPI_TypeDef* SPIX , uint16_t* PTxPuffer , enum Polling_Mechanism PollingEn) ;



#endif /* SPI_H_ */
