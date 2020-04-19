/* 
 * File:   menu.h
 * Author: David Ortiz
 *
 */


#include "ORespGlobal.h"    
#include "menu.h"
#include "mcc_generated_files/mcc.h"
#include "keyRead.h"
#include "time.h"
#include "LiquidCrystal_I2C.h"

bool lcdPrintTR, lcdPrintBR, lcdBlink;
bool lcdMenuPrint;

char lcdTopRow[20];
char lcdBtnRow[20];
menuStatusT menuStatus;
uint16_t menuVal;
time_t menuTstamp;

void MenuInit(void) {
    menuStatus = CFG_IDLE;
    lcdMenuPrint = true;
    lcdPrintBR=true;
    lcdPrintTR=true;
}

void MenuMng(void) {
    // Manage menu.
    int8_t keyPress;
    keyPress = keyRead();
    if ((keyPress >= 0) && (keyPress != KEYBREATH)) {
        lcdMenuPrint = true;
        DEBUG_PRINT(("KEY! %d\n", keyPress));

        switch (keyPress) {
            case KEYIP:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_IP;
                    menuVal = IP;
                    menuTstamp = timeGet();
                } else if (menuStatus == CFG_IP) {
                    // Accept change and exit.
                    IP = menuVal;
                    chIP = true;
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
                    chBPM = true;
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
                    chPEEP = true;
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYMAXP:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_MAXP;
                    menuVal = MaxP;
                    menuTstamp = timeGet();
                } else if (menuStatus == CFG_MAXP) {
                    // Accept change and exit.
                    MaxP = menuVal;
                    chMaxP = true;
                    if (VentMode == 0) {
                        VentMode = 1;
                        chVentMode = true;
                    }
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYMAXV:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_MAXV;
                    menuVal = MaxP;
                    menuTstamp = timeGet();
                    if ((MaxP != IP) && (VentMode == 0)) {
                        // If vent mode was previously pressure, set maxp to previous ip
                        // This is to avoid sudden changes.
                        MaxP = IP;
                        chMaxP = true;
                    }
                } else if (menuStatus == CFG_MAXV) {
                    // Accept change and exit.
                    MaxV = menuVal;
                    chMaxV = true;
                    if (VentMode == 0) {
                        VentMode = 1;
                        chVentMode = true;
                        // If vent mode was previously pressure, set maxp to previous ip
                        // This is to avoid sudden changes.
                        MaxP = IP;
                        chMaxP = true;                
                    }
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYHIGHVA:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_HIGHVA;
                    menuVal = HighVAlarm;
                    menuTstamp = timeGet();
                } else if (menuStatus == CFG_HIGHVA) {
                    // Accept change and exit.
                    HighVAlarm = menuVal;
                    chHighVAlarm = true;
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYLOWVA:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_LOWVA;
                    menuVal = LowVAlarm;
                    menuTstamp = timeGet();
                } else if (menuStatus == CFG_LOWVA) {
                    // Accept change and exit.
                    LowVAlarm = menuVal;
                    chLowVAlarm = true;
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYPLUS:
                if (menuStatus != CFG_IDLE) {
                    menuTstamp = timeGet();
                    // Should also check limits.
                    switch (menuStatus) {
                        case CFG_IP:
                        case CFG_MAXP:
                            menuVal = menuVal + 1;
                            if (menuVal > IP_MAX) {
                                menuVal = IP_MAX;
                            }
                            break;
                        case CFG_PEEP:
                            menuVal = menuVal + 1;
                            if (menuVal > PEEP_MAX) {
                                menuVal = PEEP_MAX;
                            }
                            break;
                        case CFG_BPM:
                            menuVal = menuVal + 1;
                            if (menuVal > BPM_MAX) {
                                menuVal = BPM_MAX;
                            }
                            break;
                        case CFG_MAXV:
                        case CFG_LOWVA:
                        case CFG_HIGHVA:
                            menuVal = menuVal + 20;
                            if (menuVal > VOL_MAX) {
                                menuVal = VOL_MAX;
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
                    // Should also check limits.
                    switch (menuStatus) {
                        case CFG_IP:
                        case CFG_MAXP:
                            menuVal = menuVal - 1;
                            if (menuVal < IP_MIN) {
                                menuVal = IP_MIN;
                            }
                            break;
                        case CFG_PEEP:
                            menuVal = menuVal - 1;
                            if (menuVal < PEEP_MIN) {
                                menuVal = PEEP_MIN;
                            }
                            break;
                        case CFG_BPM:
                            menuVal = menuVal - 1;
                            if (menuVal < BPM_MIN) {
                                menuVal = BPM_MIN;
                            }
                            break;
                        case CFG_MAXV:
                        case CFG_LOWVA:
                        case CFG_HIGHVA:
                            menuVal = menuVal - 20;
                            if (menuVal < VOL_MIN) {
                                menuVal = VOL_MIN;
                            }
                            break;
                        default:
                            // No processing.
                            break;
                    }
                }
                break;
        }
        
        lcdPrintBR = true;
        lcdPrintTR = true;            
        lcdMenuPrint = true;
    } else {
        if ((menuStatus != CFG_IDLE) && (timeElapsed(menuTstamp, MENU_TOUT))) {
            // Silently exit menu.
            lcdPrintTR = true;
            lcdPrintBR = true;
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
    lcdPrintTR = true;
    blink();
}

void screenMng(void) {
    // Check if something to print.
    if (lcdPrintTR && !PrintStrBusy()) {
        lcdPrintTR = false;
        if ((menuStatus == CFG_IDLE) || (menuStatus == CFG_LOWVA) || (menuStatus == CFG_HIGHVA)) {
            if (VentMode == 0) {
                sprintf(lcdTopRow, "%2d %2d  %2d -- ---", BPM, PEEP, IP);
            } else {
                sprintf(lcdTopRow, "%2d %2d  -- 2d % 3d", BPM, PEEP, MaxP, MaxV);
            }
        } else if (menuStatus == CFG_BPM) {
            if (VentMode == 0) {
                sprintf(lcdTopRow, "%2d %2d  %2d -- ---", menuVal, PEEP, IP);
            } else {
                sprintf(lcdTopRow, "%2d %2d  -- 2d % 3d", menuVal, PEEP, MaxP, MaxV);
            }
        } else if (menuStatus == CFG_PEEP) {
            if (VentMode == 0) {
                sprintf(lcdTopRow, "%2d %2d  %2d -- ---", BPM, menuVal, IP);
            } else {
                sprintf(lcdTopRow, "%2d %2d  -- 2d % 3d", BPM, menuVal, MaxP, MaxV);
            }
        } else if (menuStatus == CFG_IP) {
            sprintf(lcdTopRow, "%2d %2d  %2d -- ---", BPM, PEEP, menuVal);
        } else if (menuStatus == CFG_MAXP) {
            sprintf(lcdTopRow, "%2d %2d  -- %2d % 3d", BPM, PEEP, menuVal, MaxV);
        } else if (menuStatus == CFG_MAXV) {
            sprintf(lcdTopRow, "%2d %2d  -- %2d % 3d", BPM, PEEP, MaxP, menuVal);
        }

        DEBUG_PRINT((lcdTopRow));
        setCursor(0, 0);
        printstr(lcdTopRow);
        if (menuStatus != CFG_IDLE) {
            lcdBlink = true;
        }
    } else if (lcdPrintBR && !PrintStrBusy()) {
        lcdPrintBR = false;

        if (lcdMenuPrint) {
            lcdMenuPrint = false;
            if (menuStatus == CFG_LOWVA) {
                sprintf(lcdBtnRow, "LOW V. ALARM % 3d", menuVal);
            } else if (menuStatus == CFG_HIGHVA) {
                sprintf(lcdBtnRow, "HIGHV. ALARM % 3d", menuVal);
            } else {
                sprintf(lcdBtnRow, "                ");                
            }            
        }
        
        // Detect reason for latest change.
        DEBUG_PRINT((lcdBtnRow));
        setCursor(0, 1);
        printstr(lcdBtnRow);
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
                setCursor(8, 0);
                break;
            default:
                // No processing.
                break;
        }
    }
}

