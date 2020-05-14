/* 
 * File:   brate.h
 * Author: David Ortiz
 *
 */

#ifndef BRATE_H
#define	BRATE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"

  void bRateInit(void);
  void bRateUpdt(bool triggered);
  uint8_t bRateGet(void);  

#ifdef	__cplusplus
}
#endif

#endif


