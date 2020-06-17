/* 
 * File:   selftest.c
 * Author: David Ortiz
 *
 */

#include "selftest.h"
#include "ORespGlobal.h"    
#include "time.h"
#include "menu.h"
#include "monComm.h"
#include "keyRead.h"
#include "aCapture.h"
#include "vMeasure.h"
#include "LiquidCrystal_I2C.h"

uint16_t freeFlowRateF, freeFlowRateM, freeFlowRateL;
int16_t  vddValMean;

typedef enum
  {
   FCHK_LOW,
   FCHK_MED,
   FCHK_HIGH
  } fchk_val;

bool pressureSensorChk(bool offsetCal, uint16_t time) {
    int16_t mPVal, mPValMax, mPValMin;
    int32_t mPValMean, aPValMean;
    int16_t aPVal, aPValMax, aPValMin;
    time_t tstamp;
    bool chkResult;

    chkResult = true;

    // Now capture data.
    mPValMin = 4096;
    aPValMin = 4096;
    mPValMax = 0;
    aPValMax = 0;
    tstamp = timeGet();
    
    while (!timeElapsedR(&tstamp, TIME_MS(time))) {
        if (aCaptGetResult(MainPSensor, &mPVal)) {
            if (mPVal > mPValMax) {
                mPValMax = mPVal;
            }
            if (mPVal < mPValMin) {
                mPValMin = mPVal;
            }
            mPValMean = (31*mPValMean + 32*mPVal)>>5;
        } else {
            ERROR_CONDITION(20);
            chkResult = false;
        }

        if (aCaptGetResult(VolPSensor, &aPVal)) {
            if (aPVal > aPValMax) {
                aPValMax = aPVal;
            }
            if (aPVal < aPValMin) {
                aPValMin = aPVal;
            }
            aPValMean = (31*aPValMean + 32*aPVal)>>5;
        } else {
            chkResult = false;
            ERROR_CONDITION(21);
        }
    }

    // Cal mean values.
    mPValMean = mPValMean>>5;
    aPValMean = aPValMean>>5;
    sprintf(lcdTopRow, "MS: % 3d AS: % 3d ", mPValMean, aPValMean);
    DEBUG_PRINT(("MAIN PRESSURE. Min %d Max %d MEAN %d\n", mPValMin, mPValMax, mPValMean));
    DEBUG_PRINT(("AUX PRESSURE. Min %d Max %d MEAN %d\n", aPValMin, aPValMax, aPValMean));
    setCursor(0, 0);
    printstrblock(lcdTopRow);
    timeDelayMs(1000);

    // Now check pressure sensors working correctly.
    if (offsetCal) {
        if (((aPValMax - aPValMin) > 16) || ((mPValMax - mPValMin) > 16) || (mPValMean > 350) || (mPValMean < 40) || (aPValMean < 600) || (aPValMean > 1030)) {
            // Too noisy, or values outside expected range.
            setCursor(0, 0);
            chkResult = false;
            DEBUG_PRINT(("PRESSURE CALIBRATION ERROR\n"));
        } else {
            // Set cal values.
            aCaptureOffSet(MainPSensor, mPValMean);
            aCaptureOffSet(VolPSensor, aPValMean);
            // Reset filters.
            aCaptRstFlt(Flt0PSensor);
            aCaptRstFlt(Flt1PSensor);
            aCaptRstFlt(Flt2PSensor);
            aCaptRstFlt(Flt3PSensor);
            timeDelayMs(50); // To ensure prev data flushed.
        }
    } else {
        if (((aPValMax - aPValMin) > 16) || ((mPValMax - mPValMin) > 16) || (mPValMean > 10) || (mPValMean < -10) || (aPValMean < -10) || (aPValMean > 10)) {
            DEBUG_PRINT(("VALVE CHECK ERROR\n"));
            chkResult = false;
        }
    }
    return chkResult;
}

