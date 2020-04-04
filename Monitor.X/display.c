#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "lcd.h"

void InitDisplay(void) {
    LCDInit();
}

void ValueDisplay(int TR, float tdi, float tde) {
    char msg[16];
    
    if (TR>100) TR=100;
    if (TR<0)   TR=0;
    
    sprintf(msg, "%2d%% %1.2f %1.2f", TR, tdi, tde);
    LCDMessage(msg);
}
void AlarmDisplay(int idx, unsigned char alarm) {
    char msg[16];
    
    
    sprintf(msg, "%2d %13s", idx, alarm);
    LCDMessage(msg);
}