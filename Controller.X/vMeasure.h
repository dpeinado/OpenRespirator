/* 
 * File:   volMeas.h
 * Author: David Ortiz
 *
 * Created on April 10, 2020, 6:06 PM
 */

#ifndef VOLMEAS_H
#define	VOLMEAS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/tmr2.h"


    
void vMeasureRst(void);
void vMeasureInit(void);
uint16_t vMeasureGet(void);


#ifdef	__cplusplus
}
#endif

#endif	/* VOLMEAS_H */

