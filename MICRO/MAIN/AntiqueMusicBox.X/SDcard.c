/*
 * File:   SDcard.c
 * Author: ECE477 Team8 Antique Music Box
 * This file contain the SDcard function for the Antique Music Box
 * Created on October 31, 2015, 12:00 AM
 */
#include "SDcard.h"

#include "p33Fxxxx.h"

#define OK 0x00
#define PRINT_ERR 0x01
#define INIT_ERR 0x02
#define SD_ERR 0x03

#define SD_BLOCK_SIZE 512

//internal macros
#define HEX 16
#define READ_CMD 17
#define DUMMY 0xFF
#define START_BLOCK_TOKEN 0xFE
// R1 Response Codes (from SD Card Product Manual v1.9 section 5.2.3.1)
#define R1_IN_IDLE_STATE    (1<<0)   // The card is in idle state and running initializing process.
#define R1_ERASE_RESET      (1<<1)   // An erase sequence was cleared before executing because of an out of erase sequence command was received.
#define R1_ILLEGAL_COMMAND  (1<<2)  // An illegal command code was detected
#define R1_COM_CRC_ERROR    (1<<3)   // The CRC check of the last command failed.
#define R1_ERASE_SEQ_ERROR  (1<<4)  // An error in the sequence of erase commands occured.
#define R1_ADDRESS_ERROR    (1<<5)  // A misaligned address, which did not match the block length was used in the command.
#define R1_PARAMETER        (1<<6)  // The command's argument (e.g. address, block length) was out of the allowed range for this card.

//ports
/*#define LED PORTCbits.RC3           //GPIO
#define LED_DIR TRISCbits.TRISC3 */
#define SD_CS PORTCbits.RC1
#define SD_CS_DIR TRISCbits.TRISC1
#define SDI PORTGbits.RG8
#define SDI_DIR TRISGbits.TRISG8
#define SCK PORTGbits.RG6
#define SCK_DIR TRISGbits.TRISG6
#define SDO PORTGbits.RG7
#define SDO_DIR TRISGbits.TRISG7
#define SD_Enable() SD_CS = 0 
#define SD_Disable() SD_CS = 1 
//#define SPIFastClock() SPI2CON = 0x007F

//internal types
typedef unsigned char buffer_typ;
buffer_typ buffer[SD_BLOCK_SIZE];

unsigned long curr_block;
unsigned char synced;
unsigned long total_blocks;

//internal function definions
unsigned char SDReadBlock( unsigned long );
unsigned char SDWriteBlock(unsigned long );
void InitSPI( void );
unsigned char InitSD( void );
unsigned char SD_WriteCommand(unsigned char* cmd);
unsigned char SPIRead( void );
void SPIWrite(unsigned char data);

//functions
int currBlock = 1;
void WriteSamples(unsigned char* buff)
{
  int i = 0;
  //put in write queue
  for(i = 0; i < SD_BLOCK_SIZE; i++)
  {
    buffer[i] = *buff;
    buff++;
  }
  SDWriteBlock(currBlock);
  SDReadBlock(currBlock);
  currBlock++;
  return;
}

unsigned char setup_SDSPI( void )
{
  unsigned char res = 0;
  unsigned long start_block = 0xD6D6;

  /* Set Up UART */
  //U1BRG=216;
  //U1MODE=0x8000; /* Enable, 8data, no parity, 1 stop    */
  //U1STA =0x8400; /* Enable TX                           */
  // Configure output pins
  /*LED_DIR = 0;
  LED = 0; */

  total_blocks = 1;  //this is necessary!!

  InitSPI();
  res = InitSD();
  do
  {
    while(res)
    {
      res = InitSD(); 
      //LED = 0;
    }
    res = SDReadBlock(start_block);
  } while(res);

  //LED = 1;
/*
  unsigned char curr = 'M';
  int i = 0;
  for(i = 0; i < SD_BLOCK_SIZE; i++)
  {
    buffer1[i] = curr;
    if(curr++ == 'Z') curr = 'A';
  } */

  /* test */
  SDWriteBlock(0xB0);
  SDReadBlock(0xB0);
  //PrintDebug(0xB0);
  //PrintBlock();

  curr_block = 1;
  synced = 0;

  return OK;
}

void SD_setStart( void )
{
  curr_block = 1;
  return;
}

unsigned char SD_GetSample( int cnt )
{
  return buffer[cnt];
}

void SD_SetSample( int cnt, unsigned char sample )
{
  buffer[cnt] = sample;
  return;
}

