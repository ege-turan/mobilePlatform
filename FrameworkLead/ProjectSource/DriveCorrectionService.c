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
#include "dbprintf.h"

#include <sys/attribs.h> // for interrupts

/*----------------------------- Module Defines ----------------------------*/
#define DEBUG_PRINT_DRIVE_CORRECT
#define DRIVE_CONTROL_TIMER_MS 10
#define MIDPOINT_COUNT 1300 //300

#define KP 3 // with 3 its okay, with 1 it's better
#define KI 1
#define I_MAX 100           // anti-windup integral max

#define LINE_KP 2
#define LINE_KI 1
#define LINE_I_MAX 100      // smaller integral cap for line sensor PID

/* ==== Timer-based line following ===== */

#define LINE_UPDATE_HZ      500     // 2 ms loop
#define T4_PRESCALE_BITS    0b111   // 1:256
#define T4_PRESCALE         256
#define PBCLK_RATE          20000000L

#define T4_PERIOD ((PBCLK_RATE / T4_PRESCALE) / LINE_UPDATE_HZ - 1)

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

static void SetupLineTimer(void);
static int32_t Line_PI_Controller(int32_t error);

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static DriveCorrectionState_t CurrentState;
static int32_t Error = 0;
static int32_t Integral = 0;
static int32_t PI_result_val = 0;

static int32_t EncCountL = 0;
static int32_t EncCountR = 0;

// Line-following PID state
static int32_t LineError = 0;
static int32_t LineIntegral = 0;

static int32_t MidCount = 0;
static bool UseMidStop = false;
static bool encON = false;

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

  InitDriveCorrectionInterrupts();
  ES_Timer_InitTimer(DriveCorrectionTimer, DRIVE_CONTROL_TIMER_MS);

  ThisEvent.EventType = ES_INIT;
  return ES_PostToService(MyPriority, ThisEvent);
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
        case ES_INIT:
        {
        DB_printf("\rES_INIT received in DriveCorrectionService, priority: %d\r\n", MyPriority);
        }
        break;
        case ES_TIMEOUT:
          if (ThisEvent.EventParam == DriveCorrectionTimer)
          {
            ES_Timer_InitTimer(DriveCorrectionTimer, DRIVE_CONTROL_TIMER_MS); // re-start timer
            #ifdef DEBUG_PRINT_DRIVE_CORRECT
            if (encON){
            DB_printf("[leader ]  EncLeft: %u, EncRight: %u, PI_result_val: %d \r\n", (unsigned int)EncCountL, (unsigned int)EncCountR, PI_result_val);
            }
            #endif
          }
          break;
        case ES_STOP_DRIVE_CORRECT:
            ResetDriveControl();
            CurrentState = DC_Idle;
            DB_printf("[leader ] DriveCorrectionService: DC_Idle\r\n");
            break;
        case ES_START_ENC_FWD:
            // #ifdef DEBUG_PRINT_DRIVE_CORRECT
            DB_printf("[leader ] Forwards w/ encoder\r\n");
            // #endif
            ResetDriveControl();
            encON = true;
            UseMidStop = false;
            CurrentState = DC_EncFwd;
            break;
        
        case ES_START_ENC_REV:
            // #ifdef DEBUG_PRINT_DRIVE_CORRECT
            DB_printf("[leader ] Backwards w/ encoder\r\n");
            // #endif
            ResetDriveControl();
            encON = true;
            UseMidStop = false;
            CurrentState = DC_EncRev;
            break;

        case ES_START_ENC_FWD_MID:
            // #ifdef DEBUG_PRINT_DRIVE_CORRECT
            DB_printf("[leader ] Forwards w/ encoder and mid\r\n");
            // #endif
            ResetDriveControl();
            encON = true;
            UseMidStop = true;
            CurrentState = DC_EncFwdMid;
            break;

        case ES_START_ENC_REV_MID:
            // #ifdef DEBUG_PRINT_DRIVE_CORRECT
            DB_printf("[leader ] Backwards w/ encoder and mid\r\n");
            // #endif
            ResetDriveControl();
            encON = true;
            UseMidStop = true;
            CurrentState = DC_EncRevMid;
            break;

        case ES_START_LINE_FWD:
            // #ifdef DEBUG_PRINT_DRIVE_CORRECT
            DB_printf("[leader ] Forwards w/ line\r\n");
            // #endif
            ResetDriveControl();
            UseMidStop = false;
            CurrentState = DC_LineFwdMid;
            break;

        case ES_START_LINE_REV:
            // #ifdef DEBUG_PRINT_DRIVE_CORRECT
            DB_printf("[leader ] Backwards w/ line\r\n");
            // #endif
            ResetDriveControl();
            UseMidStop = false;
            CurrentState = DC_LineRevMid;
            break;

        case ES_START_LINE_FWD_MID:
            // #ifdef DEBUG_PRINT_DRIVE_CORRECT
            DB_printf("[leader ] Forwards w/ line and mid\r\n");
            // #endif
            ResetDriveControl();
            UseMidStop = true;
            CurrentState = DC_LineFwdMid;
            break;

        case ES_START_LINE_REV_MID:
            // #ifdef DEBUG_PRINT_DRIVE_CORRECT
            DB_printf("[leader ] Backwards w/ line and mid\r\n");
            // #endif
            ResetDriveControl();
            UseMidStop = true;
            CurrentState = DC_LineRevMid;
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

        case DC_Idle:
            break;


        default:
            break;
    }

  return ReturnEvent;
}

