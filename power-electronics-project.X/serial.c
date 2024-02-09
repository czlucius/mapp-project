/*
 * File:   serial.c
 * Author: luciuschee
 *
 * Created on 24 January, 2024, 3:46 PM
 */


#include <xc.h>


#include "serial.h"

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



/**
 * Sends sensor readings.
 * NOTE: this DOES NOT interrupt the ESP32.
 * @param type
 * @param voltage
 * @param current
 * @param contd
 */
void sendSensorReadings(char type, int voltage, int current, char contd) {
    txWriteByte('~'); // Start byte
    txWriteByte(type);
    char vstr[5];
    sprintf(vstr, "%04d", voltage);
    char istr[5];
    sprintf(istr, "%04d", current);
    
    for (char i=0; i<4; i++) {
        txWriteByte(vstr[i]);
    }
    txWriteByte('&');
    return;
    for (char i=0; i<4; i++) {
        txWriteByte(istr[i]);
    }
    txWriteByte('#');
    txWriteByte(contd + 0x30);
    txWriteByte('#');    
}

void sendSensorValue(struct Value value, char contd) {
    sendSensorReadings(value.type, value.voltage, value.current, contd);
}

//
//void sendSensorsAll(Value* valuePoint, char length) {
//    for (char i=0; i<length; i++) {
//        bit contd;
//        if (i == len - 1) {
//            contd = 0;
//        } else {
//            contd = 1;
//        }
//        Value value = *valuePoint;
//        sendSensorReadings(value.type, value.voltage, value.current, contd);
//    }
//    
//}