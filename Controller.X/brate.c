/* 
 * File:   brate.c
 * Author: David Ortiz
 *
 */

#include "brate.h"

uint8_t  eBRate;
uint24_t bRateHist;
uint24_t bRatePtr;
uint8_t  bRateCnt;

// Keep Rate in last 20 inspirations.
void bRateInit(void) {
    bRateHist = 0;
    bRatePtr = 1;
    eBRate = 0;
    bRateCnt = 0;
}

void bRateUpdt(bool triggered) {
    bRatePtr = bRatePtr << 1;
    bRateCnt++;
    if (bRateCnt > 20){
        bRateCnt = 20;
    }
    if (bRatePtr == (0x1L << 20)) {
        bRatePtr = 1;
    }

    if (bRateHist & bRatePtr) {
        eBRate -= 5;
    }
    if (triggered) {
        eBRate += 5;
        bRateHist |= bRatePtr;
    } else {
        bRateHist &= ~bRatePtr;
    }
}

uint8_t bRateGet(void) {
    return eBRate*20/bRateCnt;
}
