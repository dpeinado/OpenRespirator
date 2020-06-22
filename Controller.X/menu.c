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

#define VOL_ACHK(volume) ((volume > (VOL_AMAX/10))? (VOL_AMAX/10) : ((volume < (VOL_AMIN/10))? (VOL_AMIN/10) : volume))

#define VOL_CHK(volume) ((volume > (VOL_MAX/10))? (VOL_MAX/10) : ((volume < (VOL_MIN/10))? (VOL_MIN/10) : volume))

bool lcdPrintTR, lcdPrintBR, lcdPrintBRR, lcdBlink;

char lcdTopRow[20];
char lcdBtnRow[20];
char lcdBtnRRow[10];
menuStatusT menuStatus;
uint16_t menuVal;
time_t menuTstamp;

void MenuInit(void) {
    menuStatus = CFG_IDLE;
    lcdPrintBR=true;
    lcdPrintBRR=false;
    lcdPrintTR=true;
    // All variables changed.
    chBdTrig=true;
    chBPM=true;
    chIP=true;
    chMaxP=true;
    chPEEP=true;
    chLowVAlarm=true;
    chHighVAlarm=true;
    chMaxV=true;
    chMaxP=true;
    chVentMode=true;
}

void MenuMng(void) {
    // Manage menu.
    int8_t keyPress;
    keyPress = keyRead();
    // Check if engineering mode entered (Simultaneous press of BPM and Volume). Engineering mode exit after 5 seconds inactive.
    // Within engineering mode, BPM selects  to navigate settings, press Volume to confirm setting.
    if ((menuStatus == CFG_IDLE) && (keyPress == KEYPLUS)){
        menuStatus = CFG_ENGMODE;
        menuTstamp = timeGet();
        lcdPrintBR = true;
    } else if ((keyPress >= 0) && (keyPress != KEYBREATH)) {
        lcdPrintTR = true;
        lcdPrintBR = true;
        DEBUG_PRINT(("KEY! %d\n", keyPress));

        if (menuStatus == CFG_POWEROFF) {
            menuStatus = CFG_IDLE;
            if (keyPress == KEYBPM) {
                // Confirm power-off.
                ctrlStatus = CTRL_SLEEP;
            } else {
                // Any other case, abort setting.
            }
        } else {
            switch (keyPress) {
                case KEYIP:
                    if (menuStatus == CFG_IDLE) {
                        menuStatus = CFG_IP;
                        if ((VentMode == VMODE_PRESSURE)||(intIP == 0)) {
                            menuVal = IP;
                        } else {
                            menuVal = intIP/MPRESSURE_MBAR(1);
                            if (menuVal > IP_MAX) {
                                menuVal = IP_MAX;
                            } else if (menuVal < IP_MIN) {
                                menuVal = IP_MIN;
                            }
                        }
                        menuTstamp = timeGet();
                    } else if (menuStatus == CFG_IP) {
                        // Accept change and exit.
                        if (VentMode == VMODE_VOLUME) {
                            VentMode = VMODE_PRESSURE;
                            chVentMode = true;                            
                            // By default MaxP 2mbar higher than IP in pressure mode.
                            MaxP = menuVal + 2;
                        } else {
                            // If already in pressure mode, keep delta between IP and MaxP.
                            MaxP = MaxP + menuVal - IP;
                        }
                        if (MaxP < menuVal + 2){
                            // By default keep 2 delta as a minimum.
                            MaxP = menuVal + 2;
                        }
                        if (MaxP > IP_MAX){
                            MaxP = IP_MAX;
                        } else if (MaxP < IP_MIN) {
                            MaxP = IP_MIN;
                        }
                        
                        chMaxP = true;                       
                        IP = menuVal;
                        chIP = true;
                        menuStatus = CFG_IDLE;
                    } else if (menuStatus == CFG_ENGMODE) {
                        menuStatus = CFG_ENGTRIG;
                        menuVal = BdTrig;
                        menuTstamp = timeGet();
                    } else if (menuStatus == CFG_ENGTRIG) {
                        BdTrig = menuVal;
                        chBdTrig = true;
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
                        IDuration = ((uint16_t) 60 * 1000) / (3 * BPM);
                        EDuration = ((uint16_t) 60 * 1000 / BPM) - IDuration;
                        menuStatus = CFG_IDLE;
                    } else if (menuStatus == CFG_ENGMODE) {
                        menuStatus = CFG_ENGVSTATS;                        
                        menuTstamp = timeGet();
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
                    } else if (menuStatus == CFG_ENGMODE) {
                        menuStatus = CFG_ENGLSTATS;
                        menuTstamp = timeGet();
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
                        menuStatus = CFG_IDLE;
                    } else {
                        // Any other case, abort setting.
                        menuStatus = CFG_IDLE;
                    }
                    break;
                case KEYMAXV:
                    if (menuStatus == CFG_IDLE) {
                        menuStatus = CFG_MAXV;
                        if ((VentMode == VMODE_VOLUME)||(intMaxV==0)) {
                            menuVal = MaxV;
                        } else {
                            menuVal = 2*((intMaxV+10)/20);   
                            menuVal = VOL_CHK(menuVal);
                        }
                        menuTstamp = timeGet();
                    } else if (menuStatus == CFG_MAXV) {
                        // Accept change and exit.
                        // Set max and min alarms default values.
                        // Track changes on high and low alarms.
                        HighVAlarm += (menuVal - MaxV);
                        LowVAlarm  += (menuVal - MaxV);
                        MaxV = menuVal;
                        chHighVAlarm = true;
                        chLowVAlarm = true;
                        chMaxV = true;
                        if (VentMode == VMODE_PRESSURE) {
                            VentMode = VMODE_VOLUME;
                            chVentMode = true;
                            // By default MaxP set to 35 in volume mode.
                            MaxP = 35;
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
                                menuVal = menuVal + 1;
                                if (menuVal > IP_MAX) {
                                    menuVal = IP_MAX;
                                }
                                break;
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
                                if ((VentMode == VMODE_PRESSURE) && (menuVal > IP)) {
                                    menuVal = IP;
                                } else if (menuVal > MaxP) {
                                    menuVal = MaxP;
                                }
                                break;
                            case CFG_BPM:
                                menuVal = menuVal + 1;
                                if (menuVal > BPM_MAX) {
                                    menuVal = BPM_MAX;
                                }
                                break;
                            case CFG_ENGTRIG:
                                menuVal = menuVal + 1;
                                if (menuVal > BDT_MAX) {
                                    menuVal = BDT_MAX;
                                }
                                break;
                            case CFG_MAXV:
                                menuVal = VOL_CHK(menuVal + 2);
                                break;
                            case CFG_LOWVA:
                            case CFG_HIGHVA:
                                menuVal = VOL_ACHK(menuVal + 2);
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
                                if (menuVal < PEEP) {
                                    menuVal = PEEP;
                                } else if (menuVal < IP_MIN) {
                                    menuVal = IP_MIN;
                                } else if ((menuStatus == CFG_MAXP) && (VentMode == VMODE_PRESSURE) && (menuVal < IP)) {
                                    menuVal = IP;
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
                            case CFG_ENGTRIG:
                                menuVal = menuVal - 1;
                                if (menuVal < BDT_MIN) {
                                    menuVal = BDT_MIN;
                                }
                                break;
                            case CFG_MAXV:
                                menuVal = VOL_CHK(menuVal - 2);
                                break;
                            case CFG_LOWVA:
                            case CFG_HIGHVA:
                                menuVal = VOL_ACHK(menuVal - 2);
                                break;
                            default:
                                // No processing.
                                break;
                        }
                    }
                    break;
                case KEYPOWER:
                    if (menuStatus == CFG_IDLE) {
                        // Power-off. Needs confirmation.
                        menuStatus = CFG_POWEROFF;
                        menuTstamp = timeGet();
                        BUZZER_ON;
                    }
            }
        }
        lcdPrintBR = true;
        lcdPrintTR = true;
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
    printstrblock("OxyVitaEmergency");
    setCursor(0, 1);
    printstrblock("Ventilator V1.0 ");
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
            if (ctrlStatus != CTRL_SLEEP) {
                if (VentMode == VMODE_PRESSURE) {
                    sprintf(lcdTopRow, "%2d %2d %2d %2d ----", BPM, PEEP, IP, MaxP);
                } else {
                    sprintf(lcdTopRow, "%2d %2d -- %2d %4d", BPM, PEEP, MaxP, 10 * ((uint16_t) MaxV));
                }
            } else {
                sprintf(lcdTopRow, "----OFF-STATE---");
            }
        } else if (menuStatus == CFG_BPM) {
            if (VentMode == VMODE_PRESSURE) {
                sprintf(lcdTopRow, "%2d %2d %2d %2d ----", menuVal, PEEP, IP, MaxP);
            } else {
                sprintf(lcdTopRow, "%2d %2d -- %2d %4d", menuVal, PEEP, MaxP, 10 * ((uint16_t) MaxV));
            }
        } else if (menuStatus == CFG_PEEP) {
            if (VentMode == VMODE_PRESSURE) {
                sprintf(lcdTopRow, "%2d %2d %2d %2d ----", BPM, menuVal, IP, MaxP);
            } else {
                sprintf(lcdTopRow, "%2d %2d -- %2d %4d", BPM, menuVal, MaxP, 10 * ((uint16_t) MaxV));
            }
        } else if (menuStatus == CFG_IP) {
            sprintf(lcdTopRow, "%2d %2d %2d %2d ----", BPM, PEEP, menuVal, MaxP);
        } else if (menuStatus == CFG_MAXP) {
            if (VentMode == VMODE_PRESSURE) {
                sprintf(lcdTopRow, "%2d %2d %2d %2d ----", BPM, PEEP, IP, menuVal);
            } else {
                sprintf(lcdTopRow, "%2d %2d -- %2d %4d", BPM, PEEP, menuVal, 10 * ((uint16_t) MaxV));            
            }
        } else if (menuStatus == CFG_MAXV) {
            sprintf(lcdTopRow, "%2d %2d -- %2d %4d", BPM, PEEP, MaxP, 10 * ((uint16_t) menuVal));
        } else if (menuStatus == CFG_POWEROFF) {
            sprintf(lcdTopRow, "PRESS BPM TO    ");
        } else if (menuStatus == CFG_ENGVSTATS) {
            sprintf(lcdTopRow, "%3d %3d %3d     ", rSV2ValveORT, rSV2ValveCRT, rSV3ValveORT);
        } else if (menuStatus == CFG_ENGLSTATS) {
            sprintf(lcdTopRow, "%3d   %3d  ", (uint16_t) ((((uint24_t) MPRESSURE_MBAR(1))*((uint24_t) lungC))>>7), lungR/MPRESSURE_MBAR(1));
        } else if (menuStatus == CFG_ENGTRIG) {
            sprintf(lcdTopRow, "%1d               ", menuVal);
        }

//        DEBUG_PRINT((lcdTopRow));
        setCursor(0, 0);
        printstr(lcdTopRow);
        if (menuStatus != CFG_IDLE) {
            lcdBlink = true;
        }
    } else if (lcdPrintBRR && !PrintStrBusy()) {
        lcdPrintBRR = false;
//       DEBUG_PRINT((lcdBtnRRow));
        setCursor(11, 1);
        printstr(lcdBtnRRow);
        if (menuStatus != CFG_IDLE) {
            lcdBlink = true;
        }
    } else if (lcdPrintBR && !PrintStrBusy()) {
        lcdPrintBR = false;
        if (menuStatus == CFG_LOWVA) {
            sprintf(lcdBtnRow, "%4d %4d  ", 10 * ((uint16_t) menuVal), 10 * ((uint16_t) HighVAlarm));
        } else if (menuStatus == CFG_HIGHVA) {
            sprintf(lcdBtnRow, "%4d %4d  ", 10 * ((uint16_t) LowVAlarm), 10 * ((uint16_t) menuVal));
        } else if (menuStatus == CFG_POWEROFF) {
                sprintf(lcdBtnRow, "POWER OFF ");
        } else if (menuStatus == CFG_ENGMODE) {
                sprintf(lcdBtnRow, "VS LS BDT");
        } else if (menuStatus == CFG_ENGTRIG) {
                sprintf(lcdBtnRow, "      BDT");
        } else if (menuStatus == CFG_ENGVSTATS) {
                sprintf(lcdBtnRow, "S2O S2C S3C");
        } else if (menuStatus == CFG_ENGLSTATS) {
                sprintf(lcdBtnRow, "LC    LR   ");
        } else {
            if (ctrlStatus != CTRL_SLEEP) {
                sprintf(lcdBtnRow, "%4d %4d  ", 10 * ((uint16_t) LowVAlarm), 10 * ((uint16_t) HighVAlarm));
            } else {
                sprintf(lcdBtnRow, "PWR TO ON ");
            }
        }

        // Detect reason for latest change.
//        DEBUG_PRINT((lcdBtnRow));
        setCursor(0, 1);
        printstr(lcdBtnRow);
        lcdBlink = true;
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
            case CFG_MAXP:
                setCursor(10, 0);
                break;
            case CFG_MAXV:
                setCursor(15, 0);
                break;
            case CFG_LOWVA:
                setCursor(3, 1);
                break;
            case CFG_HIGHVA:
                setCursor(8, 1);
                break;
            default:
                setCursor(21, 0);
                break;
        }
    }
}