void SD_writeCurr( void )
{
  SDWriteBlock(curr_block);
  curr_block++;
  return;
}

void SD_readCurr( void )
{
  SDReadBlock(curr_block);
  curr_block++;
  return;
}

unsigned char SDReadBlock(unsigned long block)
{
  buffer_typ* theData;
  unsigned char read_cmd[6];
  unsigned char status = 0x0;
  unsigned int offset = 0;
  unsigned char res = 1;
/*
  if(block >= total_blocks)
  { 
    //too large for small disc
    return SD_ERR;
  } */
 
  while(res)
  {
    res = InitSD();
  }

  //LED = 0;

  //send the read command
  block = block * SD_BLOCK_SIZE; //need to be correct offset
  read_cmd[0] = READ_CMD;
  read_cmd[1] = ((block & 0xFF000000) >> 24);
  read_cmd[2] = ((block & 0x00FF0000) >> 16);
  read_cmd[3] = ((block & 0x0000FF00) >> 8 );
  read_cmd[4] = ((block & 0x000000FF)      );
  read_cmd[5] = DUMMY;
  SD_Enable();
  status = SD_WriteCommand(read_cmd);
  if(status != 0)
  {
    //printbyte(status);
    return SD_ERR;
  }

  //find the start of the read
  do
  {
    status = SPIRead();
  }while(status != START_BLOCK_TOKEN);

  //read the bytes
  theData = buffer;
  for(offset = 0; offset < SD_BLOCK_SIZE; offset++)
  {
    *theData = SPIRead();
    //printbyte(*theData);
    theData++;
  }
  SD_Disable();

  //pump for eight cycles according to spec
  SPIWrite(0xFF);

  //LED = 0;
  return OK;
}

unsigned char SDWriteBlock(unsigned long block)
{
    buffer_typ* theData;
	unsigned int i;
	unsigned char status;
    unsigned char res = 1;

    while(res)
    {
      res = InitSD();
    }

	unsigned char CMD24_WRITE_SINGLE_BLOCK[] = {24,0x00,0x00,0x00,0x00,0xFF};
    block = block * SD_BLOCK_SIZE; //need to be correct offset
	CMD24_WRITE_SINGLE_BLOCK[1] = ((block & 0xFF000000) >> 24);
	CMD24_WRITE_SINGLE_BLOCK[2] = ((block & 0x00FF0000) >> 16);
	CMD24_WRITE_SINGLE_BLOCK[3] = ((block & 0x0000FF00) >> 8);
	CMD24_WRITE_SINGLE_BLOCK[4] = ((block & 0x000000FF));

    SD_Enable();

	// Send the write command
	status = SD_WriteCommand(CMD24_WRITE_SINGLE_BLOCK);
    
	if(status != 0)
	{
        //printbyte(status);
		// ABORT: invalid response for write single command
		return 1;
	}

    //write data start token
	SPIWrite(0xFE); 
  
	//write all the bytes in the block
    theData = buffer;
	for(i = 0; i < SD_BLOCK_SIZE; ++i)
	{
		SPIWrite(*theData);
        //printbyte(*theData);
		theData++;
	}

   	// Write CRC bytes
	SPIWrite(0xFF);
	SPIWrite(0xFF); 

    //prints(msg);

    //wait to complete
    status = SPIRead();
    while(status != 0xFF)
    {
      //prints(msg);
      status = SPIRead();
    }
	
    SD_Disable();

    //wait 8 clock cycles
	SPIWrite(0xFF); 

    return(0);

}

