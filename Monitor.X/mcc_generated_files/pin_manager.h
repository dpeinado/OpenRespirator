/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.0
        Device            :  PIC18F46K42
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.10 and above
        MPLAB 	          :  MPLAB X 5.35	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set PRSV aliases
#define PRSV_TRIS                 TRISAbits.TRISA0
#define PRSV_LAT                  LATAbits.LATA0
#define PRSV_PORT                 PORTAbits.RA0
#define PRSV_WPU                  WPUAbits.WPUA0
#define PRSV_OD                   ODCONAbits.ODCA0
#define PRSV_ANS                  ANSELAbits.ANSELA0
#define PRSV_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define PRSV_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define PRSV_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define PRSV_GetValue()           PORTAbits.RA0
#define PRSV_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define PRSV_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define PRSV_SetPullup()          do { WPUAbits.WPUA0 = 1; } while(0)
#define PRSV_ResetPullup()        do { WPUAbits.WPUA0 = 0; } while(0)
#define PRSV_SetPushPull()        do { ODCONAbits.ODCA0 = 0; } while(0)
#define PRSV_SetOpenDrain()       do { ODCONAbits.ODCA0 = 1; } while(0)
#define PRSV_SetAnalogMode()      do { ANSELAbits.ANSELA0 = 1; } while(0)
#define PRSV_SetDigitalMode()     do { ANSELAbits.ANSELA0 = 0; } while(0)

// get/set VALVE aliases
#define VALVE_TRIS                 TRISAbits.TRISA1
#define VALVE_LAT                  LATAbits.LATA1
#define VALVE_PORT                 PORTAbits.RA1
#define VALVE_WPU                  WPUAbits.WPUA1
#define VALVE_OD                   ODCONAbits.ODCA1
#define VALVE_ANS                  ANSELAbits.ANSELA1
#define VALVE_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define VALVE_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define VALVE_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define VALVE_GetValue()           PORTAbits.RA1
#define VALVE_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define VALVE_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define VALVE_SetPullup()          do { WPUAbits.WPUA1 = 1; } while(0)
#define VALVE_ResetPullup()        do { WPUAbits.WPUA1 = 0; } while(0)
#define VALVE_SetPushPull()        do { ODCONAbits.ODCA1 = 0; } while(0)
#define VALVE_SetOpenDrain()       do { ODCONAbits.ODCA1 = 1; } while(0)
#define VALVE_SetAnalogMode()      do { ANSELAbits.ANSELA1 = 1; } while(0)
#define VALVE_SetDigitalMode()     do { ANSELAbits.ANSELA1 = 0; } while(0)

// get/set MEAS5 aliases
#define MEAS5_TRIS                 TRISAbits.TRISA2
#define MEAS5_LAT                  LATAbits.LATA2
#define MEAS5_PORT                 PORTAbits.RA2
#define MEAS5_WPU                  WPUAbits.WPUA2
#define MEAS5_OD                   ODCONAbits.ODCA2
#define MEAS5_ANS                  ANSELAbits.ANSELA2
#define MEAS5_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define MEAS5_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define MEAS5_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define MEAS5_GetValue()           PORTAbits.RA2
#define MEAS5_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define MEAS5_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define MEAS5_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define MEAS5_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define MEAS5_SetPushPull()        do { ODCONAbits.ODCA2 = 0; } while(0)
#define MEAS5_SetOpenDrain()       do { ODCONAbits.ODCA2 = 1; } while(0)
#define MEAS5_SetAnalogMode()      do { ANSELAbits.ANSELA2 = 1; } while(0)
#define MEAS5_SetDigitalMode()     do { ANSELAbits.ANSELA2 = 0; } while(0)

