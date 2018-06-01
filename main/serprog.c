#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "driver/uart.h"
#include "driver/spi_master.h"
#include "uart.h"
#include "spi.h"
#include "serprog.h"
#include "config.h"

typedef void ( *CommandProc ) ( void );

static void ack( void );
static void nak( void );
static void nop( void );
static void q_iface( void );
static void q_cmdmap( void );
static void q_pgmname( void );
static void q_serbuf( void );
static void q_bustype( void );
static void q_wrnmaxlen( void );
static void o_delay( void );
static void syncnop( void );
static void q_maxrdlen( void );
static void s_bustype( void );
static void o_spiop( void );

static const uint32_t CommandMap = \
    BIT( S_CMD_NOP ) | \
    BIT( S_CMD_Q_IFACE ) | \
    BIT( S_CMD_Q_CMDMAP ) | \
    BIT( S_CMD_SYNCNOP ) | \
    BIT( S_CMD_Q_PGMNAME ) | \
    BIT( S_CMD_Q_SERBUF ) | \
    BIT( S_CMD_Q_BUSTYPE ) | \
    BIT( S_CMD_Q_WRNMAXLEN ) | \
    BIT( S_CMD_Q_RDNMAXLEN ) | \
    BIT( S_CMD_S_BUSTYPE ) | \
    BIT( S_CMD_O_SPIOP ) | \
    BIT( S_CMD_O_DELAY ) \
;

static const uint8_t ProgrammerName[ 16 ] = {
    's', 'e', 'r', 'p', 'r', 'o', 'g', '-', 'e', 's', 'p', '3', '2', 0, 0, 0
};

static const uint8_t SupportedBusses = \
    BIT( 3 ) \
;

static const char* const CommandNames[ ] = {
    "S_CMD_NOP",
    "S_CMD_Q_IFACE",
    "S_CMD_Q_CMDMAP",
    "S_CMD_Q_PGMNAME",
    "S_CMD_Q_SERBUF",
    "S_CMD_Q_BUSTYPE",
    "S_CMD_Q_CHIPSIZE",
    "S_CMD_Q_OPBUF",
    "S_CMD_Q_WRNMAXLEN",
    "S_CMD_R_BYTE",
    "S_CMD_R_NBYTES",
    "S_CMD_O_INIT",
    "S_CMD_O_WRITEB",
    "S_CMD_O_WRITEN",
    "S_CMD_O_DELAY",
    "S_CMD_O_EXEC",
    "S_CMD_SYNCNOP",
    "S_CMD_Q_RDNMAXLEN",
    "S_CMD_S_BUSTYPE",
    "S_CMD_O_SPIOP",
    "S_CMD_S_SPI_FREQ",
    "S_CMD_S_PIN_STATE"
};

static CommandProc SerprogCommandTable[ ] = {
    nop,        // 0x00
    q_iface,    // 0x01
    q_cmdmap,   // 0x02
    q_pgmname,  // 0x03
    q_serbuf,   // 0x04
    q_bustype,  // 0x05
    nak,    // 0x06 q chip size
    nak,    // 0x07 q opbuf
    q_wrnmaxlen,    // 0x08 q wrnmaxlen
    nak,    // 0x09 r byte
    nak,    // 0x0A r n bytes
    nak,    // 0x0B o init
    nak,    // 0x0C o write byte
    nak,    // 0x0D o write n bytes
    nak,    // 0x0E o delay
    nak,    // 0x0F o exec,
    syncnop,// 0x10
    q_maxrdlen,    // 0x11 q max rd n len
    s_bustype,  // 0x12
    o_spiop,    // 0x13
    nak,    // 0x14 s spi freq
    nak     // 0x15 s pin state
};

static const int CommandCount = 0x16;

static void ack( void ) {
    UARTWrite8( S_ACK );
}

static void nak( void ) {
    UARTWrite8( S_NAK );
}

static void nop( void ) {
    ack( );
}

static void q_iface( void ) {
    ack( );
    UARTWrite16( 1 );
}

static void q_cmdmap( void ) {
    int i = 0;

    ack( );
    UARTWrite32( CommandMap );

    for ( i = 0; i < ( 32 - 4 ); i++ ) {
        UARTWrite8( 0 );
    }
}

static void q_pgmname( void ) {
    int i = 0;

    ack( );

    for ( i = 0; i < sizeof( ProgrammerName ); i++ ) {
        UARTWrite8( ProgrammerName[ i ] );
    }
}

static void q_serbuf( void ) {
    ack( );
    UARTWrite16( RWBufferSize );
}

static void q_bustype( void ) {
    ack( );
    UARTWrite8( SupportedBusses );
}

static void q_wrnmaxlen( void ) {
    ack( );
    UARTWrite24( RWBufferSize );
}

static void o_delay( void ) {
    ack( );
    ets_delay_us( UARTRead32( ) );
}

static void syncnop( void ) {
    nak( );
    ack( );
}

static void q_maxrdlen( void ) {
    ack( );
    UARTWrite24( RWBufferSize );
}

static void s_bustype( void ) {
    uint8_t Bustypes = UARTRead8( );

    printf( "%s: %u\n", __func__, Bustypes );

    if ( Bustypes == SupportedBusses ) {
        SPIInit( );
        ack( );
    } else {
        printf( "Unsupported bus types 0x%02X\n", Bustypes );
        nak( );
    }
}

//0x13	Perform SPI operation		24-bit slen + 24-bit rlen	ACK + rlen bytes of data / NAK
//					 + slen bytes of data
static void o_spiop( void ) {
    uint32_t TXLen = 0;
    uint32_t RXLen = 0;

    TXLen = UARTRead24( );
    RXLen = UARTRead24( );

    //printf( "%s: [TX: %u] [RX: %u]\n", __func__, TXLen, RXLen );
    SPIRW( RXLen, TXLen );
}

void Serprog_Tick( void ) {
    uint8_t Opcode = UARTRead8( );
    
    if ( Opcode < CommandCount ) {
        //printf( "%s: [0x%02x] %s\n", __func__, Opcode, CommandNames[ Opcode ] );

        if ( SerprogCommandTable[ Opcode ] != NULL ) {
            ( SerprogCommandTable[ Opcode ] ) ( );
        }
    }
}
