/* 
 * File:   controller.h
 * Author: carlospardo
 *
 * Created on April 22, 2020, 1:24 PM
 */

#ifndef CONTROLLER_H
#define	CONTROLLER_H

#ifdef	__cplusplus
extern "C" {
#endif

void ControllerInit(void);
void ControllerCheckTask(void);
void DumpI2C(void);


#ifdef	__cplusplus
}
#endif

#endif	/* CONTROLLER_H */

