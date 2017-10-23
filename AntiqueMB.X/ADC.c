/**********************************************************************
* © 2005 Microchip Technology Inc.
*
* FileName:        adcDrv1.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Fxxxx
* Compiler:        MPLAB® C30 v3.00 or higher
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Settu D 			07/09/06  First release of source file
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
**********************************************************************/

#include "p33fxxxx.h"
#include <dsp.h>
#include "ADC.h"
#include "VFDbar.h"


// Set the twiddle factor for fft
//const fractcomplex twiddleFactors[] __attribute__ ((space(auto_psv), aligned (fftPoints * 2))) = {
//	0x7FFF, 0x0000, 0x7F62, 0xF374, 0x7D8A, 0xE707, 0x7A7D, 0xDAD8,
//	0x7642, 0xCF04, 0x70E3, 0xC3A9, 0x6A6E, 0xB8E3, 0x62F2, 0xAECC,
//	0x5A82, 0xA57E, 0x5134, 0x9D0E, 0x471D, 0x9592, 0x3C57, 0x8F1D,
//	0x30FC, 0x89BE, 0x2528, 0x8583, 0x18F9, 0x8276, 0x0C8C, 0x809E,
//	0x0000, 0x8000, 0xF374, 0x809E, 0xE707, 0x8276, 0xDAD8, 0x8583,
//	0xCF04, 0x89BE, 0xC3A9, 0x8F1D, 0xB8E3, 0x9592, 0xAECC, 0x9D0E,
//	0xA57D, 0xA57D, 0x9D0E, 0xAECC, 0x9592, 0xB8E3, 0x8F1D, 0xC3A9,
//	0x89BE, 0xCF04, 0x8583, 0xDAD8, 0x8276, 0xE707, 0x809E, 0xF374
//};

//int DMA_counter = 0;
//
//fractional	inputData[fftPoints];
//// Initial FFTdata
//fractcomplex fftData[fftPoints] __attribute__ ((section (".ybss, bss, ymemory"), aligned (fftPoints * 2 * 2)));
//fractional	powerSpec[fftPoints/2];

// Number of locations for ADC buffer = 14 words
// Align the buffer to 128 words or 256 bytes. This is needed for peripheral indirect mode
//int  BufferA[SAMP_BUFF_SIZE] __attribute__((space(dma),aligned(256)));
//int  BufferB[SAMP_BUFF_SIZE] __attribute__((space(dma),aligned(256)));
// Number of locations for ADC buffer = 2 (AN4 and AN5) x 16 = 32 words
// Align the buffer to 32words or 64 bytes. This is needed for peripheral indirect mode


//int  BufferA[MAX_CHNUM+1][SAMP_BUFF_SIZE] __attribute__((space(dma),aligned(64)));
//int  BufferB[64];

int	peakFrequencyBin = 0;				/* Declare post-FFT variables to compute the */
unsigned long peakFrequency = 0;			/* frequency of the largest spectral component */


void FFT(int* inputData);


/*=============================================================================
ADC Initialisation for Channel Scan 
=============================================================================*/
//void initAdc1(void)
//{
//		AD1CON1bits.FORM   = 3;		// Data Output Format: Signed Fraction (Q15 format)
//		AD1CON1bits.SSRC   = 2;		// Sample Clock Source: GP Timer starts conversion
//		AD1CON1bits.ASAM   = 1;		// ADC Sample Control: Sampling begins immediately after conversion
//		AD1CON1bits.AD12B  = 0;		// 10-bit ADC operation
//	
//
//		AD1CON2bits.ALTS=1;			// Alternate Input Sample Mode Select Bit
//		AD1CON2bits.CHPS  = 0;		// Converts CH0
//
//		AD1CON3bits.ADRC = 0;		// ADC Clock is derived from Systems Clock
//		AD1CON3bits.ADCS = 1300;		// ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*64 = 1.6us (625Khz)
//									// ADC Conversion Time for 10-bit Tc=12*Tab = 19.2us	
//
//		AD1CON1bits.ADDMABM = 0; 	// DMA buffers are built in scatter/gather mode
//		AD1CON2bits.SMPI    = 0;	// SMPI Must be programmed to 1 for this case
//		AD1CON4bits.DMABL   = 6;	// Each buffer contains 64 words
//
//        //AD1CHS0: A/D Input Select Register
//        AD1CHS0bits.CH0SA = 10;		// MUXA +ve input selection (AIN10) for CH0
//		AD1CHS0bits.CH0NA=0;		// MUXA -ve input selection (Vref-) for CH0
//        AD1CSSLbits.CSS10 = 1;   // select AN10 for input scan
//        
////      AD1CHS0bits.CH0SB=10;		// MUXB +ve input selection (AIN5) for CH0
////		AD1CHS0bits.CH0NB=0;		// MUXB -ve input selection (Vref-) for CH0
//				
//        //AD1PCFGH/AD1PCFGL: Port Configuration Register
//	//	AD1PCFGL=0xFFFF;
//	//	AD1PCFGH=0xFFFF;
//        AD1PCFGLbits.PCFG10 = 0;		// AN10 as Analog Input
//    //  AD1PCFGLbits.PCFG10 = 0;		// AN5 as Analog Input		
//        
//        IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
//        IEC0bits.AD1IE = 0;			// Do Not Enable A/D interrupt 
//        AD1CON1bits.ADON = 1;		// Turn on the A/D converter	
//
//
//}

