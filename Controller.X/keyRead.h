#ifndef KEY_READ_H_
#define KEY_READ_H_

#include "mcc_generated_files/mcc.h"

void keyReadInit(void);

int8_t keyAvailable(void);

int8_t keyPeek(void);
// Read with escape code.

int8_t keyReadEC();

int8_t keyRead();

#endif /* KEYREAD_H_ */
