//if you hold/repeat a key for this amount of time we will consider it an escape
#ifndef ESCAPE_TIME
#define ESCAPE_TIME 2000
#endif

#define ESCAPE_CODE -100
#define KEYN 6

#include "time.h"
#include "keyRead.h"

 //emulate a stream keyboard, this is not using interrupts as a good driver should do
 // AND is not using a buffer either!
uint8_t* keys;
uint8_t lastkey;
time_t pressMills;

void keyReadInit(void){
    pressMills = 0;
    lastkey = -1;
};

int8_t keyAvailable(void) {
    int8_t ch = keyPeek();
    if (lastkey == -1) {
        lastkey = ch;
        pressMills = millis();
    } else if (ESCAPE_TIME && millis() - pressMills > ESCAPE_TIME) return 1;
    if (ch == lastkey) return 0;
    return 1;
}

int8_t keyPeek(void) {
    for (int8_t n = 0; n < KEYN; n++) {
        int8_t pin = keys[n];
        if (digitalRead(pin) != 1) return n;
    }
    return -1;
}
// Read with escape code.

int8_t keyReadEC() {
    int8_t ch = keyPeek();
    if (ch == lastkey) return -1;
    int8_t tmp = lastkey;
    bool longPress = ESCAPE_TIME && millis() - pressMills>ESCAPE_TIME;
    pressMills = millis();
    lastkey = ch;
    return longPress ? ESCAPE_CODE : tmp; //long press will result in escape
}

int8_t keyRead() {
    int8_t ch = keyPeek();
    if (ch == lastkey) return -1;
    int tmp = lastkey;
    lastkey = ch;
    return tmp;
}
