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
    
typedef enum {
    CFG_IDLE,
    CFG_IP,
    CFG_BPM,
    CFG_PEEP
} menuStatusT;

void MenuInit(void);

void MenuMng(void);

void screenInit(void);

void screenMng(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MENU_H */

