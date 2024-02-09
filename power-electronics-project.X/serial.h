
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef SERIAL_H
#define	SERIAL_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include<stdio.h>
#include "lcd.h"

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

#define MAIN_INST 0x60
#define SOLAR_INST 0x61
#define GRID_INST 0x62
#define HYDRO_INST 0x63


/**
 * Blocking, synchronous call for writing 1 byte to serial.
 * @param data byte to be written
 */
void txWriteByte(char data);
/**
 * Blocking, synchronous call waiting for 1 byte to be read from serial.
 * @return data read.
 */
char rxReadByte();

struct Value {
    char type;
    int voltage;
    int current;
};

void sendSensorReadings(char type, int voltage, int current, char contd);

void sendSensorValue(struct Value value, char contd);
//void sendSensorsAll(Value* valuePoint, char length);
