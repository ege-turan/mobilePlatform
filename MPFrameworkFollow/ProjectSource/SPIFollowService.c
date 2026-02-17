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
#include "SPIFollowService.h"

#include "PIC32_SPI_HAL.h"
#include <sys/attribs.h>

#define DEBUG_PRINT
#ifdef DEBUG_PRINT
    #include "dbprintf.h"
#endif

#define SPI1_SS_PIN SPI_RPB15
#define SPI1_SDO_PIN SPI_RPB13
#define SPI1_SDI_PIN SPI_RPB11
#define SPIClkPeriodInNs 10000 // 100 kHz = 10000 ns

// #define SPI_TIMER_MS 50 // NOT USED BY FOLLOWER

/*----------------------------- Module Defines ----------------------------*/

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
     InitSPIService

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
bool InitSPIFollowService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  clrScrn();
  DB_printf("\rStarting SPIFollowService: ");
  DB_printf("compiled at %s on %s", __TIME__, __DATE__);
  DB_printf("\n\r");

  SPISetup_BasicConfig(SPI_SPI1);                    // basic SPI setup
  SPISetup_SetFollower(SPI_SPI1);                    // set as follower
  // SPISetup_SetBitTime(SPI_SPI1, SPIClkPeriodInNs);   // 100 kHz = 10000 ns
  SPISetup_SetClockIdleState(SPI_SPI1, SPI_CLK_HI);  // clock idle high
  SPISetup_SetActiveEdge(SPI_SPI1, SPI_SECOND_EDGE); // transmit on active-to-idle edge
  SPISetup_SetXferWidth(SPI_SPI1, SPI_8BIT);         // 8-bit word mode
  SPISetEnhancedBuffer(SPI_SPI1, true);              // enable enhanced buffer
  SPISetup_MapSSInput(SPI_SPI1, SPI1_SS_PIN);        // map SS pin
  SPISetup_MapSDOutput(SPI_SPI1, SPI1_SDO_PIN);      // map SDO pin
  SPISetup_MapSDInput(SPI_SPI1, SPI1_SDI_PIN); // DASO, check this function to map SDI pin

  SPISetup_EnableSPI(SPI_SPI1); // enable SPI
  IFS0CLR = _IFS0_INT4IF_MASK;  // clear INT4IF to avoid false flags

#ifdef DEBUG_PRINT
  DB_printf("SPI1 Initialized\r\n");
#endif

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
     PostSPIFollowService

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
bool PostSPIFollowService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunSPIFollowService

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
ES_Event_t RunSPIFollowService(ES_Event_t ThisEvent)
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
        DB_printf("\rES_INIT received in SPIFollowService, priority: %d\r\n", MyPriority);
    }
    break;
    case ES_NEW_KEY:
    {
      switch (ThisEvent.EventParam) {
        case 'w':
        {
          DB_printf("Received key: w\r\n");
          SPIOperate_SPI1_Send8(0x09);
          DB_printf("\r0x09: Drive Forward Full Speed\r\n");
        }
          break;
        case 'a':
        {
          DB_printf("Received key: a\r\n");
          SPIOperate_SPI1_Send8(0x11);
          DB_printf("\r0x10: Drive Drive Reverse Full Speed\r\n");
        }
          break;
        case 's':
        {
          DB_printf("Received key: s\r\n");
          SPIOperate_SPI1_Send8(0x04);
          DB_printf("\r0x04: Rotate Counter-clockwise by 90 degrees\r\n");

        }
          break;
        case 'd':
        {
          DB_printf("Received key: d\r\n");
          SPIOperate_SPI1_Send8(0x02);
          DB_printf("\r0x02: Rotate Clockwise by 90 degrees\r\n");
        }
          break;
        case 'x':
        {
          DB_printf("Received key: x\r\n");
          SPIOperate_SPI1_Send8(0x00);
          DB_printf("\r0x00: Stop, hold Position, do not move or rotate\r\n");
        }
          break;
        default:
          DB_printf("Received key: %c\r\n", ThisEvent.EventParam);
          break;
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

