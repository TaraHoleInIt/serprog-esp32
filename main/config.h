#ifndef _CONFIG_H_
#define _CONFIG_H_

#define UARTTimeout portMAX_DELAY
#define RWBufferSize 2048
#define BaudMul 1
#define BaudRate ( 115200 * BaudMul )

#define Frequency 100000
#define Pin_MOSI 13
#define Pin_MISO 12
#define Pin_SCLK 14
#define Pin_CS 15

#endif
