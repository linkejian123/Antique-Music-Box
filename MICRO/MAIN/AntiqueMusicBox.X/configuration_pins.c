/*
 * File:   configuration_pins.c
 * Author: ECE477 Team8 Antique Music Box
 * This file configure all the pins for the Antique Music Box
 * Created on October 31, 2015, 12:00 AM
 */

#include <xc.h>
#include "p33fxxxx.h"

#define OUTPUT 0
#define INPUT 1

void Configure_Pins(void)
{
    // VFD Display configuration
    TRISFbits.TRISF4 = OUTPUT;  // RF4 latch pin
    TRISAbits.TRISA1 = OUTPUT;  // RA1 clock pin
    TRISAbits.TRISA2 = OUTPUT;  // RA2 data pin
    TRISGbits.TRISG2 = OUTPUT;  // RG2 blank
    
    // Bar Display configuration
    TRISGbits.TRISG3 = OUTPUT;  // RG3 latch pin
    TRISFbits.TRISF5 = OUTPUT;  // RF5 clock pin
    TRISAbits.TRISA6 = OUTPUT;  // RA6 data pin
    TRISAbits.TRISA7 = OUTPUT;  // RA7 blank
    
    // SD card configuration
    
    
    // UART configuration
    
    
    // Test Output pin
    TRISAbits.TRISA13=OUTPUT;
}
