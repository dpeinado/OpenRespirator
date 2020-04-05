/* 
 * File:   ORespGlobal.h
 * Author: David
 *
 * Created on April 4, 2020, 12:56 PM
 */

#ifndef ORESPGLOBAL_H
#define	ORESPGLOBAL_H

#ifdef	__cplusplus
extern "C" {
#endif

#define SV2ISOPEN LATAbits.LATA2
#define OPEN_SV2 LATAbits.LATA2 = 1
#define CLOSE_SV2 LATAbits.LATA2 = 0

// TBD.
#define SV3ISOPEN 0
#define OPEN_SV3 
#define CLOSE_SV3 

    
#define DEBUG
    
#ifdef DEBUG
#define ERROR_CONDITION(k) CLOSE_SV2;OPEN_SV3;printf("Fatal %d",k)
#else
#define ERROR_CONDITION(k) while(1)
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* ORESPGLOBAL_H */

