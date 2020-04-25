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
// Write to bottom row, either full row or only left half.
// Write to bottom row, five most right characters (to show monitor error).
extern bool lcdPrintBR;  
extern bool lcdPrintBRR;
extern char lcdTopRow[20];
extern char lcdBtnRow[20];
extern char lcdBtnRRow[10];
    
typedef enum {
    CFG_IDLE,
    CFG_IP,
    CFG_BPM,
    CFG_PEEP,
    CFG_MAXP,
    CFG_MAXV,
    CFG_LOWVA,
    CFG_HIGHVA,
    CFG_POWEROFF
} menuStatusT;

void MenuInit(void);

void MenuMng(void);

void screenInit(void);

void screenMng(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MENU_H */