// Measure and report flow.
bool flowChk(fchk_val flowVal){
  int16_t aPVal, aPValMax, aPValMin, aPValMean;
  int16_t minFlowL, maxFlowL;
  bool initOk;
  time_t tstamp;

  initOk = true;
  
  // Open valve, wait for 250ms, measure flow during 250ms, display result.
  setCursor(0, 0);
  printstrblock("FLOW RATE        ");
  aPValMin = 4096;
  aPValMax = 0;

  if (!MonitorMsgSendBlock(MONSTATE_CALF)){
    initOk=false;
    DEBUG_PRINT(("Mon error"));
    setCursor(0, 0);
    printstrblock("MONITOR ERROR");
    timeDelayMs(500);
  }

  // Set valves, set flow limits (in mililiters/second).
  switch (flowVal) {
  case FCHK_LOW:
    OPEN_SV2LOW;
    minFlowL = 250;
    break;
  case FCHK_MED:
    OPEN_SV2MED;
    minFlowL = 400;
    break;
  case FCHK_HIGH:
    OPEN_SV2;
    minFlowL = 600;
    break;
  }
  maxFlowL = 2*minFlowL;
  OPEN_SV3;

  timeDelayMs(250);
  vMeasureRst();
  tstamp = timeGet();
  while (!timeElapsedR(&tstamp, TIME_MS(250))) {
    aCaptGetResult(VolPSensor, &aPVal);
    if (aPVal > aPValMax) {
      aPValMax = aPVal;
    }
    if (aPVal < aPValMin) {
      aPValMin = aPVal;
    }
  }
  // Open Flow Rate in ml/sec. Do it in 250ms to avoid patient damage even if patient is connected.
  freeFlowRateL = vMeasureGet()<<2;
  if (!MonitorMsgSendBlock(MONSTATE_STOP)){
    initOk=false;
    DEBUG_PRINT(("Mon error"));
    setCursor(0, 0);
    printstrblock("MONITOR ERROR");
    timeDelayMs(500);
  }
  CLOSE_SV2;
  CLOSE_SV3;
  
  aPValMean=aCaptureOffGet(VolPSensor);
  
  if ((aPValMean + aPValMax)>= (0.95*8192)) {
    // Flow sensor almost saturated. Flow too high.
    setCursor(0, 0);
    printstrblock("FLOW TOO HIGH   ");
    timeDelayMs(1000);
    initOk = false;
  } else {
    sprintf(lcdTopRow, "FLOW: % 3d L/min ", (int16_t) ((uint32_t) ((uint32_t) 60 * freeFlowRateL) / 1000));
    setCursor(0, 0);
    printstrblock(lcdTopRow);
    timeDelayMs(4000);
    
    if (freeFlowRateL < minFlowL) {
      // Outside limits.
      setCursor(0, 0);
      printstrblock("FLOW TOO LOW    ");
      timeDelayMs(1000);
      initOk = false;
    } else if (freeFlowRateL > maxFlowL) {
      // Outside limits.
      setCursor(0, 0);
      printstrblock("FLOW TOO HIGH   ");
      timeDelayMs(1000);
      initOk = false;
    }
  }
  return initOk;
}