// get/set MIC aliases
#define MIC_TRIS                 TRISAbits.TRISA3
#define MIC_LAT                  LATAbits.LATA3
#define MIC_PORT                 PORTAbits.RA3
#define MIC_WPU                  WPUAbits.WPUA3
#define MIC_OD                   ODCONAbits.ODCA3
#define MIC_ANS                  ANSELAbits.ANSELA3
#define MIC_SetHigh()            do { LATAbits.LATA3 = 1; } while(0)
#define MIC_SetLow()             do { LATAbits.LATA3 = 0; } while(0)
#define MIC_Toggle()             do { LATAbits.LATA3 = ~LATAbits.LATA3; } while(0)
#define MIC_GetValue()           PORTAbits.RA3
#define MIC_SetDigitalInput()    do { TRISAbits.TRISA3 = 1; } while(0)
#define MIC_SetDigitalOutput()   do { TRISAbits.TRISA3 = 0; } while(0)
#define MIC_SetPullup()          do { WPUAbits.WPUA3 = 1; } while(0)
#define MIC_ResetPullup()        do { WPUAbits.WPUA3 = 0; } while(0)
#define MIC_SetPushPull()        do { ODCONAbits.ODCA3 = 0; } while(0)
#define MIC_SetOpenDrain()       do { ODCONAbits.ODCA3 = 1; } while(0)
#define MIC_SetAnalogMode()      do { ANSELAbits.ANSELA3 = 1; } while(0)
#define MIC_SetDigitalMode()     do { ANSELAbits.ANSELA3 = 0; } while(0)

// get/set PRS aliases
#define PRS_TRIS                 TRISAbits.TRISA4
#define PRS_LAT                  LATAbits.LATA4
#define PRS_PORT                 PORTAbits.RA4
#define PRS_WPU                  WPUAbits.WPUA4
#define PRS_OD                   ODCONAbits.ODCA4
#define PRS_ANS                  ANSELAbits.ANSELA4
#define PRS_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define PRS_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define PRS_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define PRS_GetValue()           PORTAbits.RA4
#define PRS_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define PRS_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define PRS_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define PRS_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define PRS_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define PRS_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define PRS_SetAnalogMode()      do { ANSELAbits.ANSELA4 = 1; } while(0)
#define PRS_SetDigitalMode()     do { ANSELAbits.ANSELA4 = 0; } while(0)

// get/set MEAS12 aliases
#define MEAS12_TRIS                 TRISAbits.TRISA5
#define MEAS12_LAT                  LATAbits.LATA5
#define MEAS12_PORT                 PORTAbits.RA5
#define MEAS12_WPU                  WPUAbits.WPUA5
#define MEAS12_OD                   ODCONAbits.ODCA5
#define MEAS12_ANS                  ANSELAbits.ANSELA5
#define MEAS12_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define MEAS12_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define MEAS12_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define MEAS12_GetValue()           PORTAbits.RA5
#define MEAS12_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define MEAS12_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define MEAS12_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define MEAS12_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define MEAS12_SetPushPull()        do { ODCONAbits.ODCA5 = 0; } while(0)
#define MEAS12_SetOpenDrain()       do { ODCONAbits.ODCA5 = 1; } while(0)
#define MEAS12_SetAnalogMode()      do { ANSELAbits.ANSELA5 = 1; } while(0)
#define MEAS12_SetDigitalMode()     do { ANSELAbits.ANSELA5 = 0; } while(0)

// get/set RB4 procedures
#define RB4_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define RB4_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define RB4_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define RB4_GetValue()              PORTBbits.RB4
#define RB4_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define RB4_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define RB4_SetPullup()             do { WPUBbits.WPUB4 = 1; } while(0)
#define RB4_ResetPullup()           do { WPUBbits.WPUB4 = 0; } while(0)
#define RB4_SetAnalogMode()         do { ANSELBbits.ANSELB4 = 1; } while(0)
#define RB4_SetDigitalMode()        do { ANSELBbits.ANSELB4 = 0; } while(0)

// get/set RB5 procedures
#define RB5_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define RB5_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define RB5_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define RB5_GetValue()              PORTBbits.RB5
#define RB5_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define RB5_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define RB5_SetPullup()             do { WPUBbits.WPUB5 = 1; } while(0)
#define RB5_ResetPullup()           do { WPUBbits.WPUB5 = 0; } while(0)
#define RB5_SetAnalogMode()         do { ANSELBbits.ANSELB5 = 1; } while(0)
#define RB5_SetDigitalMode()        do { ANSELBbits.ANSELB5 = 0; } while(0)