/*=============================================================================  
Timer 3 is setup to time-out every 125 microseconds (8Khz Rate). As a result, the module 
will stop sampling and trigger a conversion on every Timer3 time-out, i.e., Ts=125us. 
=============================================================================*/
//void initTmr3() 
//{
////        TMR3 = 0x0000;
////        PR3 = 17578;
////        IFS0bits.T3IF = 0;
////        IEC0bits.T3IE = 0;
////
////        //Start Timer 3
////        T3CONbits.TON = 1;
//    
//    T3CONbits.TON = 0;     // Disable Timer
//    T3CONbits.TCS = 0;     // Select internal instruction cycle clock
//    T3CONbits.TGATE = 0;   // Disable Gated Timer mode
//    T3CONbits.TCKPS = 0b11;// Select 1:256 Prescaler
//
//    TMR3 = 0x00;           // Clear timer register
//    PR3 = 2000;            
//
//    IPC2bits.T3IP = 0x03;  // Set Timer2 Interrupt Priority Level
//    IFS0bits.T3IF = 0;     // Clear Timer2 Interrupt Flag
//    IEC0bits.T3IE = 1;     // Enable Timer2 interrupt
//    T3CONbits.TON = 1;
//
//}


// DMA0 configuration
// Direction: Read from peripheral address 0-x300 (ADC1BUF0) and write to DMA RAM 
// AMODE: Peripheral Indirect Addressing Mode
// MODE: Continuous, Ping-Pong Mode
// IRQ: ADC Interrupt
// ADC stores results stored alternatively between DMA_BASE[0]/DMA_BASE[16] on every 16th DMA request 

//void initDma0(void)
//{
//	DMA0CONbits.AMODE = 2;			// Configure DMA for Peripheral indirect mode
//	DMA0CONbits.MODE  = 2;			// Configure DMA for Continuous Ping-Pong mode
//	IPC1bits.DMA0IP = 1;
//    
//	DMA0PAD=(int)&ADC1BUF0;
//	DMA0CNT = (SAMP_BUFF_SIZE*2)-1;					
//
//	DMA0REQ=13;
//
//	DMA0STA = __builtin_dmaoffset(BufferA);		
//	//DMA0STB = __builtin_dmaoffset(BufferB);
//
//	IFS0bits.DMA0IF = 0;			//Clear the DMA interrupt flag bit
//    IEC0bits.DMA0IE = 1;			//Set the DMA interrupt enable bit
//
//	DMA0CONbits.CHEN=1;
//
//}

/*=============================================================================
_DMA0Interrupt(): ISR name is chosen from the device linker script.
=============================================================================*/

//
//void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
//{	
//    if(play == 1)
//    ProcessADCSamples(&BufferA[10][0]); 
////    int i;	 
////    for(i=0;i<16;i++)
////    {
////        BufferB[j] = BufferA[10][i];
////        j++;
////    }
////	if( DMA_counter == 3)
////    {
////        ProcessADCSamples(&BufferB);
////        DMA_counter = 0;
////        j = 0;
////    }
////    DMA_counter ++;
//    
//    IFS0bits.DMA0IF = 0;		//Clear the DMA0 Interrupt Flag
//}

//void ProcessADCSamples(int * AdcBuffer)
//{
//	/* Do something with ADC Samples */ 
//    int indata;
//    int writedata;
//    for(writedata = 0; writedata < fftPoints; writedata++){
//        indata = *AdcBuffer++ - 2048;      //
//        inputData[writedata] = indata << 4;
//        if(writedata == fftPoints - 1){
//            FFT(inputData);
//            writedata = 0;
//        }
//    }
//}

//void FFT(int * inputData)
//{
//    int	VFDdata[10];
//    fractional VFD[10];
//    int index = 0;
//    int i;
//    for(i = 0; i < fftPoints; i++) 
//    {
//        fftData[i].real = inputData[i];
//        fftData[i].imag = 0;
//    }
//
//    FFTComplexIP( log2N, fftData, (fractcomplex *)twiddleFactors,(int) __builtin_psvpage(&twiddleFactors[0]));
//    BitReverseComplex( log2N, fftData ); 
//    
//    
//    SquareMagnitudeCplx( fftPoints, fftData, powerSpec ); 
//    for(i=0;i<30;i++)
//    {
//        if((i%3)==2)
//        {
//            VFD[index] = (powerSpec[i] + powerSpec[i-1] + powerSpec[i-2]) / 3;
//            index++;
//        }
//    }
//    
//    for(index=0;index<10;index++)
//    {
//        if(VFD[index]>0x100)
//            VFDdata[index] = 0;
//        else if(VFD[index] > 0xDC)
//            VFDdata[index] = 1;
//        else if(VFD[index] > 0xB8)
//            VFDdata[index] = 2;
//        else if(VFD[index] > 0x94)
//            VFDdata[index] = 3;
//        else if(VFD[index] > 0x70)
//            VFDdata[index] = 4;
//        else if(VFD[index] > 0x4C)
//            VFDdata[index] = 5;
//        else if(VFD[index] > 0x28)
//            VFDdata[index] = 6;
//        else
//            VFDdata[index] = 7;
//    }
//    
//    VFDbar(VFDdata);
//    
////	/* Find the frequency Bin ( = index into the SigCmpx[] array) that has the largest energy*/
////	/* i.e., the largest spectral component */
////	VectorMax(fftPoints/2, powerSpec, &peakFrequencyBin);
////	/* Compute the frequency (in Hz) of the largest spectral component */
////	peakFrequency = peakFrequencyBin*(SAMPLING_RATE/fftPoints);
//    i = 0;
//}