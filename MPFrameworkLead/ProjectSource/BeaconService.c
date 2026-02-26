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
#include "BeaconService.h"
#include "SPILeadService.h"
#include "dbprintf.h"
#include "PIC32_IC_Lib.h"
#include <sys/attribs.h>
#include <xc.h>


/*----------------------------- Module Defines ----------------------------*/
#define BEACON_FREQ 1427 // in Hz
#define BEACON_TOL 25     // in Hz

#define BEACON_G_FREQ 3333 // in Hz dispenser green side
#define BEACON_B_FREQ 1427 // in Hz dispenser blue side
#define BEACON_R_FREQ 909  // in Hz close to green dispenser
#define BEACON_L_FREQ 2000 // in Hz close to blue dispenser

#define BEACON_B 1
#define BEACON_L 2
#define BEACON_R 3
#define BEACON_G 4

#define IC_TIMER _Timer3_
#define BEACON_IC_CHANNEL IC3
#define BEACON_IC_PeriodTicks 0x10000 // 16-bit timer, max period is 65536 ticks
#define BEACON_IC_Prescaler IC_PS_2 // 1:2 prescaler
// IC3R = 0b0001; // Map IC3 to RPB5

#define BEACON_TIMER_MS 50 // in ms, how often to check for beacon presence


// #define VERBOSE_BEACON
/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
static uint32_t GetFreqFromPeriod(uint32_t periodTicks);
static bool CheckSideSequence(uint8_t detectedBeacon, uint8_t *sideFound);

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

// IC3 is in RB5 (0001 = RPB5, IC3)
// static volatile uint32_t currentTime = 0;
static volatile uint16_t rollover      = 0;
static volatile uint32_t periodInTicks = 0;
static volatile uint32_t lastCapture   = 0;

typedef union
{
    uint32_t time;
    uint16_t byBytes[2];
} TimeUnion;


#define SEQ_LENGTH 3