// get/set RC0 procedures
#define RC0_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define RC0_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define RC0_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define RC0_GetValue()              PORTCbits.RC0
#define RC0_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define RC0_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define RC0_SetPullup()             do { WPUCbits.WPUC0 = 1; } while(0)
#define RC0_ResetPullup()           do { WPUCbits.WPUC0 = 0; } while(0)
#define RC0_SetAnalogMode()         do { ANSELCbits.ANSELC0 = 1; } while(0)
#define RC0_SetDigitalMode()        do { ANSELCbits.ANSELC0 = 0; } while(0)

// get/set RC1 procedures
#define RC1_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define RC1_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define RC1_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define RC1_GetValue()              PORTCbits.RC1
#define RC1_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define RC1_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define RC1_SetPullup()             do { WPUCbits.WPUC1 = 1; } while(0)
#define RC1_ResetPullup()           do { WPUCbits.WPUC1 = 0; } while(0)
#define RC1_SetAnalogMode()         do { ANSELCbits.ANSELC1 = 1; } while(0)
#define RC1_SetDigitalMode()        do { ANSELCbits.ANSELC1 = 0; } while(0)

// get/set LLED aliases
#define LLED_TRIS                 TRISCbits.TRISC5
#define LLED_LAT                  LATCbits.LATC5
#define LLED_PORT                 PORTCbits.RC5
#define LLED_WPU                  WPUCbits.WPUC5
#define LLED_OD                   ODCONCbits.ODCC5
#define LLED_ANS                  ANSELCbits.ANSELC5
#define LLED_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define LLED_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define LLED_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define LLED_GetValue()           PORTCbits.RC5
#define LLED_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define LLED_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define LLED_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define LLED_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define LLED_SetPushPull()        do { ODCONCbits.ODCC5 = 0; } while(0)
#define LLED_SetOpenDrain()       do { ODCONCbits.ODCC5 = 1; } while(0)
#define LLED_SetAnalogMode()      do { ANSELCbits.ANSELC5 = 1; } while(0)
#define LLED_SetDigitalMode()     do { ANSELCbits.ANSELC5 = 0; } while(0)

// get/set RS aliases
#define RS_TRIS                 TRISCbits.TRISC6
#define RS_LAT                  LATCbits.LATC6
#define RS_PORT                 PORTCbits.RC6
#define RS_WPU                  WPUCbits.WPUC6
#define RS_OD                   ODCONCbits.ODCC6
#define RS_ANS                  ANSELCbits.ANSELC6
#define RS_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define RS_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define RS_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define RS_GetValue()           PORTCbits.RC6
#define RS_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define RS_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define RS_SetPullup()          do { WPUCbits.WPUC6 = 1; } while(0)
#define RS_ResetPullup()        do { WPUCbits.WPUC6 = 0; } while(0)
#define RS_SetPushPull()        do { ODCONCbits.ODCC6 = 0; } while(0)
#define RS_SetOpenDrain()       do { ODCONCbits.ODCC6 = 1; } while(0)
#define RS_SetAnalogMode()      do { ANSELCbits.ANSELC6 = 1; } while(0)
#define RS_SetDigitalMode()     do { ANSELCbits.ANSELC6 = 0; } while(0)

// get/set E aliases
#define E_TRIS                 TRISCbits.TRISC7
#define E_LAT                  LATCbits.LATC7
#define E_PORT                 PORTCbits.RC7
#define E_WPU                  WPUCbits.WPUC7
#define E_OD                   ODCONCbits.ODCC7
#define E_ANS                  ANSELCbits.ANSELC7
#define E_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define E_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define E_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define E_GetValue()           PORTCbits.RC7
#define E_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define E_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define E_SetPullup()          do { WPUCbits.WPUC7 = 1; } while(0)
#define E_ResetPullup()        do { WPUCbits.WPUC7 = 0; } while(0)
#define E_SetPushPull()        do { ODCONCbits.ODCC7 = 0; } while(0)
#define E_SetOpenDrain()       do { ODCONCbits.ODCC7 = 1; } while(0)
#define E_SetAnalogMode()      do { ANSELCbits.ANSELC7 = 1; } while(0)
#define E_SetDigitalMode()     do { ANSELCbits.ANSELC7 = 0; } while(0)

