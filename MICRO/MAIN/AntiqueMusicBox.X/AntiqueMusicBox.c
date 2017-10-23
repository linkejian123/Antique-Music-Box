/*
 * File:   AntiqueMusicBox.c
 * Author: ECE477 Team8 Antique Music Box
 * The main file for the AntiqueMusicBox
 * Created on September 27, 2015, 2:44 PM
 */

#include "p33fxxxx.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <xc.h>

#include "configuration_pins.h"
#include "VFDdisplayMessage.h"

_FOSCSEL(FNOSC_FRC);	// Primary oscillator

_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_HS);  

void InitClock(void);

int main(void) {
    

    // Use the configuration function to configure all the pins
    Configure_Pins();
    // Test Output pin
    while(1)
    {
        PORTAbits.RA13=1;
        delay(100);
        PORTAbits.RA13=0;
        delay(100);
    }
    char message[] = "hello world";
    VFDdisplayMessage(message);
   
    return 0;
}
