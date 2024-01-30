# Serial protocol between ESP-32 and PIC18F4550

This document outlines the serial protocol between the ESP-32 and the PIC18F4550.

## Assets
The PIC18F4550 manages these assets:
- Grid relay
- Hydroelectric relay
- Solar relay
- Main relay
- Water valve (either 12V valve or just a servo motor. But servo motor also need to be programmed with PWM.)

Since the PIC cannot get real time (RTC), we cannot expect it to internally enforce timing schedules.
Timing will be enforced via the ESP32, which is in turn enforced by AWS IoT Core.

Data in DynamoDB can also be used to enforce certain rules (e.g. if solar is used, turn off grid else turn on) through the server-side.

## Receiving
The PIC can receive several instructions, and the instructions are 1 byte long.
Here is the magic bytes for each instruction:

|Instruction|Byte|
|-|-|
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