// get/set BUZZ aliases
#define BUZZ_TRIS                 TRISDbits.TRISD0
#define BUZZ_LAT                  LATDbits.LATD0
#define BUZZ_PORT                 PORTDbits.RD0
#define BUZZ_WPU                  WPUDbits.WPUD0
#define BUZZ_OD                   ODCONDbits.ODCD0
#define BUZZ_ANS                  ANSELDbits.ANSELD0
#define BUZZ_SetHigh()            do { LATDbits.LATD0 = 1; } while(0)
#define BUZZ_SetLow()             do { LATDbits.LATD0 = 0; } while(0)
#define BUZZ_Toggle()             do { LATDbits.LATD0 = ~LATDbits.LATD0; } while(0)
#define BUZZ_GetValue()           PORTDbits.RD0
#define BUZZ_SetDigitalInput()    do { TRISDbits.TRISD0 = 1; } while(0)
#define BUZZ_SetDigitalOutput()   do { TRISDbits.TRISD0 = 0; } while(0)
#define BUZZ_SetPullup()          do { WPUDbits.WPUD0 = 1; } while(0)
#define BUZZ_ResetPullup()        do { WPUDbits.WPUD0 = 0; } while(0)
#define BUZZ_SetPushPull()        do { ODCONDbits.ODCD0 = 0; } while(0)
#define BUZZ_SetOpenDrain()       do { ODCONDbits.ODCD0 = 1; } while(0)
#define BUZZ_SetAnalogMode()      do { ANSELDbits.ANSELD0 = 1; } while(0)
#define BUZZ_SetDigitalMode()     do { ANSELDbits.ANSELD0 = 0; } while(0)

// get/set HIST aliases
#define HIST_TRIS                 TRISDbits.TRISD1
#define HIST_LAT                  LATDbits.LATD1
#define HIST_PORT                 PORTDbits.RD1
#define HIST_WPU                  WPUDbits.WPUD1
#define HIST_OD                   ODCONDbits.ODCD1
#define HIST_ANS                  ANSELDbits.ANSELD1
#define HIST_SetHigh()            do { LATDbits.LATD1 = 1; } while(0)
#define HIST_SetLow()             do { LATDbits.LATD1 = 0; } while(0)
#define HIST_Toggle()             do { LATDbits.LATD1 = ~LATDbits.LATD1; } while(0)
#define HIST_GetValue()           PORTDbits.RD1
#define HIST_SetDigitalInput()    do { TRISDbits.TRISD1 = 1; } while(0)
#define HIST_SetDigitalOutput()   do { TRISDbits.TRISD1 = 0; } while(0)
#define HIST_SetPullup()          do { WPUDbits.WPUD1 = 1; } while(0)
#define HIST_ResetPullup()        do { WPUDbits.WPUD1 = 0; } while(0)
#define HIST_SetPushPull()        do { ODCONDbits.ODCD1 = 0; } while(0)
#define HIST_SetOpenDrain()       do { ODCONDbits.ODCD1 = 1; } while(0)
#define HIST_SetAnalogMode()      do { ANSELDbits.ANSELD1 = 1; } while(0)
#define HIST_SetDigitalMode()     do { ANSELDbits.ANSELD1 = 0; } while(0)

// get/set MUTE aliases
#define MUTE_TRIS                 TRISDbits.TRISD2
#define MUTE_LAT                  LATDbits.LATD2
#define MUTE_PORT                 PORTDbits.RD2
#define MUTE_WPU                  WPUDbits.WPUD2
#define MUTE_OD                   ODCONDbits.ODCD2
#define MUTE_ANS                  ANSELDbits.ANSELD2
#define MUTE_SetHigh()            do { LATDbits.LATD2 = 1; } while(0)
#define MUTE_SetLow()             do { LATDbits.LATD2 = 0; } while(0)
#define MUTE_Toggle()             do { LATDbits.LATD2 = ~LATDbits.LATD2; } while(0)
#define MUTE_GetValue()           PORTDbits.RD2
#define MUTE_SetDigitalInput()    do { TRISDbits.TRISD2 = 1; } while(0)
#define MUTE_SetDigitalOutput()   do { TRISDbits.TRISD2 = 0; } while(0)
#define MUTE_SetPullup()          do { WPUDbits.WPUD2 = 1; } while(0)
#define MUTE_ResetPullup()        do { WPUDbits.WPUD2 = 0; } while(0)
#define MUTE_SetPushPull()        do { ODCONDbits.ODCD2 = 0; } while(0)
#define MUTE_SetOpenDrain()       do { ODCONDbits.ODCD2 = 1; } while(0)
#define MUTE_SetAnalogMode()      do { ANSELDbits.ANSELD2 = 1; } while(0)
#define MUTE_SetDigitalMode()     do { ANSELDbits.ANSELD2 = 0; } while(0)

