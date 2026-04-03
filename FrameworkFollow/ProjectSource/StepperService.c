/****************************************************************************
 Module
   StepperService.c

 Revision
   1.0.1

 Description
   This file implements TemplateService as a simple service under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 2026-03-01     ege      converted to timer-driven (no PWM)
 25             ege      updated and cleaned
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "dbprintf.h"
#include <sys/attribs.h>
#include "StepperService.h"

/* DASO ASSUMPTIONS WRITING THIS SERVICE
 -using A4988 min length of high pulse width is 1us
 -using A4988 min length of low  pulse width is 1us
 -I am going to run it as a PWM with 50% duty cycle
 -0% duty cycle means off, 50% means full step as fast as possible.
*/

/*----------------------------- Module Defines ----------------------------*/
// use prescaler of 256 to have bigger pulse width range before overflow
#define FEEDER_DUTY_STEPS 5      // 0,20,40,60,80,100%

// #define MOTOR_OUTPUT_ANSEL (ANSELAbits.ANSA2)  // analog/digital mode of RA2, RA2 doesnt have ANSEL
#define MOTOR_OUTPUT_TRIS  (TRISAbits.TRISA2)  // input/output mode of RA2
#define MOTOR_OUTPUT_LAT   (LATAbits.LATA2)    // output latch of RA2

// ENABLE PIN
#define M_FEEDER_EN_LAT  (LATBbits.LATB12)
#define M_FEEDER_EN_TRIS (TRISBbits.TRISB12)

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static uint8_t DutyCycle = 1;     // 0–5
static uint8_t HighTime = 0;
static uint8_t LowTime = 0;
static uint32_t ForwardsTimeMs = 800;
static uint32_t BackwardsTimeMs = 20;
static bool forwards = true;
static bool OutputState = false;
static bool feederOn = false;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitStepperService

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
bool InitStepperService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  DB_printf("\rStarting StepperService: ");
  DB_printf("compiled at %s on %s", __TIME__, __DATE__);
  DB_printf("\n\r");

  // Initialize pins as digital outputs
  MOTOR_OUTPUT_TRIS = 0;
  MOTOR_OUTPUT_LAT = 0; // start with low output
  // Initialize feeder enable pin
  M_FEEDER_EN_TRIS = 0;   // output (EXTRA CONTROL)
  M_FEEDER_EN_LAT  = 0;   // DISABLED

  
  // Initialize variables

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
     PostStepperService

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
bool PostStepperService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunStepperService

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
ES_Event_t RunStepperService(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT;

    switch (ThisEvent.EventType)
    {
        case ES_INIT:
        {
            DB_printf("\rStepperService initialized\r\n");
            M_FEEDER_EN_LAT = 0;     // disable driver
            MOTOR_OUTPUT_LAT = 0;    // start step pin low
        }
        break;

        case ES_FEEDER_START:
        {
            DB_printf("[follower ] StepperService: FEEDER START\r\n");
            feederOn = true;

            uint8_t period = FEEDER_DUTY_STEPS;

            HighTime = DutyCycle;
            LowTime = period - DutyCycle;

            OutputState = true;
            M_FEEDER_EN_LAT = 1; MOTOR_OUTPUT_LAT = 0; // forwards

            ES_Timer_InitTimer(FEEDER_PWM_TIMER, HighTime);

            forwards = true;
            ES_Timer_InitTimer(FEEDER_UNSTALL_TIMER, ForwardsTimeMs);
        }
        break;

        case ES_FEEDER_STOP:
        {
            DB_printf("[follower ] StepperService: FEEDER STOP\r\n");
            feederOn = false;
            M_FEEDER_EN_LAT = 0; MOTOR_OUTPUT_LAT = 0; // STOP
            ES_Timer_StopTimer(FEEDER_PWM_TIMER);     // stop toggling
            ES_Timer_StopTimer(FEEDER_UNSTALL_TIMER); // stop unstall timer
        }
        break;

        case ES_TIMEOUT:
        {
            if (ThisEvent.EventParam == FEEDER_PWM_TIMER)
            {
                if(OutputState)
                {
                    M_FEEDER_EN_LAT = 0; MOTOR_OUTPUT_LAT = 0; // STOP
                    OutputState = false;
                    if (LowTime > 0)
                        ES_Timer_InitTimer(FEEDER_PWM_TIMER, LowTime);
                } else
                {
                    if(forwards)
                    {
                        if (feederOn) {M_FEEDER_EN_LAT = 1; MOTOR_OUTPUT_LAT = 0;}// forwards
                    }
                    else
                    {
                        if (feederOn) {M_FEEDER_EN_LAT = 0; MOTOR_OUTPUT_LAT = 1;} // backwards
                    }

                    OutputState = true;

                    if (HighTime > 0)
                        ES_Timer_InitTimer(FEEDER_PWM_TIMER, HighTime);
                }
            } else if (ThisEvent.EventParam == FEEDER_UNSTALL_TIMER)
            {
                forwards = !forwards; // toggle direction
                if (forwards)
                {
                    DB_printf("[follower ] StepperService: UNSTALL TIMER, FORWARDS\r\n");
                    if (feederOn) {M_FEEDER_EN_LAT = 1; MOTOR_OUTPUT_LAT = 0;}// forwards
                    ES_Timer_InitTimer(FEEDER_UNSTALL_TIMER, ForwardsTimeMs);
                }
                else
                {
                    DB_printf("[follower ] StepperService: UNSTALL TIMER, BACKWARDS\r\n");
                    if (feederOn) {M_FEEDER_EN_LAT = 0; MOTOR_OUTPUT_LAT = 1;} // backwards
                    ES_Timer_InitTimer(FEEDER_UNSTALL_TIMER, BackwardsTimeMs);
                }
            }
        }
        break;

        default:
            break;
    }

    return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

