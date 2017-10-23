/* 
 * File:   configuration_pins.h
 * Author: EE63PC9-user
 *
 * Created on November 22, 2015, 4:04 PM
 */

#ifndef CONFIGURATION_PINS_H
#define	CONFIGURATION_PINS_H

#ifdef	__cplusplus
extern "C" {
    

#endif

#define OUTPUT 0
#define INPUT 1

//VFD Display configuration    
#define TRIS_VFD_LATCH  TRISFbits.TRISF4    // RF4 latch pin
#define TRIS_VFD_CLOCK  TRISAbits.TRISA1    // RA1 clock pin
#define TRIS_VFD_DATA  TRISAbits.TRISA2     // RA2 data pin
#define TRIS_VFD_BLANK  TRISGbits.TRISG2    // RG2 blank

#define VFD_LATCH   PORTFbits.RF4
#define VFD_CLOCK   PORTAbits.RA1
#define VFD_DATA   PORTAbits.RA2
#define VFD_BLANK   PORTGbits.RG2
    
// Bar Display configuration
#define TRIS_BAR_LATCH  TRISAbits.TRISA6  // RA6 latch pin
#define TRIS_BAR_CLOCK  TRISFbits.TRISF5  // RF5 clock pin
#define TRIS_BAR_DATA  TRISGbits.TRISG3  // RG3 data pin
#define TRIS_BAR_BLANK  TRISAbits.TRISA7  // RA7 blank
    
#define BAR_LATCH   LATAbits.LATA6
#define BAR_CLOCK   LATFbits.LATF5
#define BAR_DATA   LATGbits.LATG3
#define BAR_BLANK   LATAbits.LATA7
    
// MUX configuration
#define config_mux1 TRISEbits.TRISE1    //mux1
#define config_mux2 TRISDbits.TRISD13   //mux2

#define mux1 PORTEbits.RE1
#define mux2 PORTDbits.RD13
    
// LED configuration
#define TRIS_LED_BAR TRISDbits.TRISD4
#define TRIS_LED_VFD TRISDbits.TRISD5
    
#define LED_BAR PORTDbits.RD4//LATDbits.LATD4
#define LED_VFD PORTDbits.RD5//LATDbits.LATD5
    
void Configure_Pins(void);

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIGURATION_PINS_H */

