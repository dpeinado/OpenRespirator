#ifndef KEY_READ_H_
#define KEY_READ_H_

#include "mcc_generated_files/mcc.h"

#define KEYIP     0
#define KEYPEEP   1
#define KEYBPM    2
#define KEYMINUS  3
#define KEYPLUS   5
#define KEYBREATH 4

#define ESCAPE_CODE -100


void keyReadInit(void);

int8_t keyAvailable(void);

int8_t keyPeek(void);
// Read with escape code.

int8_t keyReadEC();

int8_t keyRead();

#endif /* KEYREAD_H_ */
