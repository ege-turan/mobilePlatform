/****************************************************************************
 Module
   DriveCorrectionService.c

 Revision
   1.0.1

 Description
   This file implements DriveCorrectionService as a simple service under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 26             ege      for mobile platform line following and encoder-based couse-correction
 25             ege      updated and cleaned
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "DriveCorrectionService.h"

#include "DCMotorService.h"

#include <sys/attribs.h> // for interrupts

/*----------------------------- Module Defines ----------------------------*/
#define MIDPOINT_COUNT 300

#define KP 3
#define KI 1
#define I_MAX 500           // anti-windup integral max

#define LINE_KP 2
#define LINE_KI 1
#define LINE_I_MAX 100      // smaller integral cap for line sensor PID

// Encoder interrupts
#define ENC_L_PORT  PORTAbits.RA0
#define ENC_R_PORT  PORTAbits.RA1

// Line sensor interrupts
#define LINE_R_PORT PORTAbits.RA2
#define LINE_L_PORT PORTAbits.RA3

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
static int32_t PI_Controller(int32_t error);
static void ResetDriveControl(void);
static void CheckMidpointStop(void);
static void InitDriveCorrectionInterrupts(void);

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static DriveCorrectionState_t CurrentState;
static int32_t Error = 0;
static int32_t Integral = 0;

static int32_t EncCountL = 0;
static int32_t EncCountR = 0;

// Line-following PID state
static int32_t LineError = 0;
static int32_t LineIntegral = 0;

static int32_t MidCount = 0;
static bool UseMidStop = false;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitDriveCorrectionService

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any
     other required initialization for this service
 Notes

 Author
     Ege Turan
****************************************************************************/
bool InitDriveCorrectionService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  CurrentState = DC_Idle;  

  // Set up the interrupts
  InitDriveCorrectionInterrupts();

  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  if (ES_PostToService(MyPriority, ThisEvent) == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/****************************************************************************
 Function
     PostDriveCorrectionService

 Parameters
     EF_Event_t ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     Ege Turan
****************************************************************************/
bool PostDriveCorrectionService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunDriveCorrectionService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes

 Author
   Ege Turan
****************************************************************************/
ES_Event_t RunDriveCorrectionService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  int32_t trim;

  switch(ThisEvent.EventType)
    {
        case ES_START_ENC_FWD:
            ResetDriveControl();
            UseMidStop = false;
            CurrentState = DC_EncFwd;
            break;
        
        case ES_START_ENC_REV:
            ResetDriveControl();
            UseMidStop = false;
            CurrentState = DC_EncRev;
            break;

        case ES_START_ENC_FWD_MID:
            ResetDriveControl();
            UseMidStop = true;
            CurrentState = DC_EncFwdMid;
            break;

        case ES_START_ENC_REV_MID:
            ResetDriveControl();
            UseMidStop = true;
            CurrentState = DC_EncRevMid;
            break;
    }

  switch(CurrentState)    // apply control loop
    {
        case DC_EncFwd:
        case DC_EncFwdMid:

            Error = EncCountL - EncCountR;
            trim = PI_Controller(Error);
            DCMotor_ApplyTrim(trim, Forward);   // uses wrapper

            CheckMidpointStop();
            break;

        case DC_EncRev:
        case DC_EncRevMid:

            Error = EncCountL - EncCountR;
            trim = PI_Controller(Error);
            DCMotor_ApplyTrim(trim, Reverse);   // uses wrapper

            CheckMidpointStop();
            break;

        case DC_LineFwdMid:
        case DC_LineRevMid:
            // controls handled in interrupts
            CheckMidpointStop();
            break;


        default:
            break;
    }

  return ReturnEvent;
}

/***************************************************************************
 ISR (Interrupt Service Routine)
 ***************************************************************************/
void __ISR(_EXTERNAL_0_VECTOR, IPL5SOFT) _EncoderL_ISR(void)
{
    IFS0bits.INT0IF = 0;
    EncCountL++;
    if(UseMidStop) MidCount++;
}

void __ISR(_EXTERNAL_1_VECTOR, IPL5SOFT) _EncoderR_ISR(void)
{
    IFS0bits.INT1IF = 0;
    EncCountR++;
    if(UseMidStop) MidCount++;
}

// Line-following ISR mini PID
void __ISR(_EXTERNAL_2_VECTOR, IPL5SOFT) _LineR_ISR(void)
{
    IFS0bits.INT2IF = 0;

    LineError = 1; // right side off line
    LineIntegral += LineError;
    if(LineIntegral > LINE_I_MAX) LineIntegral = LINE_I_MAX;

    int32_t trim = LINE_KP * LineError + LINE_KI * LineIntegral;
    DCMotor_ApplyTrim(trim, Forward);
}

void __ISR(_EXTERNAL_3_VECTOR, IPL5SOFT) _LineL_ISR(void)
{
    IFS0bits.INT3IF = 0;

    LineError = -1; // left side off line
    LineIntegral += LineError;
    if(LineIntegral < -LINE_I_MAX) LineIntegral = -LINE_I_MAX;

    int32_t trim = LINE_KP * LineError + LINE_KI * LineIntegral;
    DCMotor_ApplyTrim(trim, Forward);
}

/***************************************************************************
 private functions
 ***************************************************************************/
static int32_t PI_Controller(int32_t error)
{
    Integral += error;

    if (Integral > I_MAX) Integral = I_MAX;
    if (Integral < -I_MAX) Integral = -I_MAX;

    return KP * error + KI * Integral;
}

static void ResetDriveControl(void)
{
    Integral = 0;
    Error = 0;
    EncCountL = 0;
    EncCountR = 0;
    MidCount = 0;
    UseMidStop = false;
}

static void CheckMidpointStop(void)
{
    if(UseMidStop && MidCount >= MIDPOINT_COUNT)
    {
        ES_Event_t stop = {ES_MOTORS_OFF};
        PostDCMotorService(stop);

        ES_Event_t done = {ES_COUNT_DONE};
        ES_PostAll(done);   // notify planner
    }
}

static void InitDriveCorrectionInterrupts(void)
{
    // Disable analog on RA0-RA3
    ANSELAbits.ANSA0 = 0;
    ANSELAbits.ANSA1 = 0;
//    ANSELAbits.ANSA2 = 0; // no ANSEL
//    ANSELAbits.ANSA3 = 0; // no ANSEL

    // Inputs
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;

    // ---- Encoder INTs ----

    // INT0 -> RA0 (Left Encoder)
    INTCONbits.INT0EP = 0;
    IFS0bits.INT0IF = 0;
    IEC0bits.INT0IE = 1;

    // INT1 -> RA1 (Right Encoder)
    INTCONbits.INT1EP = 0;
    IFS0bits.INT1IF = 0;
    IEC0bits.INT1IE = 1;

    // ---- Line Sensor INTs ----

    // INT2 -> RA2 (Right Line)
    INTCONbits.INT2EP = 0;
    IFS0bits.INT2IF = 0;
    IEC0bits.INT2IE = 1;

    // INT3 -> RA3 (Left Line)
    INTCONbits.INT3EP = 0;
    IFS0bits.INT3IF = 0;
    IEC0bits.INT3IE = 1;
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

