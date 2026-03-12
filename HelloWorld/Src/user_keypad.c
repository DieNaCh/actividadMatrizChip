#include <stdint.h>
#include "main.h"
#include "user_keypad.h"

void USER_Keypad_Init( void ){
    RCC->APB2ENR |= ( 0x1UL << 3U ); 
}
uint8_t USER_Key( void ){
    
};