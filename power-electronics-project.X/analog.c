/*
 * File:   analog.c
 * Author: luciuschee
 *
 * Created on 23 January, 2024, 11:17 AM
 */


#include <xc.h>

int analogRead(char channel) {
    char toWrite = 0;
    // bit 5-2
    // (543210) 001111 -> 011110 -> 111100
    toWrite |= (channel << 2);
    // GO bit set later.
    // ADON bit set.
    toWrite |= 0b1;
    
    ADCON0 = toWrite;
    
    ADCON0bits.ADON = 1;
    
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO == 1);
    // ADC has finished!
    // Our result in ADRESH and ADRESL
    // Right justified.
    
    return (ADRESH * 256 + ADRESL);
}