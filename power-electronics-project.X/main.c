/*
 * File:   main.c
 * Author: luciuschee
 *
 * Created on 23 January, 2024, 11:08 AM
 */


#include <xc.h>
#include"analog.h"
#include"lcd.h"
#include<stdio.h>
#define HIGH 0xD0




void main(void) {
    
    // 0: GV. 1: GC.
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB3 = 0;
    
    TRISCbits.TRISC6 = 0; // TX
    TRISCbits.TRISC7 = 1; // RX
    SPBRG = 77; // Baud Rate of 9600
    
    
    // bit5-2 0000 select channel 0 conversion 
    // bit1	  A/D conversion status bit
    //	      1- GO to starts the conversion
    //	      0 - DONE when A/D is completed
    // bit0   Set to 1 to power up A/D
    ADCON1 = 0b00001100; // TODO: change as necessary.    
    // --- 
    ADCON2 = 0b10010110; // bit7   : A/D Result Format. 0 Left, 1 Right justified
                        // bit5-3 : 010 acquisition time = 4 TAD
                        // bit2-0 : 110 conversion clock = Tosc / 16
    lcd_init();
    
    
    // TXREG: byte to transmit
    // RCREG: byte received
    // TXSTA: select modes for transmit
    // RCSTA: select modes for receive
    while(1) {
        if ()
    }
    
    
    
    
    

    
    return;
}


/*
     while (1) {
        int value = analogRead(0);
        char buf[4];
        int len = sprintf(buf, "%d ", value);
        lcd_print(0, buf, len);
        
        int value2 = analogRead(1);
        char buf2[4];
        int len = sprintf(buf2, "%d ", value2);
        lcd_print(1, buf2, len);
        
        
    }
 
 
 */