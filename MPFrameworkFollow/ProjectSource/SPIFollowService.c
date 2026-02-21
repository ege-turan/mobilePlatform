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

// #define SPI_TIMER_MS 20 // NOT USED BY FOLLOWER

/*----------------------------- Module Defines ----------------------------*/
#define DEBUG_PRINT
#include "dbprintf.h"
#define SPI1_SS_PIN SPI_RPB15
#define SPI1_SDO_PIN SPI_RPB13
#define SPI1_SDI_PIN SPI_RPB11
#define SPIClkPeriodInNs 10000 // 100 kHz = 10000 ns

/* LAB 8 Keyboard COMMANDS */
#define LAB8_FWD_FULL            0x09 // 'w'
#define LAB8_REV_FULL            0x11 // 's'
#define LAB8_ROT_CCW_90          0x04 // 'a'
#define LAB8_ROT_CW_90           0x02 // 'd'
#define LAB8_STOP                0x00 // 'x'
#define LAB8_CW_BEACON           0x20 // 'b'
/* LEAD2FOLLOW SPI CMDS*/
#define SPI_LEAD_INITIAL_CMD     0xAA
#define ES_SPI_INTAKE_ON         0xA1
#define ES_SPI_DROPOFF_REACHED   0xA2
#define ES_SPI_BLUE_TEAM         0xA3
#define ES_SPI_GREEN_TEAM        0xA4
/* FOLLOW2LEAD SPI CMDS*/
#define SPI_FOLLOW_INITIAL_CMD   0xFF
#define ES_SPI_START             0xB1
#define ES_SPI_INTAKE_INCOMPLETE 0xB2
#define ES_SPI_LOADED            0xB3
#define ES_SPI_UNLOADED          0xB4
#define ES_SPI_UNLOAD_INCOMPLETE 0xB5
#define ES_SPI_END               0xB6


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static uint8_t message2send;

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

  DB_printf("SPI1 Initialized\r\n");

  // initialize vairables
  message2send = 0xFF;

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
        // Initialize buffer with 0xFF message
        SPIOperate_SPI1_Send8(message2send);
    }
    break;

    case ES_SPI_COMPLETE:
    {
      static uint8_t LastMessage = 0xAA;
      uint8_t newMessage = ThisEvent.EventParam;

      // check if message different and not bad command
      if ((newMessage != LastMessage) && (newMessage != 0xAA))
      {
        ES_Event_t NewEvent;
        NewEvent.EventType  = ES_NEW_SPI_CMD_RECEIVED;
        NewEvent.EventParam = newMessage;
        ES_PostAll(NewEvent);

        #ifdef DEBUG_PRINT
        DB_printf("New SPI from Leader:     0x%x\r\n", (unsigned int)NewEvent.EventParam);
        #endif
      }
      LastMessage = newMessage;

      // write the next message to be sent into the buffer
      SPIOperate_SPI1_Send8(message2send);
    }
      break;
    case ES_NEW_SPI_CMD_SEND:
    {
      message2send = (uint8_t)ThisEvent.EventParam;
    }
      break;

    case ES_NEW_KEY:
    {
      switch (ThisEvent.EventParam) {
        case 'w':
        {
          DB_printf("Received key: w\r\n");
          message2send = LAB8_FWD_FULL;
          DB_printf("\r0x%x: Drive Forward Full Speed\r\n", message2send);
        }
          break;
        case 's':
        {
          DB_printf("Received key: s\r\n");
          message2send = LAB8_REV_FULL;
          DB_printf("\r0x%x: Drive Drive Reverse Full Speed\r\n", message2send);
        }
          break;
        case 'a':
        {
          DB_printf("Received key: a\r\n");
          message2send = LAB8_ROT_CCW_90;
          DB_printf("\r0x%x: Rotate Counter-clockwise by 90 degrees\r\n", message2send);

        }
          break;
        case 'd':
        {
          DB_printf("Received key: d\r\n");
          message2send = LAB8_ROT_CW_90;
          DB_printf("\r0x%x: Rotate Clockwise by 90 degrees\r\n", message2send);
        }
          break;
        case 'x':
        {
          DB_printf("Received key: x\r\n");
          message2send = LAB8_STOP;
          DB_printf("\r0x%x: Stop, hold Position, do not move or rotate\r\n", message2send);
        }
          break;
        case 'b':
        {
          DB_printf("Received key: b\r\n");
          message2send = LAB8_CW_BEACON;
          DB_printf("\r0x%x: Align with Beacon\r\n", message2send);
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

