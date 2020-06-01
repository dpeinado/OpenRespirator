#ifndef KEY_READ_H_
#define KEY_READ_H_

#include "mcc_generated_files/mcc.h"

#define KEYIP        0
#define KEYPEEP      1
#define KEYBPM       2
#define KEYMINUS     5
#define KEYPLUS      3
#define KEYBREATH    4
#define KEYMAXV      6
#define KEYMAXP      7
#define KEYHIGHVA    8
#define KEYLOWVA     9
#define KEYPOWER  10

#define ESCAPE_CODE -100

void keyReadInit(void);

int8_t keyPeek(void);
// Read with escape code.

int8_t keyReadEC();

int8_t keyRead();

void keyFlush(uint8_t keyIdx);

// Poll for specific key pressed. To check simultanous key presses.
bool isKeyPressed(uint8_t keyIdx);
#endif /* KEYREAD_H_ */
