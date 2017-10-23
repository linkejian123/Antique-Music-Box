/*
 * File:   configuration_pins.c
 * Author: ECE477 Team8 Antique Music Box
 * This file configure all the pins for the Antique Music Box
 * Created on October 31, 2015, 12:00 AM
 */

#include <xc.h>
#include "p33fxxxx.h"
#include "configuration_pins.h"

void Configure_Pins(void)
{
    AD1PCFGL = 1;
    AD1PCFGH = 1;   
    AD2PCFGL = 1;
    CNPU1 = 0;
    CNPU2 = 0;
    
    // VFD Display configuration
    TRIS_VFD_LATCH = OUTPUT;  
    TRIS_VFD_CLOCK = OUTPUT;  
    TRIS_VFD_DATA = OUTPUT;  
    TRIS_VFD_BLANK = OUTPUT;

    // Bar Display configuration    
    TRIS_BAR_LATCH = OUTPUT;
    TRIS_BAR_CLOCK = OUTPUT;
    TRIS_BAR_DATA = OUTPUT;
    TRIS_BAR_BLANK = OUTPUT;
    
    // MUX configuration
    config_mux1 = OUTPUT;
    config_mux2 = OUTPUT;
    
    // LED configuration
    TRIS_LED_BAR = OUTPUT;
    TRIS_LED_VFD = OUTPUT;
    
    // Test Breathing Output pin
    TRISAbits.TRISA13 = OUTPUT;
    LATAbits.LATA13 = 1;
}
//