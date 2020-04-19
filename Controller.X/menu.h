/* 
 * File:   menu.h
 * Author: David Ortiz
 *
 */

#ifndef MENU_H
#define	MENU_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"

#define MENU_TOUT TIME_S(5)

// Set to true to force lcd update.
extern bool lcdPrintTR;
extern bool lcdPrintBR;
extern char lcdTopRow[20];
extern char lcdBtnRow[20];
    
typedef enum {
    CFG_IDLE,
    CFG_IP,
    CFG_BPM,
    CFG_PEEP,
    CFG_MAXP,
    CFG_MAXV,
    CFG_LOWVA,
    CFG_HIGHVA,
} menuStatusT;

void MenuInit(void);

void MenuMng(void);

void screenInit(void);

void screenMng(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MENU_H */

