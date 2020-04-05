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

#include <stdio.h>
#include "mcc_generated_files/mcc.h"
#include "monitor.h"
#include "display.h"
#include "buttons.h"
#include "alarm.h"
#include "buzzer.h"

/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    printf ("Hello world!!!\r\n");
    
    InitDisplay();
    AlarmInit();
    
    // Interrupt driven tasks:
    //  + ADC acquisition
    //  + I2C reception
    //  + Buzzer alarm generation TIMER 2 for tone; TIMER 4 for sequences
    
    while (1)
    {
        // Add your application code
//        MonitorPressureTask();          // Update input information from pressure sensor
//        InputTargetsTask();             // Update targets from Controller via I2C
//        CalculateParametersTask();      // Calculate estimators: IP EP Tdi Tde BPM Volume etc
//        AlarmsDetectionTask();          // Compare Parameters with target and generate alarms
        ButtonTask();                   // Read user inputs
//        UpdateDisplayTask();            // Update display
        char ch = getch();
        if (ch=='a') BuzzerTest('A');
        if (ch=='b') BuzzerTest('B');
        if (ch=='c') BuzzerTest('C');
        if (ch=='d') BuzzerTest('D');
        if (ch=='e') BuzzerTest('E');
        if (ch=='f') BuzzerTest('F');
        if (ch=='o') BuzzerTest('O');
        if (ch=='1') AlarmSet(ALARM_LOW);
        if (ch=='2') AlarmSet(ALARM_MED);
        if (ch=='3') AlarmSet(ALARM_HIGH);
        if (ch=='0') AlarmSet(ALARM_NONE);
        if (ch) {
            putch(ch);
            //putch('\n');
        }
    }
}
/**
 End of File
*/