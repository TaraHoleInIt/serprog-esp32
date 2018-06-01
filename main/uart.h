#ifndef _UART_H_
#define _UART_H_

bool UARTInit( int TXPin, int RXPin );

uint8_t UARTRead8( void );
uint16_t UARTRead16( void );
uint32_t UARTRead24( void );
uint32_t UARTRead32( void );

void UARTWrite8( uint8_t Data );
void UARTWrite16( uint16_t Data );
void UARTWrite24( uint32_t Data );
void UARTWrite32( uint32_t Data );

#endif
