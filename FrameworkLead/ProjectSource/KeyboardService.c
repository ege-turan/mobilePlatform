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
#include "TemplateService.h"
#include "dbprintf.h"

#include "TopHSMPathPlanner.h"
#include "HSMDriverFromTo.h"
#include "PlansAndSteps.h"
#include "DCMotorService.h"
#include "SPILeadService.h"
#include "BeaconService.h"



/*----------------------------- Module Defines ----------------------------*/
#define VERBOSE_KEYBOARD
/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static ES_Event_t KB_Event1;
static ES_Event_t KB_Event2;
static ES_Event_t KB_Event3;
static ES_Event_t KB_Event4;
static ES_Event_t KB_Event5;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateService

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
bool InitKeyboardService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  DB_printf("\rStarting KeyboardService: ");
  DB_printf("compiled at %s on %s", __TIME__, __DATE__);
  DB_printf("\n\r");
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
     PostTemplateService

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
bool PostKeyboardService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTemplateService

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
ES_Event_t RunKeyboardService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  /********************************************
   in here you write your service code
   *******************************************/
  switch (ThisEvent.EventType)
  {
    // This event is run once at the end of service initialisation
    case ES_INIT:
    {
      DB_printf("\rES_INIT received in KeyboardService, priority: %d\r\n", MyPriority);
    }
      break;


    case ES_NEW_KEY:
    {
      switch (ThisEvent.EventParam) {
        case '1':
        {
          KB_Event1.EventType  = ES_NEW_SPI_CMD_RECEIVED;
          KB_Event1.EventParam = CMD_SPI_START;
          break;
        }
          
        case '2': DB_printf("KEYBOARD2 \r\n"); KB_Event1.EventType  = ES_NEW_SPI_CMD_RECEIVED; KB_Event1.EventParam = 0xb1; break;
        case '3':
        {
          KB_Event1.EventType  = ES_NEW_SPI_CMD_RECEIVED;
          KB_Event1.EventParam = CMD_SPI_UNLOADED;
          break;
        }
        case '4':
        {
          KB_Event1.EventType  = ES_PLAN_DONE;
          DB_printf("KEYBOARD4 \r\n");
          KB_Event1.EventType  = ES_NEW_SPI_CMD_RECEIVED;
          // KB_Event1.EventParam = CMD_SPI_LOADED;
          DB_printf("PostedEvent: %u, with param 0x%x\r\n", (unsigned int) KB_Event1.EventType, (unsigned int) KB_Event1.EventParam);
          break;
        }
        case '5': KB_Event1.EventType  = ES_SIDE_FOUND; KB_Event1.EventParam = 0; break;
        case '6': KB_Event1.EventType  = ES_BEACON_DISPENSER;    break;
        case '7': KB_Event1.EventType  = ES_CENTERED_PIVOT;      break;
        case '8': KB_Event1.EventType  = ES_LINE_PIVOT_L;        break;
        case '9': KB_Event1.EventType  = ES_LINE_PIVOT_R;        break;
        case '0': KB_Event1.EventType  = ES_LIMIT_SWITCH;        break;
        case 'h': KB_Event1.EventType  = ES_COUNT_DONE;          break;
        case 'e': KB_Event1.EventType  = ES_START_ENC_FWD_MID; KB_Event1.EventParam = 0;  break;
        case 'r': KB_Event1.EventType  = ES_START_ENC_REV_MID; KB_Event1.EventParam = 0;  break;
        case 'o': KB_Event1.EventType  = ES_START_LINE_FWD;      break;
        case 'l': KB_Event1.EventType  = ES_START_LINE_REV;      break;
        case 'w': KB_Event1.EventType  = ES_MOTOR_PRIMITIVE; KB_Event1.EventParam = Forwards;  break;
        case 'a': KB_Event1.EventType  = ES_MOTOR_PRIMITIVE; KB_Event1.EventParam = RotateCCW; break;
        case 's': KB_Event1.EventType  = ES_MOTOR_PRIMITIVE; KB_Event1.EventParam = Backwards; break;
        case 'd': KB_Event1.EventType  = ES_MOTOR_PRIMITIVE; KB_Event1.EventParam = RotateCW;  break;
        case 'x': KB_Event1.EventType  = ES_MOTOR_PRIMITIVE; KB_Event1.EventParam = Stop;      break;
        
        
        default:
          KB_Event1.EventType  = ES_MOTORS_OFF;
          DB_printf("ES_MOTORS_OFF Key not linked to KeyboardService msg: %c\r\n", ThisEvent.EventParam);
          break;
      }
      ES_PostAll(KB_Event1);
      DB_printf("PostedEvent: %u, with param 0x%x\r\n", (unsigned int) KB_Event1.EventType, (unsigned int) KB_Event1.EventParam);
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

