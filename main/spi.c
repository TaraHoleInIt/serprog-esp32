#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "driver/uart.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "serprog.h"
#include "uart.h"
#include "spi.h"
#include "config.h"

static spi_device_handle_t FlashHandle = NULL;
static bool SPIReady = false;

static spi_bus_config_t SPIBusConfig = {
    .mosi_io_num = Pin_MOSI,
    .miso_io_num = Pin_MISO,
    .sclk_io_num = Pin_SCLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 8192,
    .flags = SPICOMMON_BUSFLAG_MASTER
};

static spi_device_interface_config_t FlashDeviceConfig = {
    .clock_speed_hz = Frequency,
    .spics_io_num = Pin_CS,
    .flags = 0,//SPI_DEVICE_HALFDUPLEX,
    .queue_size = 1
};

bool SPIInit( void ) {
    if ( SPIReady == false ) {
        ESP_ERROR_CHECK( spi_bus_initialize( VSPI_HOST, &SPIBusConfig, 1 ) );
        ESP_ERROR_CHECK( spi_bus_add_device( VSPI_HOST, &FlashDeviceConfig, &FlashHandle ) );

        SPIReady = true;
    }

    return SPIReady;
}

void SPIRW( uint32_t RBytes, uint32_t SBytes ) {
    static uint8_t TXBuffer[ RWBufferSize ];
    static uint8_t RXBuffer[ RWBufferSize ];
    spi_transaction_t SPITrans;
    int Length = 0;
    int i = 0;

    /* TODO:
     * Properly handle this!
     */
    if ( SBytes > RWBufferSize ) {
        printf( "[SBytes: %u] > %d\n", SBytes, sizeof( TXBuffer ) );
        return;
    }

    /* TODO:
     * Properly handle this!
     */
    if ( RBytes > RWBufferSize ) {
        printf( "[RBytes: %u] > %d\n", RBytes, sizeof( RXBuffer ) );
        return;
    }

    if ( SBytes > 0 ) {
        memset( TXBuffer, 0x00, sizeof( TXBuffer ) );
        uart_read_bytes( UART_NUM_2, TXBuffer, SBytes, portMAX_DELAY );
    }

    memset( &SPITrans, 0, sizeof( spi_transaction_t ) );

    Length = ( SBytes > RBytes ) ? SBytes : RBytes;
    Length = ( RBytes > 0 ) ? Length + 1 : Length;

    SPITrans.length = ( SBytes > 0 ) ? Length * 8 : 0;
    SPITrans.rxlength = ( RBytes > 0 ) ? Length * 8 : 0;
    SPITrans.tx_buffer = ( SBytes > 0 ) ? TXBuffer : NULL;
    SPITrans.rx_buffer = ( RBytes > 0 ) ? RXBuffer : NULL;

    ESP_ERROR_CHECK( spi_device_transmit( FlashHandle, &SPITrans ) );
    UARTWrite8( S_ACK );

    if ( RBytes > 0 ) {
       uart_write_bytes( UART_NUM_2, ( const char* ) &RXBuffer[ 1 ], RBytes );
       uart_flush( UART_NUM_2 );
       uart_wait_tx_done( UART_NUM_2, portMAX_DELAY );
    }
}