//internal functions
unsigned char InitSD( void )
{
	unsigned int i = 0;
	unsigned char status;

	// Turn off SD Card
	SD_Disable();
	//SD_PowerOff();

	// Wait for power to really go down
	for(i = 0; i; i++);
	for(i = 0; i; i++);
	for(i = 0; i; i++);
	for(i = 0; i; i++);

	// Turn on SD Card
	//SD_PowerOn();

	// Wait for power to really come up
	for(status = 0; status < 10; ++status)
	{
		for(i = 0; i; i++);
		for(i = 0; i; i++);
		for(i = 0; i; i++);
		for(i = 0; i; i++);
	}

	// We need to give SD Card about a hundred clock cycles to boot up
	for(i = 0; i < 16; ++i)
	{
		SPIWrite(0xFF); // write dummy data to pump clock signal line
	}	

	SD_Enable();

	// This is the only command required to have a valid CRC
	// After this command, CRC values are ignore unless explicitly enabled using CMD59
	unsigned char CMD0_GO_IDLE_STATE[] = {0x00,0x00,0x00,0x00,0x00,0x95};

	// Wait for the SD Card to go into IDLE state
	i = 0;
	do
	{
		status = SD_WriteCommand(CMD0_GO_IDLE_STATE);

		// fail and return
		if(i++ > 50)
		{
			return 1;
		}
	} while( status != 0x01 );

	// Wait for SD Card to initialize
	unsigned char CMD1_SEND_OP_COND[] = {0x01,0x00,0x00,0x00,0x00,0xFF};

	i = 0;
	do
	{
		status = SD_WriteCommand(CMD1_SEND_OP_COND);
		if(i++ > 50)
		{
			return 2;
		}
	} while( (status & R1_IN_IDLE_STATE) != 0 );

    // Send CMD55, required to precede all "application specific" commands
	unsigned char CMD55_APP_CMD[] = {55,0x00,0x00,0x00,0x00,0xFF};
	status = SD_WriteCommand(CMD55_APP_CMD); // Do not check response here

	// Send the ACMD41 command to initialize SD Card mode (not supported by MMC cards)
	i = 0;
	unsigned char ACMD41_SD_SEND_OP_COND[] = {41,0x00,0x00,0x00,0x00,0xFF};
	do
	{
		status = SD_WriteCommand(ACMD41_SD_SEND_OP_COND);
		// Might return 0x04 for Invalid Command if MMC card is connected

		if(i++ > 50)
		{
			return 3;
		}
	} while( (status & R1_IN_IDLE_STATE) != 0 );

	// Set the SPI bus to full speed now that SD Card is initialized in SPI mode
	SD_Disable();
	//SPIFastClock();

	return 0;
}

void InitSPI(void)
{
	//SD_PowerOff();
	//SD_PWR_DIR = 0;	 // output
	//SD_PowerOff();

	SD_Disable();
	SD_CS_DIR = 0; // output
	SD_Disable();

	SDI_DIR = 1; // input
	SCK_DIR = 1;
	SDO_DIR = 1;

	// set SPI port to slowest setting
	// master mode
	// 8 bit
	// Idle state for Clock is high level
	// Primary prescaler 64:1
	// Secondary prescaler 8:1
	SPI2CON1 = 0x0060;
    SPI2CON2 = 0x0000;
	SPI2STAT = 0x8000; // enable SPI port
}

unsigned char SD_WriteCommand(unsigned char* cmd)
{
	unsigned int i;
	unsigned char response;
	unsigned char savedSD_CS = SD_CS;

	// SD Card Command Format
	// (from Section 5.2.1 of SanDisk SD Card Product Manual v1.9).
	// Frame 7 = 0
	// Frame 6 = 1
	// Command (6 bits)
	// Address (32 bits)
	// Frame 0 = 1

	// Set the framing bits correctly (never change)
	cmd[0] |= (1<<6);
	cmd[0] &= ~(1<<7);
	cmd[5] |= (1<<0);
	
	// Send the 6 byte command
	SD_Enable();
	for(i = 0; i < 6; ++i)
	{
		SPIWrite(*cmd);
		cmd++;
	}
	
	// Wait for the response
	i = 0;
	do
	{
		response = SPIRead();

		if(i > 60000)  //instead of 100
		{
			break;
		}
		i++;
	} while(response == 0xFF);

	SD_Disable();

	// Following any command, the SD Card needs 8 clocks to finish up its work.
	// (from SanDisk SD Card Product Manual v1.9 section 5.1.8)
	SPIWrite(0xFF); 

	SD_CS = savedSD_CS;
	return(response);
}

void SPIWrite(unsigned char data)
{
	// DO NOT WAIT FOR SPITBF TO BE CLEAR HERE
	// (for some reason, it doesn't work on this side of the write data).

	// Write the data!
	SPI2BUF = data;

	// Wait until send buffer is ready for more data.
	while(SPI2STATbits.SPITBF);
}

unsigned char SPIRead(void)
{
	unsigned char data;

	if(SPI2STATbits.SPIRBF)
	{
		// already have some data to return, don't initiate a read
		data = SPI2BUF;

		SPI2STATbits.SPIROV = 0;
		return data;
	}

	// We don't have any data to read yet, so initiate a read
	SPI2BUF = 0xFF;  // write dummy data to initiate an SPI read
	while(SPI2STATbits.SPITBF); // wait until the data is finished reading
	data = SPI2BUF;

	SPI2STATbits.SPIROV = 0;
	return data;
}