static const uint8_t GreenSequence[SEQ_LENGTH] = {BEACON_L, BEACON_R, BEACON_G}; // Assumes CCW rotation
static const uint8_t BlueSequence[SEQ_LENGTH]  = {BEACON_R, BEACON_L, BEACON_B}; // Assumes CCW rotation
static uint8_t BeaconBuffer[SEQ_LENGTH];
static uint8_t BufferIndex = 0;
static bool BufferFilled = false;
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
bool InitBeaconService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  // announce
  DB_printf("\rStarting BeaconService: ");
  DB_printf("compiled at %s on %s", __TIME__, __DATE__);
  DB_printf("\n\r");
  // Initialize Timer 3 and Input Capture 2 for beacon detection
  IC_Setup_ConfigureTimer(IC_TIMER, BEACON_IC_PeriodTicks, BEACON_IC_Prescaler);
  IC_Setup_EnableChannel(BEACON_IC_CHANNEL, IC_RisingEdge);

  // IC_Setup_MapChannelToPin(IC3, IC_RPB5); // Check w/ Ege, I think this doesn't work as intended
  IC3R = 0b0001; // Map IC3 to RPB5
  __builtin_enable_interrupts();
  DB_printf("Beacon IC and Timer initialized\r\n");

  // start timer for beacon detection
  ES_Timer_InitTimer(BeaconDetectTimer, BEACON_TIMER_MS); // check for beacon every BEACON_TIMER_MS ms

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
bool PostBeaconService(ES_Event_t ThisEvent)
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
ES_Event_t RunBeaconService(ES_Event_t ThisEvent)
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
      DB_printf("BeaconService received ES_INIT\r\n");
    }
    break;

    case ES_TIMEOUT:
    {
      if (ThisEvent.EventParam == BeaconDetectTimer)
      {
        static uint8_t LastBeaconState = 0;
        uint8_t CurrentBeaconState;
        uint32_t CurrentBeaconFreq;
        bool ReturnVal = false;

        uint32_t CurrentPeriodinTicks = periodInTicks;
        CurrentBeaconFreq             = GetFreqFromPeriod(CurrentPeriodinTicks);

        if ((CurrentBeaconFreq >= (BEACON_B_FREQ - BEACON_TOL)) &&
                 (CurrentBeaconFreq <= (BEACON_B_FREQ + BEACON_TOL)))
        {
            CurrentBeaconState = BEACON_B;
        }
        else if ((CurrentBeaconFreq >= (BEACON_L_FREQ - BEACON_TOL)) &&
                 (CurrentBeaconFreq <= (BEACON_L_FREQ + BEACON_TOL)))
        {
            CurrentBeaconState = BEACON_L;
        }
        else if ((CurrentBeaconFreq >= (BEACON_R_FREQ - BEACON_TOL)) &&
                 (CurrentBeaconFreq <= (BEACON_R_FREQ + BEACON_TOL)))
        {
            CurrentBeaconState = BEACON_R;
        }
        else if ((CurrentBeaconFreq >= (BEACON_G_FREQ - BEACON_TOL)) &&
                 (CurrentBeaconFreq <= (BEACON_G_FREQ + BEACON_TOL)))
        {
            CurrentBeaconState = BEACON_G;
        }
        else if ((CurrentBeaconFreq >= (BEACON_FREQ - BEACON_TOL)) &&
            (CurrentBeaconFreq <= (BEACON_FREQ + BEACON_TOL)))
        {
            CurrentBeaconState = 5;
        }
        else 
        {
            CurrentBeaconState = 0;
        }

        // event detected, so post detected event
        if ((CurrentBeaconState != 0)) // && (CurrentBeaconState != LastBeaconState))
        {
          #ifdef VERBOSE_BEACON
          DB_printf("Beacon Detected! Freq: %u Hz. State: %u\r\n", (unsigned int)CurrentBeaconFreq, CurrentBeaconState);
          #endif
          if (CurrentBeaconState == BEACON_G || CurrentBeaconState == BEACON_B)
          {
            ES_Event_t NewEvent;
            NewEvent.EventType = ES_BEACON_DISPENSER;
            ES_PostAll(NewEvent);
          }
        } else
        {
          #ifdef VERBOSE_BEACON
          DB_printf("No Beacon. Freq: %u Hz\r\n", (unsigned int)CurrentBeaconFreq);
          #endif
        }

        if ((CurrentBeaconState != 0))// && (CurrentBeaconState != LastBeaconState))
        {
          uint8_t sideDetected;
          if (CheckSideSequence(CurrentBeaconState, &sideDetected))
          {
            // Side detected, post ES_SIDE_FOUND
            ES_Event_t SideEvent;
            SideEvent.EventType = ES_SIDE_FOUND;
            SideEvent.EventParam = (sideDetected == BEACON_G) ? ES_TEAM_GREEN : ES_TEAM_BLUE;
            ES_PostAll(SideEvent);
            // SPI event
            ES_Event_t SPIEvent;
            SPIEvent.EventType = ES_NEW_SPI_CMD_SEND;
            SPIEvent.EventParam = (sideDetected == BEACON_G) ? CMD_SPI_GREEN_TEAM : CMD_SPI_BLUE_TEAM;
            PostSPILeadService(SPIEvent);

            #ifdef VERBOSE_BEACON
            DB_printf("Side Found! %s\r\n", (sideDetected == BEACON_G) ? "GREEN" : "BLUE");
            #endif
          }
          if (CurrentBeaconState == BEACON_G)
          {
            DB_printf("Green Sequence Detected: ");
            ES_Event_t NewEvent;
            NewEvent.EventType = ES_BEACON_DISPENSER;
            ES_PostAll(NewEvent);
          }
          else if (CurrentBeaconState == BEACON_B)
          {
            DB_printf("Blue Sequence Detected: ");
            ES_Event_t NewEvent;
            NewEvent.EventType = ES_BEACON_DISPENSER;
            ES_PostAll(NewEvent);
          }
        }
        LastBeaconState = CurrentBeaconState; // update the state for next time

        ES_Timer_InitTimer(BeaconDetectTimer, BEACON_TIMER_MS); // re-start timer
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
static uint32_t GetFreqFromPeriod(uint32_t periodTicks)
{
  if (periodTicks == 0)
    return 0;                    // avoid division by zero
  return 10000000UL / periodTicks; // 10,000,000 ticks/s / ticks = freq in Hz
}

void __ISR(_INPUT_CAPTURE_3_VECTOR, IPL7SOFT) ICHandler(void)
{
    uint16_t capture;
    capture = IC3BUF;
    while (IC3CONbits.ICBNE == 1)
    {
        IC3BUF; // clear the buffer
    }
    IC_ClearInterruptFlag(IC3);
    // IFS0CLR = _IFS0_IC3IF_MASK;               // clear interrupt flag

    if (IFS0bits.T3IF == 1 && capture < (PR3 / 2))
    {
        rollover++;
        IFS0CLR = _IFS0_T3IF_MASK;
    }
    TimeUnion currentTime;
    currentTime.byBytes[0] = capture;
    currentTime.byBytes[1] = rollover;
    if ((currentTime.time - lastCapture) > 0)
    {
        periodInTicks = (currentTime.time - lastCapture); // One tick is 0.1us = 1/10 MHz
    }

    lastCapture = currentTime.time; // update the last capture value
}

void __ISR(_TIMER_3_VECTOR, IPL6SOFT) Timer3ISR(void)
{
    if (IFS0bits.T3IF == 1)
    {
        rollover++;
        IFS0CLR = _IFS0_T3IF_MASK;
    }
    return;
}

static bool CheckSideSequence(uint8_t detectedBeacon, uint8_t *sideFound)
{
  // Store new beacon in circular buffer
  BeaconBuffer[BufferIndex] = detectedBeacon;
  BufferIndex = (BufferIndex + 1) % SEQ_LENGTH;

  if (BufferIndex == 0)
    BufferFilled = true;

  if (!BufferFilled)
    return false;

  // Reconstruct buffer in correct time order
  uint8_t Ordered[SEQ_LENGTH];
  for (uint8_t i = 0; i < SEQ_LENGTH; i++)
  {
      Ordered[i] = BeaconBuffer[(BufferIndex + i) % SEQ_LENGTH];
  }

  // Try all possible rotations
  for (uint8_t start = 0; start < SEQ_LENGTH; start++)
  {
      bool matchGreen = true;
      bool matchBlue  = true;

      for (uint8_t i = 0; i < SEQ_LENGTH; i++)
      {
          uint8_t seqIndex = (start + i) % SEQ_LENGTH;

          if (Ordered[i] != GreenSequence[seqIndex])
              matchGreen = false;

          if (Ordered[i] != BlueSequence[seqIndex])
              matchBlue = false;
      }

      if (matchGreen)
      {
          *sideFound = BEACON_G;
          return true;
      }

      if (matchBlue)
      {
          *sideFound = BEACON_B;
          return true;
      }
  }

  return false;
}
/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

