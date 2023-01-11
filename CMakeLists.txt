cmake_minimum_required(VERSION 3.24)
project(mspRefleks C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(mspRefleks
        gra.c
        gra.h
        in430.h
        kafelek.c
        kafelek.h
        lcd.c
        lcd.h
        main.c
        msp430x14x.h
        portyLcd.c
        portyLcd.h
        portyUart.c
        portyUart.h
        uart.c
        uart.h
        uartservice.c
        znaki.c
        znaki.h)
