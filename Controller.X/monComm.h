/* 
 * File:   monComm.h
 * Author: David Ortiz
 *
 */

#ifndef MONCOMM_H
#define	MONCOMM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"
#include "time.h"

  // Enable only in debug!!!
#define FORCEMONOK 0
  
// FUNCTIONS TO COMMUNICATE WITH MONITOR.
// Message to sent to monitor.
#define MONIDX_MODE   0
#define MONIDX_IPV    1
#define MONIDX_EPV    2
#define MONIDX_BPMV   3
#define MONIDX_PMAXV  4
#define MONIDX_VMAXV  5
#define MONIDX_LVAV   6
#define MONIDX_HVAV   7
#define MONIDX_BRRTV  8
#define MONIDX_ALARMV 9

  char monitorMsg[10];
  char ctrlErrorStatus;
  
  typedef enum
    {
     MON_VDDE= 0x80,
     MON_SV3E= 0x40,
     MON_IPE = 0x20,
     MON_EPE = 0x10,
     MON_SV2E= 0x08
    } monErrorT;
  
  typedef enum
    {
     MONSTATE_INIT   = 0x20,
     MONSTATE_CALP   = 0x40,
     MONSTATE_SV2CHK = 0x60,
     MONSTATE_SV1CHK = 0x80,
     MONSTATE_CALF   = 0xA0,
     MONSTATE_STOP = 0x00,
     MONSTATE_RUNP = 0x10,
     MONSTATE_RUNV = 0x14,
     MONSTATE_SLEEP = 0x8
    } monStateT;

  extern time_t    monTstamp;
  extern i2c2_error_t lastI2CMonTrfResponse;
  extern bool chBPM, chIP, chMaxP, chPEEP, chLowVAlarm, chHighVAlarm, chMaxV, chMaxP, chVentMode;
 
  void MonitorInit(void);
  void MonitorErrorSet(monErrorT flag);
  void MonitorErrorClr(monErrorT flag);
  bool MonitorMsgBusy(void);
  void MonitorMsgForcedSend (monStateT state);
  void MonitorMsgSend(monStateT state);
  bool MonitorMsgSendBlock(monStateT state);
  
#ifdef	__cplusplus
}
#endif

#endif


