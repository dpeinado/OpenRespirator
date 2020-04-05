# 1 "time.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "time.c" 2
# 1 "./mcc_generated_files/tmr0.h" 1
# 54 "./mcc_generated_files/tmr0.h"
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\stdint.h" 1 3



# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\musl_xc8.h" 1 3
# 4 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\stdint.h" 2 3
# 22 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\stdint.h" 3
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 1 3
# 127 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned long uintptr_t;
# 142 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long intptr_t;
# 158 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 3
typedef signed char int8_t;




typedef short int16_t;




typedef __int24 int24_t;




typedef long int32_t;





typedef long long int64_t;
# 188 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long long intmax_t;





typedef unsigned char uint8_t;




typedef unsigned short uint16_t;




typedef __uint24 uint24_t;




typedef unsigned long uint32_t;





typedef unsigned long long uint64_t;
# 229 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned long long uintmax_t;
# 22 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\stdint.h" 2 3


typedef int8_t int_fast8_t;

typedef int64_t int_fast64_t;


typedef int8_t int_least8_t;
typedef int16_t int_least16_t;

typedef int24_t int_least24_t;

typedef int32_t int_least32_t;

typedef int64_t int_least64_t;


typedef uint8_t uint_fast8_t;

typedef uint64_t uint_fast64_t;


typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;

typedef uint24_t uint_least24_t;

typedef uint32_t uint_least32_t;

typedef uint64_t uint_least64_t;
# 139 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\stdint.h" 3
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/stdint.h" 1 3
typedef int32_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef uint32_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
# 139 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\stdint.h" 2 3
# 54 "./mcc_generated_files/tmr0.h" 2

# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\stdbool.h" 1 3
# 55 "./mcc_generated_files/tmr0.h" 2
# 100 "./mcc_generated_files/tmr0.h"
void TMR0_Initialize(void);
# 129 "./mcc_generated_files/tmr0.h"
void TMR0_StartTimer(void);
# 161 "./mcc_generated_files/tmr0.h"
void TMR0_StopTimer(void);
# 196 "./mcc_generated_files/tmr0.h"
uint8_t TMR0_ReadTimer(void);
# 235 "./mcc_generated_files/tmr0.h"
void TMR0_WriteTimer(uint8_t timerVal);
# 272 "./mcc_generated_files/tmr0.h"
void TMR0_Reload(uint8_t periodVal);
# 308 "./mcc_generated_files/tmr0.h"
_Bool TMR0_HasOverflowOccured(void);
# 1 "time.c" 2

# 1 "./mcc_generated_files/tmr1.h" 1
# 100 "./mcc_generated_files/tmr1.h"
void TMR1_Initialize(void);
# 129 "./mcc_generated_files/tmr1.h"
void TMR1_StartTimer(void);
# 161 "./mcc_generated_files/tmr1.h"
void TMR1_StopTimer(void);
# 196 "./mcc_generated_files/tmr1.h"
uint16_t TMR1_ReadTimer(void);
# 235 "./mcc_generated_files/tmr1.h"
void TMR1_WriteTimer(uint16_t timerVal);
# 271 "./mcc_generated_files/tmr1.h"
void TMR1_Reload(void);
# 310 "./mcc_generated_files/tmr1.h"
void TMR1_StartSinglePulseAcquisition(void);
# 349 "./mcc_generated_files/tmr1.h"
uint8_t TMR1_CheckGateValueStatus(void);
# 387 "./mcc_generated_files/tmr1.h"
_Bool TMR1_HasOverflowOccured(void);
# 2 "time.c" 2

# 1 "./time.h" 1
# 15 "./time.h"
typedef uint16_t time_t;






void timeInit(void);
time_t timeGet(void);
_Bool timeElapsedR(time_t *prevTime, time_t duration);
_Bool timeElapsed(time_t prevTime, time_t duration);
# 3 "time.c" 2


void timeInit(void){
    TMR0_StartTimer();
    TMR1_StartTimer();
}


time_t timeGet(void){
    return TMR1_ReadTimer();
}


_Bool timeElapsedR(time_t *prevTime, time_t duration){
    time_t intTime, intDur;
    intTime = TMR1_ReadTimer();
    intDur = intTime - *prevTime;
    if (intDur < duration) {
        return 0;
    } else {
        *prevTime = intTime;
        return 1;
    }
}


_Bool timeElapsed(time_t prevTime, time_t duration){
    time_t intTime, intDur;
    intTime = TMR1_ReadTimer();
    intDur = intTime - prevTime;
    if (intDur < duration) {
        return 0;
    } else {
        return 1;
    }
}