/***************************************************************************
 ISR (Interrupt Service Routine)
 ***************************************************************************/
void __ISR(_EXTERNAL_4_VECTOR, IPL5SOFT) _EncoderL_ISR(void)
{
    IFS0bits.INT4IF = 0;
    EncCountL++;
    if(UseMidStop) MidCount++;
}

void __ISR(_EXTERNAL_3_VECTOR, IPL5SOFT) _EncoderR_ISR(void)
{
    IFS0bits.INT3IF = 0;
    EncCountR++;
    if(UseMidStop) MidCount++;
}

// Line-following ISR mini PID
void __ISR(_TIMER_4_VECTOR, IPL4SOFT) _Line_Timer_ISR(void)
{
    IFS0bits.T4IF = 0;

    if(CurrentState != DC_LineFwdMid &&
       CurrentState != DC_LineRevMid)
        return;

    uint8_t L = LINE_L_PORT;
    uint8_t R = LINE_R_PORT;

    if(!L && R)
        LineError = +1;
    else if(L && !R)
        LineError = -1;
    else
        LineError = 0;

    int32_t trim = Line_PI_Controller(LineError);

    if(CurrentState == DC_LineFwdMid)
        DCMotor_ApplyTrim(trim, Forward);
    else
        DCMotor_ApplyTrim(trim, Reverse);
}

/***************************************************************************
 private functions
 ***************************************************************************/
static int32_t PI_Controller(int32_t error)
{
    Integral += error;

    if (Integral > I_MAX) Integral = I_MAX;
    if (Integral < -I_MAX) Integral = -I_MAX;

    PI_result_val = KP * error + KI * Integral;
    
    return PI_result_val;
}

static int32_t Line_PI_Controller(int32_t error)
{
    if(error != 0) LineIntegral += error;

    if(LineIntegral > LINE_I_MAX) LineIntegral = LINE_I_MAX;
    if(LineIntegral < -LINE_I_MAX) LineIntegral = -LINE_I_MAX;

    return LINE_KP * error + LINE_KI * LineIntegral;
}

static void SetupLineTimer(void)
{
    T4CON = 0;
    T4CONbits.TCKPS = T4_PRESCALE_BITS;

    PR4  = T4_PERIOD;
    TMR4 = 0;

    IPC4bits.T4IP = 4;
    IPC4bits.T4IS = 0;

    IFS0bits.T4IF = 0;
    IEC0bits.T4IE = 1;

    T4CONbits.TON = 1;
}

static void ResetDriveControl(void)
{
    Integral = 0;
    PI_result_val = 0;
    Error = 0;
    EncCountL = 0;
    EncCountR = 0;
    MidCount = 0;
    UseMidStop = false;
    encON = false;

    LineIntegral = 0;
    LineError = 0;
}

static void CheckMidpointStop(void)
{
    int32_t avg = (EncCountL + EncCountR) >> 1;

    if(UseMidStop && avg >= MIDPOINT_COUNT)
    {
        UseMidStop = false;   // prevent retrigger
        MidCount = 0;

        ES_Event_t stop = {ES_MOTORS_OFF};
        PostDCMotorService(stop);
        PostDriveCorrectionService((ES_Event_t){ES_STOP_DRIVE_CORRECT});

        ES_Event_t done = {ES_COUNT_DONE};
        ES_PostAll(done);   // notify planner
    }
}

static void InitDriveCorrectionInterrupts(void)
{
    __builtin_disable_interrupts();

    // ---- Disable analog on encoder + line pins ----
    ANSELAbits.ANSA0 = 0;
    ANSELAbits.ANSA1 = 0;
    //    ANSELAbits.ANSA2 = 0; // no ANSEL
    //    ANSELAbits.ANSA3 = 0; // no ANSEL

    // ---- Set inputs ----
    TRISAbits.TRISA0 = 1;   // ENC L
    TRISAbits.TRISA1 = 1;   // ENC R
    TRISAbits.TRISA2 = 1;   // LINE R
    TRISAbits.TRISA3 = 1;   // LINE L

    // Map RA0 -> INT4
    INT4Rbits.INT4R = 0b0000;

    // Map RA1 -> INT3
    INT3Rbits.INT3R = 0b0000;

    // ---- Encoder Interrupts ----

    // INT4 -> RA0 (Left)
    INTCONbits.INT4EP = 0;   // rising edge
    IFS0bits.INT4IF = 0;
    IEC0bits.INT4IE = 1;
    IPC4bits.INT4IP = 5;

    // INT3 -> RA1 (Right)
    INTCONbits.INT3EP = 0;
    IFS0bits.INT3IF = 0;
    IEC0bits.INT3IE = 1;
    IPC3bits.INT3IP = 5;

    // ---- Line Timer ----
    SetupLineTimer();

    __builtin_enable_interrupts();
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

