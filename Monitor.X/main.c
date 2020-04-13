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
#include "lcd.h"
#include "buttons.h"
#include "alarm.h"
#include "buzzer.h"
#include "tick.h"

/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    PWM6CON = 0x00;

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    printf ("\033c Hello world!!!\r\n New version \r\n");
    
    InitDisplay();
    AlarmInit();
    InitializePressure();
    //tick_init();
    ButtonInit();
    
    // Interrupt driven tasks:
    //  + ADC acquisition using TIMER 0
    //  + I2C reception
    //  + Buzzer alarm generation TIMER 2 for tone; TIMER 4 for sequences
    //  + Display message generation TIMER 5
    
    while (1)
    {
        // Add your application code
//        InputTargetsTask();             // Update targets from Controller via I2C

        DisplayTask();
        
        if (UART1_is_rx_ready())
        {
            char ch = getch();
            if (ch=='a') BuzzerTest('A');
            if (ch=='b') BuzzerTest('B');
            if (ch=='c') BuzzerTest('C');
            if (ch=='d') BuzzerTest('D');
            if (ch=='e') BuzzerTest('E');
            if (ch=='f') BuzzerTest('F');
            if (ch=='o') BuzzerTest('O');
            if (ch=='h') HistAlarm();
            if (ch=='m') MuteAlarm();
            if (ch=='i') I2CSend(1,1,1,0xF0);
            if (ch=='v') SetSV1(false);
            if (ch=='V') SetSV1(true);
            if (ch=='0') TestAlarm(0);
            if (ch=='1') TestAlarm(1);
            if (ch=='2') TestAlarm(2);
            if (ch=='3') TestAlarm(3);
            if (ch=='4') TestAlarm(4);
            if (ch=='5') TestAlarm(5);
            if (ch=='6') TestAlarm(6);
            if (ch=='7') TestAlarm(7);
            if (ch=='8') TestAlarm(8);
            if (ch=='9') TestAlarm(9);

            if (ch=='l') printf("\r\nADC: %d %03X %lu %lu\r\n", ADCC_GetConversionResult(), ADCC_GetConversionResult(), tick_get(), tick_get_slow());
            if (ch=='p') MonitorDump();
            if (ch=='z') SetCalibrateState(false);
            if (ch=='Z') SetCalibrateState(true);
            if (ch) {
                //putch(ch);
                //putch('\n');
            }
        }
        _delay(10);
    }
}
/**
 End of File
*/