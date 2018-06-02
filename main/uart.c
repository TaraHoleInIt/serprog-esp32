#include <stdio.h>
#include <stdint.h>
#include "driver/uart.h"
#include "config.h"
#include "uart.h"

static uart_port_t UARTPort = UART_NUM_2;

bool UARTInit(  int TXPin, int RXPin ) {
    uart_config_t Config = {
        .baud_rate = BaudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK( uart_param_config( UARTPort, &Config ) );
    ESP_ERROR_CHECK( uart_driver_install( UARTPort, 2048, 0, 0, NULL, 0 ) );
    ESP_ERROR_CHECK( uart_set_pin( UARTPort, TXPin, RXPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE ) );

    return true;
}

uint8_t UARTRead8( void ) {
    uint8_t Data = 0;

    uart_read_bytes( UARTPort, &Data, sizeof( Data ), UARTTimeout );
    return Data;
}

uint16_t UARTRead16( void ) {
    uint16_t Data = 0;

    uart_read_bytes( UARTPort, ( uint8_t* ) &Data, sizeof( Data ), UARTTimeout );
    return Data;
}

uint32_t UARTRead24( void ) {
    uint32_t Data = 0;

    uart_read_bytes( UARTPort, ( uint8_t* ) &Data, 3, UARTTimeout );
    return Data;
}

uint32_t UARTRead32( void ) {
    uint32_t Data = 0;

    uart_read_bytes( UARTPort, ( uint8_t* ) &Data, sizeof( Data ), UARTTimeout );
    return Data;
}

void UARTWrite8( uint8_t Data ) {
    uart_write_bytes( UARTPort, ( const char* ) &Data, sizeof( Data ) );
    //uart_flush( UARTPort );
    //uart_wait_tx_done( UARTPort, UARTTimeout );
}

void UARTWrite16( uint16_t Data ) {
    uart_write_bytes( UARTPort, ( const char* ) &Data, sizeof( Data ) );
    //uart_flush( UARTPort );
    //uart_wait_tx_done( UARTPort, UARTTimeout );
}

void UARTWrite24( uint32_t Data ) {
    uart_write_bytes( UARTPort, ( const char* ) &Data, 3 );
    //uart_flush( UARTPort );
    //uart_wait_tx_done( UARTPort, UARTTimeout );
}

void UARTWrite32( uint32_t Data ) {
    uart_write_bytes( UARTPort, ( const char* ) &Data, sizeof( Data ) );
    //uart_flush( UARTPort );
    //uart_wait_tx_done( UARTPort, UARTTimeout );
}