// get/set LED aliases
#define LED_TRIS                 TRISDbits.TRISD3
#define LED_LAT                  LATDbits.LATD3
#define LED_PORT                 PORTDbits.RD3
#define LED_WPU                  WPUDbits.WPUD3
#define LED_OD                   ODCONDbits.ODCD3
#define LED_ANS                  ANSELDbits.ANSELD3
#define LED_SetHigh()            do { LATDbits.LATD3 = 1; } while(0)
#define LED_SetLow()             do { LATDbits.LATD3 = 0; } while(0)
#define LED_Toggle()             do { LATDbits.LATD3 = ~LATDbits.LATD3; } while(0)
#define LED_GetValue()           PORTDbits.RD3
#define LED_SetDigitalInput()    do { TRISDbits.TRISD3 = 1; } while(0)
#define LED_SetDigitalOutput()   do { TRISDbits.TRISD3 = 0; } while(0)
#define LED_SetPullup()          do { WPUDbits.WPUD3 = 1; } while(0)
#define LED_ResetPullup()        do { WPUDbits.WPUD3 = 0; } while(0)
#define LED_SetPushPull()        do { ODCONDbits.ODCD3 = 0; } while(0)
#define LED_SetOpenDrain()       do { ODCONDbits.ODCD3 = 1; } while(0)
#define LED_SetAnalogMode()      do { ANSELDbits.ANSELD3 = 1; } while(0)
#define LED_SetDigitalMode()     do { ANSELDbits.ANSELD3 = 0; } while(0)

// get/set D4 aliases
#define D4_TRIS                 TRISDbits.TRISD4
#define D4_LAT                  LATDbits.LATD4
#define D4_PORT                 PORTDbits.RD4
#define D4_WPU                  WPUDbits.WPUD4
#define D4_OD                   ODCONDbits.ODCD4
#define D4_ANS                  ANSELDbits.ANSELD4
#define D4_SetHigh()            do { LATDbits.LATD4 = 1; } while(0)
#define D4_SetLow()             do { LATDbits.LATD4 = 0; } while(0)
#define D4_Toggle()             do { LATDbits.LATD4 = ~LATDbits.LATD4; } while(0)
#define D4_GetValue()           PORTDbits.RD4
#define D4_SetDigitalInput()    do { TRISDbits.TRISD4 = 1; } while(0)
#define D4_SetDigitalOutput()   do { TRISDbits.TRISD4 = 0; } while(0)
#define D4_SetPullup()          do { WPUDbits.WPUD4 = 1; } while(0)
#define D4_ResetPullup()        do { WPUDbits.WPUD4 = 0; } while(0)
#define D4_SetPushPull()        do { ODCONDbits.ODCD4 = 0; } while(0)
#define D4_SetOpenDrain()       do { ODCONDbits.ODCD4 = 1; } while(0)
#define D4_SetAnalogMode()      do { ANSELDbits.ANSELD4 = 1; } while(0)
#define D4_SetDigitalMode()     do { ANSELDbits.ANSELD4 = 0; } while(0)

