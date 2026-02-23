/****************************************************************************
 Module
   TemplateService.c

 Revision
   1.0.1

 Description
   This file implements TemplateService as a simple service under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 25             ege      updated and cleaned
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "PIC32_PWM_Lib.h"
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

#define HIGH_PULSE_WIDTH_US 5000.0 // in us
#define PWM_FREQ_HZ (1.0/((2.0*HIGH_PULSE_WIDTH_US) * 1e-6)) // in Hz
#define PWM_PERIOD_TICKS (20000000.0/(256.0*PWM_FREQ_HZ)) // in ticks, with PBCLK = 20 MHz and prescaler of 256

#define STEPPER_TIMER _Timer2_
#define STEPPER_PRESCALER PWM_PS_256
#define STEPPER_CHANNEL 5
#define STEPPER_PIN PWM_RPA2

// #define MOTOR_OUTPUT_ANSEL (ANSELAbits.ANSA2)  // analog/digital mode of RA2, RA2 doesnt have ANSEL
#define MOTOR_OUTPUT_TRIS  (TRISAbits.TRISA2)  // input/output mode of RA2
#define MOTOR_OUTPUT_LAT   (LATAbits.LATA2)    // output latch of RA2

#define TEST_TIMER_MS 20000 // in ms
/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
void _InitMotorPWM(WhichTimer_t thisTimer, PWM_Prescaler_t thisPrescaler, uint8_t thisChannel, PWM_PinMap_t thisPin);
/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

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

  // Initialise PWM stuff
  _InitMotorPWM(STEPPER_TIMER, STEPPER_PRESCALER, STEPPER_CHANNEL, STEPPER_PIN);
  // Initialize pins as digital outputs
  MOTOR_OUTPUT_TRIS = 0;
  MOTOR_OUTPUT_LAT = 0; // start with low output
  // Initialize PWM at 0% duty cycle (0%)
  PWM_Operate_SetPulseWidthOnChannel(0, STEPPER_CHANNEL);
  // Initialize test timer
  ES_Timer_InitTimer(STEPPER_TEST_TIMER, TEST_TIMER_MS);
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
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  /********************************************
   in here you write your service code
   *******************************************/
  switch (ThisEvent.EventType)
  {
    case ES_INIT:
    {
      DB_printf("\rES_INIT received in SPILeadService, priority: %d\r\n", MyPriority);
      PWM_Operate_SetPulseWidthOnChannel((PWM_PERIOD_TICKS / 2), STEPPER_CHANNEL); // 50% duty cycle
    }
    break;

    case ES_TIMEOUT:
    {
      if (ThisEvent.EventParam == STEPPER_TEST_TIMER)
      {
        DB_printf("StepperService: Test Timer Timeout\r\n");
        // Toggle between 0% and 50% duty cycle to test stepping
        static bool isOn = true;
        if (isOn)
        {
          PWM_Operate_SetPulseWidthOnChannel(0, STEPPER_CHANNEL); // off
        }
        else
        {
          PWM_Operate_SetPulseWidthOnChannel(PWM_PERIOD_TICKS / 2, STEPPER_CHANNEL); // 50% duty cycle
        }
        isOn = !isOn;
        // Restart timer
        ES_Timer_InitTimer(STEPPER_TEST_TIMER, TEST_TIMER_MS);
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
void _InitMotorPWM(WhichTimer_t thisTimer, PWM_Prescaler_t thisPrescaler, uint8_t thisChannel, PWM_PinMap_t thisPin)
{
    // Initialize Timer 2 for PWM
    PWM_Setup_ConfigureTimer(thisTimer, PWM_PERIOD_TICKS, thisPrescaler);

    // Initialize OC1 for PWM1 output
    PWM_Setup_SetChannel(thisChannel);
    PWM_Setup_AssignChannelToTimer(thisChannel, thisTimer);

    // Assign output compares for both pwm channels to respective pins
    PWM_Setup_MapChannelToOutputPin(thisChannel, thisPin);
}
/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

