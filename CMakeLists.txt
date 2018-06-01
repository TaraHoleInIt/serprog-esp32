cmake_minimum_required(VERSION 3.5)

set(MAIN_SRCS main/main.c main/uart.c main/serprog.c main/spi.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake) 
project(serprog)
