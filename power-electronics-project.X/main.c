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
#include"delays.h"

#define HIGH 0xD0
// Port C is a 7 bit wide port.
#define MAIN_PIN_RELAY PORTCbits.RC0
#define GRID_PIN_RELAY PORTCbits.RC1
#define HYDRO_PIN_RELAY PORTCbits.RC2
#define SOLAR_PIN_RELAY PORTCbits.RC3
#define VALVE_PIN PORTCbits.RC4






void onRxReceive(char data);

void sendDataToAWS() {
    
}


void interrupt isr(void) {
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


int previousState[3] = {1,1,1};
void onRxReceive(char data) {
    char a[1];
    a[0] = data; 
    
    switch (data) {
        case 0x30:
            previousState[0] =GRID_PIN_RELAY;
            previousState[1] = HYDRO_PIN_RELAY;
            previousState[2] = SOLAR_PIN_RELAY;
            GRID_PIN_RELAY = 0;
            HYDRO_PIN_RELAY = 0;
            SOLAR_PIN_RELAY = 0;
            
            break;
        case 0x31:
            GRID_PIN_RELAY = previousState[0];
            HYDRO_PIN_RELAY = previousState[1];
            SOLAR_PIN_RELAY = previousState[2];
            break;
        case 0x32:
            GRID_PIN_RELAY = 1;
            break;
        case 0x33:
            GRID_PIN_RELAY = 0;
            break;
        case 0x34:
           SOLAR_PIN_RELAY = 0;
           break;
        case 0x35:
            SOLAR_PIN_RELAY = 1;
            break;
        case 0x36:
            HYDRO_PIN_RELAY = 1;
            break;
        case 0x37:
            HYDRO_PIN_RELAY = 0;
            break;
        case 0x38:
            VALVE_PIN = 1;
            break;
        case 0x39:
            VALVE_PIN = 1;
            break;
            

    }
    
}



void onetone(void) {
    unsigned int k;
    for (k = 0; k < 100; k++) 
    {
        delay_us(300);  // 100 - 3000 Change accordingly.
        PORTCbits.RC0 = !PORTCbits.RC0; 
    }
    PORTCbits.RC0 = 0;
}


void timerInit() {
    // 1 second for timer0.
    TMR0H = 0x48;
	TMR0L = 0xE5;
     
    T0CON = 0b00000111;    // bit7:0 Stop Timer0
							// bit6:0 Timer0 as 16 bit timer
							// bit5:0 Clock source is internal 
							// bit4:0 Increment on lo to hi transition on TOCKI pin
							// bit3:0 Prescaler output is assigned to Timer0  
							// bit2-bit0:111 1:256 prescaler
    INTCONbits.TMR0IF = 0;
    // TODO: to calculate the bits and pre-scaler value for 1 min. 
    
    T0CONbits.TMR0ON = 1;	// Turn on timer
    INTCONbits.TMR0IE = 1; // Turn on timer interrupts
}


void main(void) {
    
    // For analog inputs
    
       // RA  54321
    TRISA = 0b11111;
    TRISE = 0b111;
    
    
    // End analog inputs
    
    
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB3 = 0;
    
    TRISDbits.TRISD1 = 0;
    
    
    
    // For Serial Communication and Interrupts
    
    // TXREG: byte to transmit
    // RCREG: byte received
    // TXSTA: select modes for transmit
    // RCSTA: select modes for receive
    // PIR1: Interrupt Request flap 1. 2 BIT. TXIF (tranmission complete) RCIF (receiving complete)
    TRISCbits.TRISC6 = 0; // TX
    TRISCbits.TRISC7 = 1; // RX
    SPBRG = 77; // Baud Rate of 9600
    TXSTA = 0b00100000;
    RCSTA = 0b10010000;
    
    INTCONbits.PEIE = 1;
    PIE1bits.RCIE = 1; // Enable interrupt on RX.
    // End Serial
    
    
    

    // Bit 7-6 unimplemented
    // Bit 5 VCFG1 VREF-. We set to 0 by default.
    // Bit 4 VCFG0 VREF+. We set to 1 by default.
    // Bit 3-0 A/D Port config.
    ADCON1 = 0b00000111; // TODO: change as necessary.    
    // --- 
    ADCON2 = 0b10010110; // bit7   : A/D Result Format. 0 Left, 1 Right justified
                        // bit5-3 : 010 acquisition time = 4 TAD
                        // bit2-0 : 110 conversion clock = Tosc / 16
    lcd_init();
    
    timerInit();
    
    
    INTCONbits.GIE = 1;
    
    
    
    

//    while (1) {
//        struct Value grid = {GRID_INST, 1022, 1022};
//        sendSensorValue(grid, 1);
//        struct Value hydro = {HYDRO_INST, 1022, 1022};
//        sendSensorValue(hydro, 0);
//        delay_ms(5000);
//    }
//    
    
    
    

    
    return;
}

/*
 * Hydro: Rail 1
 * Grid: Rail 2
 * Solar: Rail 3
 */


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