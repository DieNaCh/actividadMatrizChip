/* ******************************************* START *********************************************** */
/* Libraries, Definitions and Global Declarations */
#include <stdint.h> // 							standard integer library
#include "main.h"
#include "user_keypad.h"

// Array of our 4 LEDs mapped to their physical hardware addresses
Pin LEDS[4] = {
    { GPIOB, (0x1UL << 11U) }, // LED 3
    { GPIOB, (0x1UL << 10U) }, // LED 2
    { GPIOB, (0x1UL <<  9U) }, // LED 1
    { GPIOB, (0x1UL <<  8U) }  // LED 0
};

/* Superloop structure */
int main(void)
{
	/* Declarations and Initializations */
	USER_SystemClock_Config( ); // 				configure the system clock to 64 MHz
	USER_Keypad_Init( ); //						initialize keypad rows as output and columns as input
	USER_GPIO_Init( );
    
	
	/* Repetitive block */
    for(;;){
		uint8_t key = USER_Key( );

		if (key != 0xFFU) {
			// Validate the press occurred (debouncing)
			USER_Delay_10ms();

			if (USER_Key() == key) {
				// Clear LEDs
				for (uint8_t i = 0; i < 4; i++) {
					LEDS[i].port->ODR &= ~LEDS[i].mask;
				}

				// Turn on corresponding LEDS
				for (uint8_t i = 0; i < 4; i++) {
					if (key & ( 1U << i )) {
						LEDS[i].port->ODR |= LEDS[i].mask;
					}
				}

				while (USER_Key() != 0xFFU); // Wait for release
				USER_Delay_10ms();
			}
		}
    }
}

void USER_GPIO_Init( void ){
	// RCC_APB2ENR modified to IO port A clock enable
	RCC->APB2ENR	= RCC->APB2ENR//			RCC_APB2ENR actual value
					|//							to set
					( 0x1UL << 2U );//			(mask) IOPAEN bit

	// GPIOx_ODR modified to reset pin 5 of port A (LD2 is connected to PA5)
	GPIOA->ODR		= GPIOA->ODR//				GPIOx_ODR actual value
					&//							to clear
					~( 0x1UL << 5U );//			(mask) ODR5 bit

	// GPIOx_CRL modified to configure pin5 as output
	GPIOA->CRL		=	GPIOA->CRL//			GPIOx_CRL actual value
					&//							to clear
					~( 0x3UL << 22U )//			(mask) CNF5[1:0] bits
					&//							to clear
					~( 0x2UL << 20U );//		(mask) MODE5_1 bit

	// GPIOx_CRL modified to select pin5 max speed of 10MHz
	GPIOA->CRL		=	GPIOA->CRL//			GPIOx_CRL actual value
					|//							to set
					( 0x1UL << 20U );//			(mask) MODE5_0 bit
	
	// LEDs, from most significant bit to least significant bit
	// As push-pull output
	// CNF[1:0] = 00
    // MODE[1:0] = 01
    // ODR = 0 or 1

	// PB8
	GPIOB->ODR &=		~( 0x1UL <<  8U );
	GPIOB->CRH &= 		~( 0x3UL <<  2U )
				&		~( 0x2UL <<  0U );
	GPIOB->CRH |=		 ( 0x1UL <<  0U );

	// PB9
	GPIOB->ODR &=		~( 0x1UL <<  9U );
	GPIOB->CRH &= 		~( 0x3UL <<  6U )
				&		~( 0x2UL <<  4U );
	GPIOB->CRH |=		 ( 0x1UL <<  4U );

	// PB10
	GPIOB->ODR &=		~( 0x1UL << 10U );
	GPIOB->CRH &= 		~( 0x3UL << 10U )
				&		~( 0x2UL <<  8U );
	GPIOB->CRH |=		 ( 0x1UL <<  8U );

	// PB11
	GPIOB->ODR &=		~( 0x1UL << 11U );
	GPIOB->CRH &= 		~( 0x3UL << 14U )
				&		~( 0x2UL << 12U );
	GPIOB->CRH |=		 ( 0x1UL << 12U );
}

void USER_SystemClock_Config( void ){
	FLASH->ACR	&=	~( 0x5UL <<  0U );//		two wait states latency, if SYSCLK > 48MHz
	FLASH->ACR	|=	 ( 0x2UL <<  0U );//		two wait states latency, if SYSCLK > 48MHz
	RCC->CFGR	&=	~( 0x1UL << 16U )//			PLL HSI oscillator clock /2 selected as PLL input clock
				&	~( 0x7UL << 11U )// 		APB2 prescaler /1
				&	~( 0x3UL <<  8U );// 		APB1 prescaler /2
	RCC->CFGR	|=	 ( 0xFUL << 18U )//			PLL input clock x 16 (PLLMUL bits)
				|	 ( 0x4UL <<  8U );//		APB1 prescaler /2
	RCC->CR		|=	 ( 0x1UL << 24U );//		PLL2 ON
	while( !(RCC->CR & ~( 0x1UL << 25U )));//	wait until PLL is locked
	RCC->CFGR	&=	~( 0x1UL << 0U  );//		PLL used as system clock (SW bits)
	RCC->CFGR	|=	 ( 0x2UL << 0U  );//		PLL used as system clock (SW bits)
	while( 0x8UL != ( RCC->CFGR & 0xCUL ));//	wait until PLL is switched
}

void USER_Delay_1sec( void ){
	__asm(" 		ldr r0, =7111111UL	");//	load the value to be used as delay count
	__asm(" again1:	sub r0, r0, #1		");//	decrement the delay count
	__asm("			cmp r0, #0			");//	check if the delay count has reached zero
	__asm("			bne again1			");//	if not, repeat the process
	__asm("			nop					");//	no operation (to ensure exact timing)
}

void USER_Delay_10ms( void ){
	__asm(" 			ldr r0, =71111UL	");//	load the value to be used as delay count
	__asm(" again10:	sub r0, r0, #1		");//	decrement the delay count
	__asm("				cmp r0, #0			");//	check if the delay count has reached zero
	__asm("				bne again10			");//	if not, repeat the process
	__asm("				nop					");//	no operation (to ensure exact timing)
}