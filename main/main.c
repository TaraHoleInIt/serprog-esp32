#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "driver/uart.h"
#include "driver/spi_master.h"
#include "uart.h"
#include "serprog.h"

void app_main( void ) {
    printf( "Ready...\n" );

    if ( UARTInit( 17, 16 ) == true ) {
        while ( true ) {
            Serprog_Tick( );
            vTaskDelay( pdMS_TO_TICKS( 5 ) );
        }
    }
}
