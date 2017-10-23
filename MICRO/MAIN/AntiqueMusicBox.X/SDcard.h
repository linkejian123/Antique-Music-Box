/* 
 * File:   SDcard.h
 * Author: EE63PC9-user
 *
 * Created on October 31, 2015, 3:15 PM
 */

#ifndef SDCARD_H
#define	SDCARD_H

#ifdef	__cplusplus
extern "C" {
#endif


unsigned char setup_SDSPI( void );
void SD_setStart( void );
unsigned char SD_GetSample( int );
void SD_SetSample( int, unsigned char );
void SD_writeCurr( void );
void SD_readCurr( void );



#ifdef	__cplusplus
}
#endif

#endif	/* SDCARD_H */

