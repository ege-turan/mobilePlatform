/****************************************************************************
 Module
   OperatorFSM.c

 Revision
   1.0.1

 Description
   This file implements OperatorService as a flat state machines under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 25             ege      updated and cleaned
 01/15/12 11:12 jec      revisions for Gen2 framework
 11/07/11 11:26 jec      made the queue static
 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"

#include <sys/attribs.h>

#include "OperatorFSM.h"
#include "SPIFollowService.h"
#include "IntakeService.h"
#include "DropoffLoweringArmFSM.h"

/*----------------------------- Module Defines ----------------------------*/

// Pins
#define DIN_GAMEMODE PORTBbits.RB4
#define DIN_GAMEMODE_TRIS TRISBbits.TRISB4

// Timers
#define GAME_TIME_MS    21800
#define INTAKE_PACE_MS  5000
#define DROPOFF_PACE_MS 7000

// Measured
#define CAPACITY 30
#define GAME_COUNT 10

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
static void InitOperatorInterrupts(void);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match that of enum in header file
static OperatorState_t CurrentState;

static uint8_t carrying;
static uint8_t gameCounter;
static uint8_t GameMode;
static uint8_t Strategy;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitOperatorFSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
     Ege Turan
****************************************************************************/
bool InitOperatorFSM(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  // put us into the Initial PseudoState
  CurrentState = OperatorInitPState;
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
     PostOperatorFSM

 Parameters
     EF_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     Ege Turan
****************************************************************************/
bool PostOperatorFSM(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunOperatorFSM

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   Ege Turan
****************************************************************************/
ES_Event_t RunOperatorFSM(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT;

  /********************* GLOBAL GAME TIMER *********************/
  if (ThisEvent.EventType == ES_TIMEOUT &&
      ThisEvent.EventParam == GAME_TIMER)
  {
      gameCounter++;

      if (gameCounter >= GAME_COUNT)
      {
          // Stop all pacing timers
          ES_Timer_StopTimer(INTAKE_PACE_TIMER);
          ES_Timer_StopTimer(DROPOFF_PACE_TIMER);
          ES_Timer_StopTimer(GAME_TIMER);

          // Send SPI END command
          ES_Event_t spiEvent;
          spiEvent.EventType = ES_NEW_SPI_CMD_SEND;
          spiEvent.EventParam = CMD_SPI_END;
          PostSPIFollowService(spiEvent);

          // create an event ES_FEEDER_STOP and post it to StepperService
          ES_Event_t feederStopEvent;
          feederStopEvent.EventType = ES_FEEDER_STOP;
          PostStepperService(feederStopEvent);

          // todo: event to reset field side indicator (ES_RESET)

          // Reset system state
          carrying = 0;

          CurrentState = Standby;
          return ReturnEvent;
      }
      else
      {
          // Restart game timer for next interval
          ES_Timer_InitTimer(GAME_TIMER, GAME_TIME_MS);
      }
  }

  switch (CurrentState)
  {

    /********************* INIT PSEUDOSTATE *********************/
    case OperatorInitPState:
    {
      if (ThisEvent.EventType == ES_INIT)
      {
        // Initialize variables
        carrying = 0;
        GameMode = 0;
        Strategy = 0;
        gameCounter = 0;

        // Set RB4 as digital input for GAMEMODE
        DIN_GAMEMODE_TRIS = 1;

        // (Add outputs)

        // Initialize cargo interrupts
        InitOperatorInterrupts();

        CurrentState = Standby;
      }
    }
    break;


    /********************* STANDBY *********************/
    case Standby:
    {
      switch (ThisEvent.EventType)
      {
        case ES_START_DOWN:
        {
          carrying = 0;

          // Read Game Mode from RB4
          GameMode = DIN_GAMEMODE;

          // Save Strategy if needed (placeholder)
          Strategy = 0;

          // Start Game Timer
          gameCounter = 0;
          ES_Timer_InitTimer(GAME_TIMER, GAME_TIME_MS);

          // Send SPI Start Command
          ES_Event_t spiEvent;
          spiEvent.EventType = ES_NEW_SPI_CMD_SEND;

          if (GameMode == 0)
            spiEvent.EventParam = CMD_SPI_START;
          else
            spiEvent.EventParam = CMD_SPI_START_DEATHMATCH;

          PostSPIFollowService(spiEvent);

          CurrentState = WaitingForNavigation;
        }
        break;
      }
    }
    break;


    /********************* WAITING FOR NAVIGATION *********************/
    case WaitingForNavigation:
    {
      switch (ThisEvent.EventType)
      {
        case ES_NEW_SPI_CMD_RECEIVED:
        {
          if (ThisEvent.EventParam == CMD_SPI_INTAKE_ON)
          {
              ES_Timer_InitTimer(INTAKE_PACE_TIMER, INTAKE_PACE_MS);

              CurrentState = Intaking;
          }
          else if (ThisEvent.EventParam == CMD_SPI_DROPOFF_REACHED)
          {
              ES_Timer_InitTimer(DROPOFF_PACE_TIMER, DROPOFF_PACE_MS);

              CurrentState = LoweringDropoff;
          }
          else if (ThisEvent.EventParam == CMD_SPI_RELEASE_DROPOFF)
          {
              // Leader requested dropoff to be raised
              CurrentState = RaisingDropoff;
          }
          break;
        }
        break;
      }
    }
    break;


    /********************* INTAKING *********************/
    case Intaking:
    {
      switch (ThisEvent.EventType)
      {
        case ES_CARGO_IN:
        {
          carrying++;

          if (carrying < CAPACITY)
          {
            ES_Timer_InitTimer(INTAKE_PACE_TIMER, INTAKE_PACE_MS);  // sets & starts
          }
          else
          {
            // Stop intake pacing timer
            ES_Timer_StopTimer(INTAKE_PACE_TIMER);                  // stop completely

            // Notify SPI and IntakeService
            ES_Event_t loadedEvent;
            loadedEvent.EventType  = ES_NEW_SPI_CMD_SEND;
            loadedEvent.EventParam = CMD_SPI_LOADED;  // or payload if needed

            PostSPIFollowService(loadedEvent);  // send to leader
            PostIntakeService(loadedEvent);     // stop intake locally

            CurrentState = WaitingForNavigation;
          }
        }
        break;

        case ES_TIMEOUT:
        {
          if (ThisEvent.EventParam == INTAKE_PACE_TIMER)
          {
            ES_Event_t incompleteEvent;
            incompleteEvent.EventType = ES_NEW_SPI_CMD_SEND;
            incompleteEvent.EventParam = CMD_SPI_INTAKE_INCOMPLETE;
            PostSPIFollowService(incompleteEvent);
            CurrentState = WaitingForNavigation;
          }
        }
        break;
      }
    }
    break;

    /********************* LOWERING DROPOFF *********************/
    case LoweringDropoff:
    {
        switch (ThisEvent.EventType)
        {
            case ES_DROPOFF_LOWERED:
            {
                if (carrying == 0)
                {
                    ES_Timer_StopTimer(DROPOFF_PACE_TIMER);   // double check that we stopped the timer

                    // Notify SPI that dropoff is unloaded
                    ES_Event_t unloadEvent;
                    unloadEvent.EventType  = ES_NEW_SPI_CMD_SEND;
                    unloadEvent.EventParam = CMD_SPI_UNLOADED;
                    PostSPIFollowService(unloadEvent);

                    CurrentState = WaitingForNavigation;
                }
                else  // carrying > 0
                {
                    // Stop pacing timer and move to DroppingOff state
                    ES_Timer_StopTimer(DROPOFF_PACE_TIMER);
                    CurrentState = DroppingOff;

                    // create an event ES_FEEDER_START and post it to StepperService
                    ES_Event_t feederStartEvent;
                    feederStartEvent.EventType = ES_FEEDER_START;
                    PostStepperService(feederStartEvent);
                }
            }
            break;

            case ES_NEW_SPI_CMD_RECEIVED:
            {
                if (ThisEvent.EventParam == CMD_SPI_RELEASE_DROPOFF)
                {
                    // Stop pacing timer and move to RaisingDropoff state
                    ES_Timer_StopTimer(DROPOFF_PACE_TIMER);
                    CurrentState = RaisingDropoff;
                }
            }
            break;

        }
    }
    break;

    /********************* DROPPING OFF *********************/
    case DroppingOff:
    {
        switch (ThisEvent.EventType)
        {
            case ES_CARGO_OUT:
            {
                if (carrying > 0)
                {
                    carrying--;

                    if (carrying == 0)
                    {
                        ES_Timer_StopTimer(DROPOFF_PACE_TIMER);   // double check that we stopped the timer

                        // Notify SPI that all cargo has been unloaded
                        ES_Event_t unloadEvent;
                        unloadEvent.EventType  = ES_NEW_SPI_CMD_SEND;
                        unloadEvent.EventParam = CMD_SPI_UNLOADED;
                        PostSPIFollowService(unloadEvent);

                        // create an event ES_FEEDER_STOP and post it to StepperService
                        ES_Event_t feederStopEvent;
                        feederStopEvent.EventType = ES_FEEDER_STOP;
                        PostStepperService(feederStopEvent);

                        CurrentState = WaitingForNavigation;
                    }
                }
            }
            break;
        }
    }
    break;

    /********************* RAISING DROPOFF *********************/
    case RaisingDropoff:
    {
        switch (ThisEvent.EventType)
        {
            case ES_DROPOFF_RELEASED:
            {
                // Notify SPI that dropoff is released
                ES_Event_t releaseEvent;
                releaseEvent.EventType  = ES_NEW_SPI_CMD_SEND;
                releaseEvent.EventParam = CMD_SPI_RELEASED;
                PostSPIFollowService(releaseEvent);

                CurrentState = WaitingForNavigation;
            }
            break;
        }
    }
    break;

    /********************* DEFAULT *********************/
    default:
      break;
  }

  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryOperatorSM

 Parameters
     None

 Returns
     OperatorState_t The current state of the Operator state machine

 Description
     returns the current state of the Operator state machine
 Notes

 Author
     Ege Turan
****************************************************************************/
OperatorState_t QueryOperatorFSM(void)
{
  return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/
static void InitOperatorInterrupts(void)
{
    __builtin_disable_interrupts();

    // ---- Disable analog on cargo input pins ----
    // No analog selection for these pins
//    ANSELBbits.ANSB8 = 0;   // DIN_CARGO_IN
//    ANSELBbits.ANSB9 = 0;   // DIN_CARGO_OUT

    // ---- Set inputs ----
    TRISBbits.TRISB8 = 1;   // DIN_CARGO_IN
    TRISBbits.TRISB9 = 1;   // DIN_CARGO_OUT

    // ---- Map pins to external interrupts ----
    // PRB8 -> INT3
    INT3Rbits.INT3R = 0b0100;   // check datasheet: RB8 -> INT3
    // PRB9 -> INT1
    INT1Rbits.INT1R = 0b0100;   // check datasheet: RB9 -> INT1

    // ---- Configure interrupts ----
    // INT1 -> Cargo OUT (falling edge)
    INTCONbits.INT1EP = 1;   // falling edge
    IFS0bits.INT1IF = 0;     // clear flag
    IEC0bits.INT1IE = 1;     // enable interrupt
    IPC1bits.INT1IP = 5;     // priority 5

    // INT3 -> Cargo IN (rising edge)
    INTCONbits.INT3EP = 0;   // rising edge
    IFS0bits.INT3IF = 0;     // clear flag
    IEC0bits.INT3IE = 1;     // enable interrupt
    IPC3bits.INT3IP = 5;     // priority 5

    __builtin_enable_interrupts();
}

 /***************************************************************************
 ISR (Interrupt Service Routine)
 ***************************************************************************/
// use INT1 at #define DIN_CARGO_OUT at PRB9 at falling edge to post an ES_CARGO_OUT event to OperatorFSM 
// and INT3 at #define DIN_CARGO_IN at PRB8 at rising edge to post ES_CARGO_IN to OperatorFSM 
void __ISR(_EXTERNAL_1_VECTOR, IPL5SOFT) _CargoOut_ISR(void)
{
    IFS0bits.INT1IF = 0;   // clear interrupt flag
    ES_Event_t CargoEvent;
    CargoEvent.EventType = ES_CARGO_OUT;
    PostOperatorFSM(CargoEvent);
}

void __ISR(_EXTERNAL_3_VECTOR, IPL5SOFT) _CargoIn_ISR(void)
{
    IFS0bits.INT3IF = 0;   // clear interrupt flag
    ES_Event_t CargoEvent;
    CargoEvent.EventType = ES_CARGO_IN;
    PostOperatorFSM(CargoEvent);
}

