#include <stdint.h>
#include "main.h"
#include "user_keypad.h"

void USER_Keypad_Init( void ){
    // Enable clock
    RCC->APB2ENR |= ( 0x1UL << 3U ); 
    
    // Rows are output push-pull
    // CNF[1:0] = 00
    // MODE[1:0] = 01
    // ODR = 0 or 1

    // PA3
    GPIOA->ODR  &=      ~( 0x1UL <<  3U );
    GPIOA->CRL  &=      ~( 0x3UL << 14U )
                &       ~( 0x2UL << 12U );
    GPIOA->CRL  |=       ( 0x1UL << 12U );
    
    // PA2
    GPIOA->ODR  &=      ~( 0x1UL <<  2U );
    GPIOA->CRL  &=      ~( 0x3UL << 10U )
                &       ~( 0x2UL <<  8U );
    GPIOA->CRL  |=       ( 0x1UL <<  8U );

    // PA10
    GPIOA->ODR  &=      ~( 0x1UL << 10U );
    GPIOA->CRH  &=      ~( 0x3UL << 10U )
                &       ~( 0x2UL <<  8U );
    GPIOA->CRH  |=       ( 0x1UL <<  8U );
    
    // PB3
    GPIOB->ODR  &=      ~( 0x1UL <<  3U );
    GPIOB->CRL  &=      ~( 0x3UL << 14U )
                &       ~( 0x2UL << 12U );
    GPIOB->CRL  |=       ( 0x1UL << 12U );

    // Columns are input pull-up
    // CNF[1:0] = 10
    // MODE[1:0] = 00
    // ODR = 1

    // PB5
    GPIOB->ODR  |=      ( 0x1UL <<  5U );
    GPIOB->CRL  &=      ( 0x1UL << 22U )
                &       ( 0x3UL << 20U );
    GPIOB->CRL  |=      ( 0x2UL << 22U );
    // PB4
    GPIOB->ODR  |=      ( 0x1UL <<  4U );
    GPIOB->CRL  &=      ( 0x1UL << 18U )
                &       ( 0x3UL << 16U );
    GPIOB->CRL  |=      ( 0x2UL << 18U );
    // PB10
    GPIOB->ODR  |=      ( 0x1UL << 10U );
    GPIOB->CRH  &=      ( 0x1UL << 10U )
                &       ( 0x3UL <<  8U );
    GPIOB->CRH  |=      ( 0x2UL << 10U );
    // PA8
    GPIOA->ODR  |=      ( 0x1UL <<  8U );
    GPIOA->CRH  &=      ( 0x1UL <<  2U )
                &       ( 0x3UL <<  0U );
    GPIOA->CRH  |=      ( 0x2UL <<  2U );
}

Pin rows[4] = { 
    { GPIOA, ( 0x1UL <<  3U ) }, 
    { GPIOA, ( 0x1UL <<  2U ) },
    { GPIOA, ( 0x1UL << 10U ) },
    { GPIOB, ( 0x1UL <<  3U ) }
};

Pin cols[4] = { 
    { GPIOB, ( 0x1UL <<  5U ) }, 
    { GPIOB, ( 0x1UL <<  4U ) },
    { GPIOB, ( 0x1UL << 10U ) },
    { GPIOA, ( 0x1UL <<  8U ) }
};

uint8_t mapping[4][4] = {
    // Mapped * to 0xEU
    // Mapped # to 0xFU
    {0x1U, 0x2U,0x3U, 0xAU},
    {0x4U, 0x5U, 0x6U, 0xBU},
    {0x7U, 0x8U, 0x9U, 0xCU},
    {0xEU, 0x0U, 0xFU, 0xDU}
};

uint8_t USER_Key( void ){
    // Sweep through the matrix
    for (uint8_t i = 0; i < 4; i++) {
        
        // Pull all rows high
        for (uint8_t j = 0; j < 4; j++) {
            rows[j].port->ODR |= rows[j].mask;
        }

        // Pull current row low
        rows[i].port->ODR &= ~rows[j].mask;

        for (uint8_t j = 0; j < 4; j++) {
            // Check if button is pressed
            if ( !(cols[j].port->IDR & cols[j].mask) ) {
                return mapping[i][j];
            }
        }
    }

    return '\0'; // Return null if nothing is pressed
}