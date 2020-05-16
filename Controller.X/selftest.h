/* 
 * File:   selftest.h
 * Author: David Ortiz
 *
 */

#ifndef SELFTEST_H
#define	SELFTEST_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"

extern uint16_t freeFlowRateF, freeFlowRateM, freeFlowRateL;
  
// Initialization procedure.
// Self-test.
bool SelfTest(void);
  
#ifdef	__cplusplus
}
#endif

#endif


