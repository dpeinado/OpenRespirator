/* 
 * File:   menu.h
 * Author: David Ortiz
 *
 */


#include "ORespGlobal.h"    
#include "menu.h"
#include "mcc_generated_files/mcc.h"

menuStatusT menuStatus;

void MenuInit(void) {
    menuStatus = CFG_IDLE;
}

void MenuMng(void) {
    // Manage menu.
    int8_t keyPress;
    keyPress = keyRead();
    if (keyPress >= 0) {
        DEBUG_PRINT(("KEY! %d\n", keyPress));
        lcdPrint = true;

        switch (keyPress) {
            case KEYIP:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_IP;
                    menuVal = IP;
                    menuTstamp = timeGet();
                } else if (menuStatus == CFG_IP) {
                    // Accept change and exit.
                    IP = menuVal;
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYBPM:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_BPM;
                    menuVal = BPM;
                    menuTstamp = timeGet();
                } else if (menuStatus == CFG_BPM) {
                    // Accept change and exit.
                    BPM = menuVal;
                    IDuration = ((uint16_t) 60*1000)/(3*BPM);
                    EDuration = ((uint16_t) 60*1000/BPM) - IDuration;
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYPEEP:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_PEEP;
                    menuVal = PEEP;
                    menuTstamp = timeGet();
                } else if (menuStatus == CFG_PEEP) {
                    // Accept change and exit.
                    PEEP = menuVal;
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYPLUS:
                if (menuStatus != CFG_IDLE) {
                    menuTstamp = timeGet();
                    menuVal = menuVal + 1;
                    // Should also check limits.
                    switch (menuStatus) {
                        case CFG_IP:
                            if (menuVal > IP_MAX) {
                                menuVal = IP_MAX;
                            }
                            break;
                        case CFG_PEEP:
                            if (menuVal > PEEP_MAX) {
                                menuVal = PEEP_MAX;
                            }
                            break;
                        case CFG_BPM:
                            if (menuVal > BPM_MAX) {
                                menuVal = BPM_MAX;
                            }
                            break;
                        default:
                            // No processing.
                            break;
                    }
                }
                break;
            case KEYMINUS:
                if (menuStatus != CFG_IDLE) {
                    menuTstamp = timeGet();
                    menuVal = menuVal - 1;
                    // Should also check limits.
                    switch (menuStatus) {
                        case CFG_IP:
                            if (menuVal < IP_MIN) {
                                menuVal = IP_MIN;
                            }
                            break;
                        case CFG_PEEP:
                            if (menuVal < PEEP_MIN) {
                                menuVal = PEEP_MIN;
                            }
                            break;
                        case CFG_BPM:
                            if (menuVal < BPM_MIN) {
                                menuVal = BPM_MIN;
                            }
                            break;
                        default:
                            // No processing.
                            break;
                    }
                }
                break;
        }
    } else {
        if ((menuStatus != CFG_IDLE) && (timeElapsed(menuTstamp, MENU_TOUT))) {
            // Silently exit menu.
            lcdPrint = true;
            menuStatus = CFG_IDLE;
        }
        return;
    }
}

void screenInit(void) {
    LcdI2CInit(0x27, 16, 2);
    setCursor(0, 0);
    printstrblock("EMERG.RESPIRATOR");
    setCursor(0, 1);
    printstrblock("  CONTROLLER   ");
    timeDelayMs(TIME_MS(2000));
    clear();
    lcdPrint = true;
    blink();
}

void screenMng(void) {
    // Check if something to print.
    if (lcdPrint && !PrintStrBusy()) {
        if (menuStatus == CFG_IDLE) {
            sprintf(lcdTopRow, "% 2d %2d % 2d IV: % 3d", BPM, PEEP, IP, lastCycleVol);
        } else if (menuStatus == CFG_BPM) {
            sprintf(lcdTopRow, "% 2d %2d % 2d IV: % 3d", menuVal, PEEP, IP, lastCycleVol);
        } else if (menuStatus == CFG_PEEP) {
            sprintf(lcdTopRow, "% 2d %2d % 2d IV: % 3d", BPM, menuVal, IP, lastCycleVol);
        } else if (menuStatus == CFG_IP) {
            sprintf(lcdTopRow, "% 2d %2d % 2d IV: % 3d", BPM, PEEP, menuVal, lastCycleVol);
        }
        DEBUG_PRINT((lcdTopRow));
        lcdPrint = false;
        setCursor(0, 0);
        printstr(lcdTopRow);
        if (menuStatus != CFG_IDLE) {
            lcdBlink = true;
        }
    } else if (lcdBlink && !PrintStrBusy()) {
        lcdBlink = false;
        switch (menuStatus) {
            case CFG_BPM:
                setCursor(1, 0);
                break;
            case CFG_PEEP:
                setCursor(4, 0);
                break;
            case CFG_IP:
                setCursor(7, 0);
                break;
            default:
                // No processing.
                break;
        }
    }
}

