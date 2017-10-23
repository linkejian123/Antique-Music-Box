/* 
 * File:   AMBmain.c
 * Author: EE63PC9-user
 *
 * Created on November 22, 2015, 3:52 PM
 */

#include "p33fxxxx.h"
#include "SD-SPI.h"
#include "FSIO.h"
#include "vs1053.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <xc.h>
#include "configuration_pins.h"
#include "VFDdisplayMessage.h"
#include "VFDbar.h"


// External Oscillator

_FOSCSEL(FNOSC_FRC);		
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_NONE);  
								// Clock Switching is enabled and Fail Safe Clock Monitor is disabled
								// OSC2 Pin Function: OSC2 is Clock Output
								// Primary Oscillator Mode: XT Crystal


_FWDT(FWDTEN_OFF);              // Watchdog Timer Enabled/disabled by user software
								// (LPRC can be disabled by clearing SWDTEN bit in RCON register
#define True 1
#define False 0


#define MAXBUFFERSIZE 32
   
SearchRec file;
unsigned char attributes = ATTR_HIDDEN | ATTR_SYSTEM | ATTR_READ_ONLY | ATTR_VOLUME| ATTR_ARCHIVE;
char extention[] = "*.mp3";
int findfile = 0;   
FSFILE * pointer; 
unsigned char section[MAXBUFFERSIZE];
unsigned int timer_counter = 0;

unsigned short int currentVolume, scibass=0, memtest=0, clock=0, scimode=0, scistatus=0, hdat0=0, hdat1=0, samplerate=0;

void InitClock(void);
void Findfile(void);
void InitSPI(void);



int main(void)
{   
    Configure_Pins(); 

    //char message[] = "hello world  ";
    
    //VFDdisplayMessage(message);
    
    long data[5][2]={
        {7,6},
        {5,4},
        {3,2},
        {1,0},
        {7,7},
    };
    VFDbar(data);
        
//    InitSPI();
//    
//    vs1053Config();										// Open SPI2 and Set Pin
//    vs1053Reset();										// Reset MP3 Player
//	vs1053Init();										// initializes the vs1053 mp3 player to "New" modes
//	scimode = vs1053SCIRead(SCI_MODE);
//	scistatus = vs1053SCIRead(SCI_STATUS);	
//	vs1053SCIWrite(SCI_BASS, 0x44);
//	scibass = vs1053SCIRead(SCI_BASS);
//	clock = vs1053SCIRead(SCI_CLOCKF);
//	vs1053SCIWrite(SCI_AUDATA, 0xAC45);
//	samplerate = vs1053SCIRead(SCI_AUDATA);
//
//
//    while(!MDD_MediaDetect());
//    FSInit();
//       
//    Findfile();
            
    while(1);

}

void InitSPI(void) 
{
    SPI1CON1bits.DISSCK = 0; //SCK pin is controlled by SPI module
    SPI1CON1bits.DISSDO = 0; //SDO pin is controlled by SPI module
    SPI1CON1bits.MODE16 = 0; //8-bit mode selected
    SPI1CON1bits.SMP = 0; //sample data in midle of data output time
    SPI1CON1bits.CKE = 1; //positive clock edge selected
    SPI1CON1bits.SSEN = 0; //master mode, SS pin is not used
    SPI1CON1bits.CKP = 0; //clock active high
    SPI1CON1bits.MSTEN = 1; //select master mode
    SPI1CON1bits.SPRE = 0b101; //secondary scale 1:4
    SPI1CON1bits.PPRE = 0b11; //primary scale 1:1
    SPI1STATbits.SPIROV = 0; //clear overflow flag
    SPI1STATbits.SPIEN = 1; //enable SPI module; config SDI, SDO, SCK pins
    SPI1STATbits.SPISIDL = 0; //halt the module in idle state
    SPI1STATbits.SPITBF = 0; //clear TX buffer full flag
    SPI1STATbits.SPIRBF = 0; //clear RX buffer full flag
    
    SPI2CON1bits.DISSCK = 0; //SCK pin is controlled by SPI module
    SPI2CON1bits.DISSDO = 0; //SDO pin is controlled by SPI module
    SPI2CON1bits.MODE16 = 0; //8-bit mode selected
    SPI2CON1bits.SMP = 0; //sample data in midle of data output time
    SPI2CON1bits.CKE = 1; //positive clock edge selected
    SPI2CON1bits.SSEN = 0; //master mode, SS pin is not used
    SPI2CON1bits.CKP = 0; //clock active high
    SPI2CON1bits.MSTEN = 1; //select master mode
    SPI2CON1bits.SPRE = 0b101; //secondary scale 1:4
    SPI2CON1bits.PPRE = 0b11; //primary scale 1:1
    SPI2STATbits.SPIROV = 0; //clear overflow flag
    SPI2STATbits.SPIEN = 1; //enable SPI module; config SDI, SDO, SCK pins
    SPI2STATbits.SPISIDL = 0; //halt the module in idle state
    SPI2STATbits.SPITBF = 0; //clear TX buffer full flag
    SPI2STATbits.SPIRBF = 0; //clear RX buffer full flag
}

void Findfile(void)
{
    findfile = FindFirst(extention,attributes,&file);
    if ( findfile == 0)   // file find success
    {
        pointer= FSfopen(file.filename,"r");  // open current file
        if(pointer == NULL)
            while(1);
        while(FSfeof(pointer) == 0)
        {
            FSfread(section,1,MAXBUFFERSIZE,pointer);             
            while(X_DREQ != 1);
            vs1053SendMusic(section,MAXBUFFERSIZE); 
        }
        FSfclose(pointer);
        SendZerosToVS1053();
    }
         
}
      
void InitClock() 
{
    PLLFBD = 38;	// 
    CLKDIVbits.PLLPOST = 0;	// N1 = 2
    CLKDIVbits.PLLPRE = 0;	// N2 = 2
    OSCTUN = 0;
    RCONbits.SWDTEN = 0;
    // Clock switch to incorporate PLL    NOSC = 0b010
    __builtin_write_OSCCONH(0b001);
    __builtin_write_OSCCONL(0b001);	// Start clock switching
    while (OSCCONbits.COSC != 0b001);	// Wait for Clock switch to occur	
    while(OSCCONbits.LOCK != 1) {};
}

