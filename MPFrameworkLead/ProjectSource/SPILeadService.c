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
#include "SPILeadService.h"

#include "PIC32_SPI_HAL.h"
#include <sys/attribs.h>


/*----------------------------- Module Defines ----------------------------*/
#define DEBUG_PRINT
#include "dbprintf.h"

#define SPI1_SS_PIN SPI_RPB15
#define SPI1_SDO_PIN SPI_RPB13
#define SPI1_SDI_PIN SPI_RPB11
#define SPIClkPeriodInNs 10000 // 100 kHz = 10000 ns

#define SPI_TIMER_MS 20


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
     InitSPILeadService

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
bool InitSPILeadService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  clrScrn();
  DB_printf("\rStarting SPILeadService: ");
  DB_printf("compiled at %s on %s", __TIME__, __DATE__);
  DB_printf("\n\r");

  SPISetup_BasicConfig(SPI_SPI1);                    // basic SPI setup
  SPISetup_SetLeader(SPI_SPI1, SPI_SMP_MID);         // set as leader
  SPISetup_SetBitTime(SPI_SPI1, SPIClkPeriodInNs);   // 100 kHz = 10000 ns
  SPISetup_SetClockIdleState(SPI_SPI1, SPI_CLK_HI);  // clock idle high
  SPISetup_SetActiveEdge(SPI_SPI1, SPI_SECOND_EDGE); // transmit on active-to-idle edge
  SPISetup_SetXferWidth(SPI_SPI1, SPI_8BIT);         // 8-bit word mode
  SPISetEnhancedBuffer(SPI_SPI1, true);              // enable enhanced buffer
  SPISetup_MapSSOutput(SPI_SPI1, SPI1_SS_PIN);       // map SS pin
  SPISetup_MapSDOutput(SPI_SPI1, SPI1_SDO_PIN);      // map SDO pin
  SPISetup_MapSDInput(SPI_SPI1, SPI1_SDI_PIN); // DASO, check this function to map SDI pin

  SPISetup_EnableSPI(SPI_SPI1); // enable SPI
  IFS0CLR = _IFS0_INT4IF_MASK;  // clear INT4IF to avoid false flags

#ifdef DEBUG_PRINT
  DB_printf("SPI1 Initialized\r\n");
#endif

  // Initialize timers and variables
  ES_Timer_InitTimer(SPI_TIMER, SPI_TIMER_MS);
  message2send = CMD_SPI_LEAD_INITIAL;

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
     PostSPILeadService

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
bool PostSPILeadService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunSPILeadService

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
ES_Event_t RunSPILeadService(ES_Event_t ThisEvent)
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
      DB_printf("\rES_INIT received in SPILeadService, priority: %d\r\n", MyPriority);
    }
      break;
    case ES_NEW_SPI_CMD_SEND:
    {
      message2send = (uint8_t)ThisEvent.EventParam;
    }
      break;
    case ES_TIMEOUT:
    {
      if (ThisEvent.EventParam == SPI_TIMER)
      {
        static uint8_t LastMessage = 0xFF;

        // If previous SPI transfer finished, read its result
        if (SPIOperate_HasSS1_Risen())
        {
          uint8_t newMessage = (uint8_t)SPIOperate_ReadData(SPI_SPI1);
          // DB_printf("Follower Sent Over SPI:     0x%x\r\n", (unsigned int)newMessage);

          // check if message different and not bad command
          if ((newMessage != LastMessage) && (newMessage != CMD_SPI_FOLLOW_INITIAL))
          {
            ES_Event_t NewEvent;
            NewEvent.EventType  = ES_NEW_SPI_CMD_RECEIVED;
            NewEvent.EventParam = newMessage;
            ES_PostAll(NewEvent);

            #ifdef DEBUG_PRINT
            DB_printf("New SPI from Follower:     0x%x\r\n", (unsigned int)NewEvent.EventParam);
            #endif
          }

          LastMessage = newMessage;
        }

        // Always Querry CommandGenerator for new command
        SPIOperate_SPI1_Send8(message2send);

        ES_Timer_InitTimer(SPI_TIMER, SPI_TIMER_MS); // re-start timer
      }
    }
      break;

    case ES_NEW_KEY:
    {
      switch (ThisEvent.EventParam) {
        case 'p':
        {
          message2send = 0xD1;
          DB_printf("\rSend 0xD1 over SPI\r\n");
        }
          break;
        case 'o':
        {
          message2send = 0xD2;
          DB_printf("\rSend 0xD2 over SPI\r\n");
        }
          break;
        case 'i':
        {
          message2send = 0xD3;
          DB_printf("\rSend 0xD3 over SPI\r\n");
        }
          break;
        case 'u':
        {
          message2send = 0xD4;
          DB_printf("\rSend 0xD4 over SPI\r\n");
        }
          break;
        
        default:
          // DB_printf("Key not linked to SPI msg: %c\r\n", ThisEvent.EventParam);
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

