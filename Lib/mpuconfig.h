///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// Filename:        mpconfig.h
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.40
//
// Holds MPU hardware configuration switches. Refer to MPU datasheet and 
// P18F23K20.INC in C18 directory for details
///////////////////////////////////////////////////////////////////////////////

#ifndef MPUCONFIG_H
#define MPUCONFIG_H

/// I N C L U D E S ///////////////////////////////////////////////////////////
#include <p18f26k20.h>

/// M P U  C O N F I G U R A T I O N  S E T T I N G S /////////////////////////
#pragma config FOSC = INTIO67	//Internal oscillator block, port function on RA6 and RA7
#pragma config FCMEN = ON
//#pragma config FCMEN = OFF		//Fail-Safe Clock Monitor disabled
#pragma config IESO = OFF		//Oscillator Switchover mode disabled
#pragma config PWRT = ON		//Power up timer enabled
#pragma config BOREN = OFF
//#pragma config BOREN = ON		//Brown-out Reset enabled and controlled by software (SBOREN is enabled)
#pragma config BORV = 27		//VBOR set to 2.7V nominal
#pragma config WDTEN = OFF		//WDT is controlled by SWDTEN bit of the WDTCON register
#pragma config WDTPS = 32768	//WDT post-scale 1:32768
#pragma config CCP2MX = PORTC	//CCP2 input/output is multiplexed with RC1
#pragma config PBADEN = OFF		//PORTB<4:0> pins are configured as digital I/O on Reset
#pragma config LPT1OSC = OFF	//Timer1 configured for higher power operation
#pragma config HFOFST = OFF		//The system clock is held off until the HFINTOSC is stable
#pragma config MCLRE = ON
//#pragma config MCLRE = OFF	//RE3 input pin enabled; MCLR disabled
#pragma config STVREN = ON
//#pragma config STVREN = OFF		//Stack full/underflow will not cause Reset
#pragma config LVP = OFF		//Single-Supply ICSP disabled
#pragma config XINST = OFF		//Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
#pragma config DEBUG = ON		//Enabled
//#pragma config DEBUG = OFF	//Disabled
#pragma config CP0 = OFF		//Block 0 (000800-001FFFh) not code-protected
#pragma config CP1 = OFF		//Block 1 (002000-003FFFh) not code-protected
#pragma config CP2 = OFF		//Block 2 (008000-00BFFFh) not code-protected
#pragma config CP3 = OFF		//Block 3 (00C000-00FFFFh) not code-protected
#pragma config CPB = OFF		//Boot block (000000-0007FFh) not code-protected
#pragma config CPD = OFF		//Data EEPROM not code-protected
#pragma config WRT0 = OFF		//Block 0 (000800-001FFFh) not write-protected
#pragma config WRT1 = OFF		//Block 1 (002000-003FFFh) not write-protected
#pragma config WRT2 = OFF		//Block 2 (008000-00BFFFh) not write-protected
#pragma config WRT3 = OFF		//Block 3 (00C000h-00FFFFh) not write-protected
#pragma config WRTB = OFF		//Boot block (000000-0007FFh) not write-protected
#pragma config WRTC = OFF		//Configuration registers (300000-3000FFh) not write-protected
#pragma config WRTD = OFF		//Data EEPROM not write-protected
#pragma config EBTR0 = OFF		//Block 0 (000800-001FFFh) not protected from table reads executed in other blocks
#pragma config EBTR1 = OFF		//Block 1 (002000-003FFFh) not protected from table reads executed in other blocks
#pragma config EBTR2 = OFF		//Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks
#pragma config EBTR3 = OFF		//Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks
#pragma config EBTRB = OFF		//Boot block (000000-0007FFh) not protected from table reads executed in other blocks

#endif //MPUCONFIG_H
