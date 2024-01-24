/*
 * File:   serial.c
 * Author: luciuschee
 *
 * Created on 24 January, 2024, 3:46 PM
 */


#include <xc.h>

/**
 * Blocking, synchronous call for writing 1 byte to serial.
 * @param data byte to be written
 */
void txWriteByte(char data) {
    if (PIR1bits.TXIF == 1) { // Previous transmission complete.
        TXREG = data;
        while(PIR1bits.TXIF != 1); // Polling - wait until transmit finish
    }
}

/**
 * Blocking, synchronous call waiting for 1 byte to be read from serial.
 * @return data read.
 */
char rxReadByte() {
    while (PIR1bits.RCIF != 1);
    char data = RCREG;
    return data;    
}

