/****************************************************************************
 Module
   FieldSideServoService.c

 Revision
   1.0.1

 Description
   This file implements FieldSideServoService as a simple service under the
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
#include "FieldSideServoService.h"
#include "PIC32_PWM_Lib.h"
#include "SPIFollowService.h"

/*----------------------------- Module Defines ----------------------------*/
#define M_BG_PWM_CH      3
#define M_BG_PWM_PIN     PWM_RPB10
#define M_BG_PWM_TIMER   _Timer3_

#define PWM_SERVO_CENTER 3750  // 1.5 ms pulse width at 20 ms period
#define PWM_SERVO_SIDE   2500  // 1 ms
#define PWM_SERVO_OTHER  5000  // 2 ms

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitFieldSideServoService

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
bool InitFieldSideServoService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;

  // ----- Configure Timer3 for PWM at 50 Hz (20 ms period) -----
  // PBCLK = 20 MHz, Prescaler = 64 -> Period ticks = 20e6 / (64 * 50) = 6250
  PWM_Setup_ConfigureTimer(M_BG_PWM_TIMER, 6250, PWM_PS_64);

  // Map PWM channel for servo
  PWM_Setup_SetChannel(M_BG_PWM_CH);
  PWM_Setup_AssignChannelToTimer(M_BG_PWM_CH, M_BG_PWM_TIMER);
  PWM_Setup_MapChannelToOutputPin(M_BG_PWM_CH, M_BG_PWM_PIN);

  // Initialize servo to center
  PWM_Operate_SetPulseWidthOnChannel(PWM_SERVO_CENTER, M_BG_PWM_CH);

  // Post initial transition event
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
     PostFieldSideServoService

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
bool PostFieldSideServoService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunFieldSideServoService

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
ES_Event_t RunFieldSideServoService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (ThisEvent.EventType)
  {
      case ES_NEW_SPI_CMD_RECEIVED:
      {
          switch (ThisEvent.EventParam)
          {
              case CMD_SPI_BLUE_TEAM:
                  PWM_Operate_SetPulseWidthOnChannel(PWM_SERVO_SIDE, M_BG_PWM_CH);
                  break;

              case CMD_SPI_GREEN_TEAM:
                  PWM_Operate_SetPulseWidthOnChannel(PWM_SERVO_OTHER, M_BG_PWM_CH);
                  break;
          }
      }
      break;

      case ES_RESET:
          PWM_Operate_SetPulseWidthOnChannel(PWM_SERVO_CENTER, M_BG_PWM_CH);
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

