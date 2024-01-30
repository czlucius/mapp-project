/*
 * File:   main.c
 * Author: luciuschee
 *
 * Created on 23 January, 2024, 11:08 AM
 */


#include <xc.h>
#include"analog.h"
#include"lcd.h"
#include"serial.h"
#include<stdio.h>

#define HIGH 0xD0
// Port C is a 7 bit wide port.
#define MAIN_PIN PORTCbits.RC0
#define GRID_PIN PORTCbits.RC1
#define HYDRO_PIN PORTCbits.RC2
#define SOLAR_PIN PORTCbits.RC3
#define VALVE_PIN PORTCbits.RC4



void onRxReceive(char data);


void __interrupt() isr(void) {
    if (PIR1bits.RCIF == 1) {
        if(RCSTAbits.OERR) { 
            // Overflow error!
            RCSTAbits.CREN = 0; // continuous receive disable
            // no-operation
            NOP();
            RCSTAbits.CREN=1; // continuous receive enable
        } 
        onRxReceive(RCREG);        
    }
    if (INTCONbits.TMR0IF == 1) {
        // TODO reset timer to 1 min.
        INTCONbits.TMR0IF = 0;
        sendDataToAWS();
    }
}

void sendDataToAWS() {
    
}



/*
 * 
|Turn main on|0x30|
|Turn main off|0x31|
|Turn grid on|0x32|
|Turn grid off|0x33|
|Turn solar on|0x34|
|Turn solar off|0x35|
|Turn hydro on|0x36|
|Turn hydro off|0x37|
|Turn water valve on|0x38|
|Turn water valve off|0x39|
 */

void onRxReceive(char data) {
    
    switch (data) {
        case 0x30:
            MAIN_PIN = 1;
            break;
        case 0x31:
            MAIN_PIN = 0;
            break;
        case 0x32:
            GRID_PIN = 1;
            break;
        case 0x33:
            GRID_PIN = 0;
            break;
        case 0x34:
            HYDRO_PIN = 1;
            break;
        case 0x35:
            HYDRO_PIN = 0;
            break;
        case 0x36:
            VALVE_PIN = 1;
            break;
        case 0x37:
            VALVE_PIN = 1;
            break;

    }
    
}





void main(void) {
    
    // 0: GV. 1: GC.
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB3 = 0;
    
    TRISCbits.TRISC6 = 0; // TX
    TRISCbits.TRISC7 = 1; // RX
    SPBRG = 77; // Baud Rate of 9600
    TXSTA = 0b00100000;
    RCSTA = 0b10010000;
    
    INTCONbits.PEIE = 1;
    PIE1bits.RCIE = 1; // Enable interrupt on RX.
    
    
    
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
    
    
    
    
    T0CON = 0b10000100;
    INTCONbits.TMR0IF = 0;
    // TODO: to calculate the bits and pre-scaler value for 1 min. 
    
    
    INTCONbits.TMR0IE = 1;
    
    INTCONbits.GIE = 1;
    
    
    
    
    
    // TXREG: byte to transmit
    // RCREG: byte received
    // TXSTA: select modes for transmit
    // RCSTA: select modes for receive
    // PIR1: Interrupt Request flap 1. 2 BIT. TXIF (tranmission complete) RCIF (receiving complete)
//    while(1) {
//        if ()
//    }
    
    
    
    
    

    
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