// get/set D5 aliases
#define D5_TRIS                 TRISDbits.TRISD5
#define D5_LAT                  LATDbits.LATD5
#define D5_PORT                 PORTDbits.RD5
#define D5_WPU                  WPUDbits.WPUD5
#define D5_OD                   ODCONDbits.ODCD5
#define D5_ANS                  ANSELDbits.ANSELD5
#define D5_SetHigh()            do { LATDbits.LATD5 = 1; } while(0)
#define D5_SetLow()             do { LATDbits.LATD5 = 0; } while(0)
#define D5_Toggle()             do { LATDbits.LATD5 = ~LATDbits.LATD5; } while(0)
#define D5_GetValue()           PORTDbits.RD5
#define D5_SetDigitalInput()    do { TRISDbits.TRISD5 = 1; } while(0)
#define D5_SetDigitalOutput()   do { TRISDbits.TRISD5 = 0; } while(0)
#define D5_SetPullup()          do { WPUDbits.WPUD5 = 1; } while(0)
#define D5_ResetPullup()        do { WPUDbits.WPUD5 = 0; } while(0)
#define D5_SetPushPull()        do { ODCONDbits.ODCD5 = 0; } while(0)
#define D5_SetOpenDrain()       do { ODCONDbits.ODCD5 = 1; } while(0)
#define D5_SetAnalogMode()      do { ANSELDbits.ANSELD5 = 1; } while(0)
#define D5_SetDigitalMode()     do { ANSELDbits.ANSELD5 = 0; } while(0)

// get/set D6 aliases
#define D6_TRIS                 TRISDbits.TRISD6
#define D6_LAT                  LATDbits.LATD6
#define D6_PORT                 PORTDbits.RD6
#define D6_WPU                  WPUDbits.WPUD6
#define D6_OD                   ODCONDbits.ODCD6
#define D6_ANS                  ANSELDbits.ANSELD6
#define D6_SetHigh()            do { LATDbits.LATD6 = 1; } while(0)
#define D6_SetLow()             do { LATDbits.LATD6 = 0; } while(0)
#define D6_Toggle()             do { LATDbits.LATD6 = ~LATDbits.LATD6; } while(0)
#define D6_GetValue()           PORTDbits.RD6
#define D6_SetDigitalInput()    do { TRISDbits.TRISD6 = 1; } while(0)
#define D6_SetDigitalOutput()   do { TRISDbits.TRISD6 = 0; } while(0)
#define D6_SetPullup()          do { WPUDbits.WPUD6 = 1; } while(0)
#define D6_ResetPullup()        do { WPUDbits.WPUD6 = 0; } while(0)
#define D6_SetPushPull()        do { ODCONDbits.ODCD6 = 0; } while(0)
#define D6_SetOpenDrain()       do { ODCONDbits.ODCD6 = 1; } while(0)
#define D6_SetAnalogMode()      do { ANSELDbits.ANSELD6 = 1; } while(0)
#define D6_SetDigitalMode()     do { ANSELDbits.ANSELD6 = 0; } while(0)

// get/set D7 aliases
#define D7_TRIS                 TRISDbits.TRISD7
#define D7_LAT                  LATDbits.LATD7
#define D7_PORT                 PORTDbits.RD7
#define D7_WPU                  WPUDbits.WPUD7
#define D7_OD                   ODCONDbits.ODCD7
#define D7_ANS                  ANSELDbits.ANSELD7
#define D7_SetHigh()            do { LATDbits.LATD7 = 1; } while(0)
#define D7_SetLow()             do { LATDbits.LATD7 = 0; } while(0)
#define D7_Toggle()             do { LATDbits.LATD7 = ~LATDbits.LATD7; } while(0)
#define D7_GetValue()           PORTDbits.RD7
#define D7_SetDigitalInput()    do { TRISDbits.TRISD7 = 1; } while(0)
#define D7_SetDigitalOutput()   do { TRISDbits.TRISD7 = 0; } while(0)
#define D7_SetPullup()          do { WPUDbits.WPUD7 = 1; } while(0)
#define D7_ResetPullup()        do { WPUDbits.WPUD7 = 0; } while(0)
#define D7_SetPushPull()        do { ODCONDbits.ODCD7 = 0; } while(0)
#define D7_SetOpenDrain()       do { ODCONDbits.ODCD7 = 1; } while(0)
#define D7_SetAnalogMode()      do { ANSELDbits.ANSELD7 = 1; } while(0)
#define D7_SetDigitalMode()     do { ANSELDbits.ANSELD7 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/