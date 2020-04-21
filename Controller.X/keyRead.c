//if you hold/repeat a key for this amount of time we will consider it an escape
#ifndef ESCAPE_TIME
#define ESCAPE_TIME 2000
#endif

#define KEYN 10
#define KEYDN 6
#define KEYCN 4

#include "time.h"
#include "keyRead.h"

 //emulate a stream keyboard, this is not using interrupts as a good driver should do
 // AND is not using a buffer either!

// Pins of portd.
int8_t keysD[6] = {1,2,3,4,5,7};
int8_t keysC[4] = {4,5,6,7};
int8_t lastkey, lastkeyEC;
time_t pressMills;

uint8_t digitalReadD(uint8_t pin){
    return (PORTD&(1<<pin))!= 0;
}
uint8_t digitalReadC(uint8_t pin){
    return (PORTC&(1<<pin))!= 0;
}

void keyReadInit(void){
    pressMills = 0;
    lastkey = -1;
    lastkeyEC = -1;
    // Keys already initialized.
};

int8_t keyPeek(void) {
    for (int8_t n = 0; n < KEYDN; n++) {
        if (digitalReadD(keysD[n]) != 1) {
            return n;
        }
    }
    for (int8_t n = 0; n < KEYCN; n++) {
        if (digitalReadC(keysC[n]) != 1) {
            return n+KEYDN;
        }
    }
    return -1;
}

// Check if long press on a key. Should not be used together with keyPeek, not for normal key read.
int8_t keyReadEC() {
    int8_t ch = keyPeek();
    if (lastkeyEC == -1) {
        lastkeyEC = ch;
        pressMills = timeGet();
    } else if ((ch != -1) && (ch == lastkeyEC)) {
        bool longPress = ESCAPE_TIME && (timeDiff(pressMills,timeGet())>ESCAPE_TIME);        
        if (longPress) {
            return ESCAPE_CODE;
        }
    } else if (ch == -1) {
        lastkeyEC = -1;
    }
    return -1;
}

int8_t keyRead() {
    int8_t ch = keyPeek();
    if (ch == lastkey) return -1;
    int tmp = lastkey;
    lastkey = ch;
    return tmp;
}