// Initialization procedure.
// Self-test.
bool SelfTest(bool tstScreen){
  int idx, cIdx;
  int16_t vddVal, vddValMax, vddValMin;
  int16_t aPVal, aPValMax, aPValMin, aPValMean;
  bool lcdBLight;
  time_t tstamp;
  bool initOk;

  ctrlStatus = CTRL_UNCAL;
  CLOSE_SV2;
  CLOSE_SV3;

    if (tstScreen) {
        setCursor(0, 1);
        printstrblock("SELF-TEST       ");
        setCursor(0, 0);
        printstrblock("DISPLAY TEST    ");
        for (idx = 0; idx < 10; idx++) {
            setCursor(0, 0);
            lcdTopRow[0] = 0x30 + idx;
            for (cIdx = 1; cIdx < 16; cIdx++) {
                lcdTopRow[cIdx] = lcdTopRow[0];
            }
            printstrblock(lcdTopRow);
            setCursor(0, 1);
            printstrblock(lcdTopRow);
            timeDelayMs(350);
        }
    }
    
  setCursor(0, 0);
  printstrblock("SELF-TEST. LEAVE");
  setCursor(0, 1);
  printstrblock("AIRWAY OPEN     ");
  timeDelayMs(1000);
  
  vddValMin = 8000;
  vddValMax = 0;
  initOk = true;
  
  if (!MonitorMsgSendBlock(MONSTATE_INIT)) {
    initOk = false;
    DEBUG_PRINT(("Mon error"));
    setCursor(0, 0);
    printstrblock("MONITOR ERROR");
    timeDelayMs(500);
  }
  
  aCaptureOffSet(MainPSensor, 0);
  aCaptureOffSet(VolPSensor, 0);
  // First check VDD stability.
  tstamp = timeGet();
  
  while (!timeElapsedR(&tstamp, TIME_MS(500))) {
    if (aCaptGetResult(VddSensor, &vddVal)) {
      if (vddVal > vddValMax) {
	vddValMax = vddVal;
      }
      if (vddVal < vddValMin) {
	vddValMin = vddVal;
      }
    } else {
      ERROR_CONDITION(22);
            initOk = false;
    }
  }

  if (!initOk) {
    return initOk;
  }

  // Now check within limits.
  vddValMean = (vddValMax + vddValMin) >> 1;
  DEBUG_PRINT(("VDD Min %d Max %d MEAN %d\n", vddValMin, vddValMax, vddValMean));

  if ((vddValMean > 5250) || (vddValMean < 4750)) {
    // Print error message and exit.
    setCursor(0, 0);
    printstrblock("INTERNAL ERROR  ");
    setCursor(0, 1);
    sprintf(lcdTopRow, "VDD OOL: % 3d mV", vddValMean);
    printstrblock(lcdTopRow);
    initOk = false;
  } else if ((vddValMax - vddValMin) > 100) {
    // Print error message and exit.
    setCursor(0, 0);
    printstrblock("INTERNAL ERROR  ");
    setCursor(0, 1);
    printstrblock("VDD NOISY       ");
    initOk = false;
  }
  
  if (!initOk) {
    return initOk;
  }

  while (ctrlStatus == CTRL_UNCAL) {
    BLED_OFF;
    // First, ensure no key pressed.
    while (keyPeek() != -1)
      DEBUG_PRINT(("KP %d\n", keyPeek()));
    ;
    keyReadInit();
    
    // Repeat until correct calibration.
    // Now calibrate.
    BLED_ON;
    setCursor(0, 0);
    printstrblock("CAL PRESS BREATH");
    tstamp = timeGet();
    lcdBLight = true;
    
    tstamp = timeGet();
    while (1) {
      if (timeElapsedR(&tstamp, TIME_MS(500))) {
	setBacklight(lcdBLight);
	lcdBLight = !lcdBLight;
      }
      // WAIT UNTIL BREATH BUTTON IS PRESSED DURING 2 SECONDS.
      // MEANWHILE BLINK LCD and KEEP BREATH PRESSED.
      if (keyReadEC() == ESCAPE_CODE) {
	if (keyPeek() == KEYBREATH) {
	  // Key Breath pressed.
	  setBacklight(true);
	  break;
	}
      }
    }
    
    BLED_OFF;
    DEBUG_PRINT(("CALP\n"));
    if (!MonitorMsgSendBlock(MONSTATE_CALP)){
      initOk=false;
      DEBUG_PRINT(("Mon error"));
      setCursor(0, 0);
      printstrblock("MONITOR ERROR   ");
      timeDelayMs(500);
    }
    
    if (pressureSensorChk(true, 500)) {
      // Success.
      ctrlStatus = CTRL_STOP;
    } else {
      setCursor(0, 0);
      printstrblock("CAL ERROR. RETRY ");
      timeDelayMs(500);
    }
  }
  
  if (!initOk) {
    return initOk;
  }
  
  // Two extra valve check steps. 
  DEBUG_PRINT(("CHKSV2\n"));
  // First CLOSE SV2, OPEN SV1, check no pressure, no volume detected.
  if (!MonitorMsgSendBlock(MONSTATE_SV2CHK)) {
        initOk = false;
        DEBUG_PRINT(("Mon error"));
        setCursor(0, 0);
        printstrblock("MONITOR ERROR   ");
        timeDelayMs(500);
    }
  // Also check buzzer during this stage.
  BUZZER_ON;
  
  CLOSE_SV2;
  timeDelayMs(200);
  if (!pressureSensorChk(false, 100)) {
    initOk=false;
	setCursor(0, 0);
    printstrblock("SV2 Valve error ");
    timeDelayMs(500);
  }
  
  // Then CLOSE SV1, OPEN SV2, check no pressure, no volume detected.
  BUZZER_OFF;
  DEBUG_PRINT(("CHKSV1\n"));
    if (!MonitorMsgSendBlock(MONSTATE_SV1CHK)) {
        initOk = false;
        DEBUG_PRINT(("Mon error"));
        setCursor(0, 0);
        printstrblock("MONITOR ERROR   ");
        timeDelayMs(500);
    }
  timeDelayMs(100);
  OPEN_SV2;
  timeDelayMs(300);
  if (!pressureSensorChk(false, 100)) {
    initOk=false;
	setCursor(0, 0);
    printstrblock("SV1 Valve error ");
    timeDelayMs(500);
  }
  
  // Last step, check flow  . 
  DEBUG_PRINT(("CHK FLOW\n"));
  tstamp = timeGet();
  
  // Measure and report flow.
  initOk &= flowChk(FCHK_LOW);
  initOk &= flowChk(FCHK_MED);
  initOk &= flowChk(FCHK_HIGH);

  // Wait until key depressed.
  while (keyPeek() != -1)
    ;
  keyReadInit();
  
  return initOk;
}

