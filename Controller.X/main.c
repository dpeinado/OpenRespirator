/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.80.0
        Device            :  PIC18F46K42
        Driver Version    :  2.00
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

#include "mcc_generated_files/mcc.h"
#include "ORespGlobal.h"
#include "aCapture.h"

void putch(char byte)
{
    while (!UART1_is_tx_ready());
    UART1_Write(byte);
}

// TODO: ENSURE USE WITH TIME ABOVE 1/2 COUNT GIVES ERROR.
#define TIME_MS(time) ((uint16_t) time*10)
#define TIME_S(time) ((uint16_t) (time*10000))

// TIME function. Maintains global time. Resolution 0.1ms. Span about 6s. 
uint16_t timeGet(void){
    return TMR1_ReadTimer();
}

// Checks if time has finish and refresh tstamp.
bool timeElapsedR(uint16_t *prevTime, uint16_t duration){
    uint16_t intTime, intDur;
    intTime = TMR1_ReadTimer();
    intDur = intTime - *prevTime;
    if (intDur < duration) {
        return false;
    } else {
        *prevTime = intTime;
        return true;
    }
}

// Only checks if time hasfinished.
bool timeElapsed(uint16_t prevTime, uint16_t duration){
    uint16_t intTime, intDur;
    intTime = TMR1_ReadTimer();
    intDur = intTime - prevTime;
    if (intDur < duration) {
        return false;
    } else {
        return true;
    }
}

/*
                         Main application
 */
void main(void)
{
    int16_t mainPressure;
    uint16_t tstamp1;
    
    // Initialize the device
    SYSTEM_Initialize();

    // Disable ADC Irq.
    PIE1bits.ADIE = 0;
    
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:
    INTERRUPT_GlobalInterruptEnable();
    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Initialize.
    TMR0_StartTimer();
    TMR1_StartTimer();
    aCaptureInit();
    
    IO_RA2_LAT=0;
    tstamp1 = timeGet();
    
    while (1)
    {
        if (aCaptGetResult(MainPSensor,&mainPressure)){
//        if (ADCC_IsConversionDone()){
//            adcCaptRst=ADCC_GetConversionResult();
//            ADCC_StartConversion(channel_ANE2);
            printf("P %d  \r", mainPressure);
        }

        if (timeElapsedR(&tstamp1, TIME_S(0.5))){
            // DORDBG. SET RA2 TO 1.
            IO_RA2_Toggle();
            printf("\nPrueba\n");
        }
        // Add your application code
    }
}
/**
 End of File
*/