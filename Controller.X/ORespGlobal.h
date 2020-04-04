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

//#define DEBUG
    
#ifdef DEBUG
#define ERROR_CONDITION(k) 
#else
#define ERROR_CONDITION(k) while(1)
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* ORESPGLOBAL_H */

