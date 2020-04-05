#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"

// This code generates alarm tones and alarm tones sequences

#define ALARM_NONE 0
#define ALARM_LOW  1 // E 200ms LOW 200ms C 200ms LOW 16s
#define ALARM_MED  2 // C 200ms LOW 200ms A 200ms LOW 200ms F 200ms LOW 8s
#define ALARM_HIGH 4 // C 180ms LOW 100ms A 180ms LOW 100ms F 180ms LOW 400ms A 180ms LOW 100ms F 180ms LOW 1s
                     // ... C 180ms LOW 100ms A 180ms LOW 100ms F 180ms LOW 400ms A 180ms LOW 100ms F 180ms LOW 4s

uint8_t alarmState;

#define BUZZER_OFF 0
#define BUZZER_LOW 1
#define BUZZER_A   2
#define BUZZER_B   3
#define BUZZER_C   4
#define BUZZER_D   5
#define BUZZER_E   6
#define BUZZER_F   7

uint8_t buzzerState;
#define PERIOD_A  0x8D  // 440 Hz
#define PERIOD_B  0xFC  // 246.94 Hz
#define PERIOD_C  0xEE  // 261.63 Hz
#define PERIOD_D  0xD4  // 293.66 Hz
#define PERIOD_E  0xBD  // 329.63 Hz
#define PERIOD_F  0xB2  // 349.23 Hz

void BuzzerInit (void) {
    // Setup Timer 2 for Tone generation
    // Prescaler to 1/64
    // OSC to Fosc/4
    // Setup Timer 4 for Tone sequence
    
}


void BuzzerOn (uint8_t period) {
    T2PR = period;
    PWM6CON = 0x80;
}

void BuzzerOff (void) {
    PWM6CON = 0x00;
}