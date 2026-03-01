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
#include "OperatorFSM.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static OperatorState_t CurrentState;

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
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
    case OperatorInitPState:        // If current state is initial Psedudo State
    {
      if (ThisEvent.EventType == ES_INIT)    // only respond to ES_Init
      {
        // this is where you would put any actions associated with the
        // transition from the initial pseudo-state into the actual
        // initial state

        // now put the machine into the actual initial state
        // CurrentState = UnlockWaiting;
      }
    }
    break;

    // case UnlockWaiting:        // If current state is state one
    {
      switch (ThisEvent.EventType)
      {
        //case ES_LOCK:  //If event is event one

        {   // Execute action function for state one : event one
          // CurrentState = Locked;  //Decide what the next state will be
        }
        break;

        // repeat cases as required for relevant events
        default:
          ;
      }  // end switch on CurrentEvent
    }
    break;
    // repeat state pattern as required for other states
    default:
      ;
  }                                   // end switch on Current State
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
    ANSELBbits.ANSB8 = 0;   // DIN_CARGO_IN
    ANSELBbits.ANSB9 = 0;   // DIN_CARGO_OUT

    // ---- Set inputs ----
    TRISBbits.TRISB8 = 1;   // DIN_CARGO_IN
    TRISBbits.TRISB9 = 1;   // DIN_CARGO_OUT

    // ---- Map pins to external interrupts ----
    // PRB8 -> INT3
    INT3Rbits.INT3R = 0b0010;   // check your datasheet: RB8 -> INT3
    // PRB9 -> INT1
    INT1Rbits.INT1R = 0b0011;   // check your datasheet: RB9 -> INT1

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

