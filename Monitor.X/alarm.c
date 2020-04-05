#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "alarm.h"
#include "buzzer.h"

void MuteAlarm(void) {}
void HistAlarm(void) {}

void AlarmSet(uint8_t state) {
    BuzzerSet(state);
}

void AlarmClear(){
    BuzzerClear();
}

void AlarmInit() {
    BuzzerInit();
}
