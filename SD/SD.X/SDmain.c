
#include "p33fxxxx.h"
#include "SD-SPI.h"
#include "FSIO.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <xc.h>

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

FSFILE *pointer;
SearchRec file;
unsigned char song_is_over=0;
unsigned char button_pressed;
unsigned char attributes = ATTR_HIDDEN | ATTR_SYSTEM | ATTR_READ_ONLY | ATTR_VOLUME | ATTR_ARCHIVE;
char name[] = "*.mp3";
unsigned char sector[32];
unsigned short int currentVolume, scibass=0, memtest=0, clock=0, scimode=0, scistatus=0, hdat0=0, hdat1=0, samplerate=0;
#define MAXBUFFERSIZE 32

void InitClock(void);

int main(void)
{   

    int findfirstflag = 0;
    
    int SDinit_flag = 0;
    FSFILE *MyFile;
    char ptr[100];
    int nitems = 0;
    
    // initialize SD card
    SDinit_flag = FSInit();

    if ( SDinit_flag == False)
    {     
    // fail to initialize SD card
    }
    // Find the first mp3 in the sd card
    
    findfirstflag = FindFirst (name, attributes, &file);
    if ( findfirstflag == True)
    {
       // find song fail    
    }
    
    // read the data from the song found, transfer it to the buffer
    
    MyFile = FSfopen( file.filename,"r");
    

    nitems = FSfread((void*)ptr,10,1,MyFile );   

    FSfclose(MyFile);
    
    while(1);
}


