//if you hold/repeat a key for this amount of time we will consider it an escape
#ifndef ESCAPE_TIME
#define ESCAPE_TIME 2000
#endif

#define KEYN 6

#include "time.h"
#include "keyRead.h"

 //emulate a stream keyboard, this is not using interrupts as a good driver should do
 // AND is not using a buffer either!

// Pins of portd.
int8_t keys[KEYN] = {1,2,3,4,5,7};
int8_t lastkey, lastkeyEC;
time_t pressMills;

uint8_t digitalRead(uint8_t pin){
    return (PORTD&(1<<pin))!= 0;
}

void keyReadInit(void){
    pressMills = 0;
    lastkey = -1;
    lastkeyEC = -1;
    // Keys already initialized.
};

int8_t keyAvailable(void) {
    int8_t ch = keyPeek();
    if (lastkey == -1) {
        lastkey = ch;
        pressMills = timeGet();
    } else if (ESCAPE_TIME && (timeDiff(pressMills,timeGet()) > ESCAPE_TIME)) return 1;
    if (ch == lastkey) return 0;
    return 1;
}

int8_t keyPeek(void) {
    for (int8_t n = 0; n < KEYN; n++) {
        if (digitalRead(keys[n]) != 1) {
            return n;
        }
    }
    return -1;
}

// Read with escape code.
int8_t keyReadEC() {
    int8_t ch = keyPeek();
    if (ch == lastkeyEC) return -1;
    int8_t tmp = lastkeyEC;
    bool longPress = ESCAPE_TIME && (timeDiff(pressMills,timeGet())>ESCAPE_TIME);
    pressMills = timeGet();
    lastkeyEC = ch;
    return longPress ? ESCAPE_CODE : tmp; //long press will result in escape
}

int8_t keyRead() {
    int8_t ch = keyPeek();
    if (ch == lastkey) return -1;
    int tmp = lastkey;
    lastkey = ch;
    return tmp;
}
