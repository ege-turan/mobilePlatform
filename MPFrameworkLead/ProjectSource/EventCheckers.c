/****************************************************************************
 Module
   EventCheckers.c

 Revision
   1.0.1

 Description
   This file implements event checkers for our application along with the event
   checkers used in the basic framework test harness.

 Notes
   Note the use of static variables in sample event checker to detect
   ONLY transitions.

 History
 When           Who     What/Why
 -------------- ---     --------
 25             ege     updated and cleaned
 08/06/13 13:36 jec     initial version
****************************************************************************/

// this will pull in the symbolic definitions for events, which we will want
// to post in response to detecting events
#include "ES_Configure.h"
// This gets us the prototype for ES_PostAll
#include "ES_Framework.h"
// this will get us the structure definition for events, which we will need
// in order to post events in response to detecting events
#include "ES_Events.h"
// if you want to use distribution lists then you need those function
// definitions too.
#include "ES_PostList.h"
// This include will pull in all of the headers from the service modules
// providing the prototypes for all of the post functions
#include "ES_ServiceHeaders.h"
// this test harness for the framework references the serial routines that
// are defined in ES_Port.c
#include "ES_Port.h"
// include our own prototypes to insure consistency between header &
// actual functionsdefinition
#include "EventCheckers.h"

#include "dbprintf.h"

// This is the event checking function sample. It is not intended to be
// included in the module. It is only here as a sample to guide you in writing
// your own event checkers
#if 0
/****************************************************************************
 Function
   Check4Lock
 Parameters
   None
 Returns
   bool: true if a new event was detected
 Description
   Sample event checker grabbed from the simple lock state machine example
 Notes
   will not compile, sample only
 Author
   J. Edward Carryer, 08/06/13, 13:48
****************************************************************************/
bool Check4Lock(void)
{
  static uint8_t  LastPinState = 0;
  uint8_t         CurrentPinState;
  bool            ReturnVal = false;

  CurrentPinState = LOCK_PIN;
  // check for pin high AND different from last time
  // do the check for difference first so that you don't bother with a test
  // of a port/variable that is not going to matter, since it hasn't changed
  if ((CurrentPinState != LastPinState) &&
      (CurrentPinState == LOCK_PIN_HI)) // event detected, so post detected event
  {
    ES_Event ThisEvent;
    ThisEvent.EventType   = ES_LOCK;
    ThisEvent.EventParam  = 1;
    // this could be any of the service post functions, ES_PostListx or
    // ES_PostAll functions
    ES_PostAll(ThisEvent);
    ReturnVal = true;
  }
  LastPinState = CurrentPinState; // update the state for next time

  return ReturnVal;
}

#endif

/****************************************************************************
 Function
   Check4Keystroke
 Parameters
   None
 Returns
   bool: true if a new key was detected & posted
 Description
   checks to see if a new key from the keyboard is detected and, if so,
   retrieves the key and posts an ES_NewKey event to TestHarnessService0
 Notes
   The functions that actually check the serial hardware for characters
   and retrieve them are assumed to be in ES_Port.c
   Since we always retrieve the keystroke when we detect it, thus clearing the
   hardware flag that indicates that a new key is ready this event checker
   will only generate events on the arrival of new characters, even though we
   do not internally keep track of the last keystroke that we retrieved.
 Author
   Ege Turan
****************************************************************************/
bool Check4Keystroke(void)
{
  if (IsNewKeyReady())   // new key waiting?
  {
    ES_Event_t ThisEvent;
    ThisEvent.EventType   = ES_NEW_KEY;
    ThisEvent.EventParam  = GetNewKey();
    ES_PostAll(ThisEvent);
    return true;
  }
  return false;
}


/* EVERYTHING BELOW HERE WAS FROM LAB 8*/
#include "PIC32_IC_Lib.h"
#include <sys/attribs.h>
#include <xc.h>

static uint32_t GetFreqFromPeriod(uint32_t periodTicks);


// IC is in RB9 (0100 = RPB9, IC2)

// static volatile uint32_t currentTime = 0;
static volatile uint16_t rollover      = 0;
static volatile uint32_t periodInTicks = 0;
static volatile uint32_t lastCapture   = 0;

typedef union
{
    uint32_t time;
    uint16_t byBytes[2];
} TimeUnion;

#define BEACON_FREQ 1427 // in Hz
#define BEACON_TOL 25     // in Hz
// #define DEBUG_PRINT

bool Check4Beacon(void)
{
    static bool Initialized = false;
    if (!Initialized)
    {
        // Initialize Timer 3 and Input Capture 2 for beacon detection
        IC_Setup_ConfigureTimer(_Timer3_, 0x10000, IC_PS_2);
        IC_Setup_EnableChannel(IC2, IC_RisingEdge);

        // IC_Setup_MapChannelToPin(IC2, IC_RPB9); // Check w/ Ege, I think this doesn't work as intended
        IC2R = 0b0100; // Map IC2 to RPB9
        __builtin_enable_interrupts();

#ifdef DEBUG_PRINT
        DB_printf("Beacon IC and Timer initialized\r\n");
#endif
        Initialized = true;
    }

    static uint32_t CallCounter = 0;
    CallCounter++;
    if (CallCounter < 10000)
    {
        return false;
    }
    CallCounter = 0;

    static uint8_t LastBeaconState = 0;
    uint8_t CurrentBeaconState;
    uint32_t CurrentBeaconFreq;
    bool ReturnVal = false;

    uint32_t CurrentPeriodinTicks = periodInTicks;
    CurrentBeaconFreq             = GetFreqFromPeriod(CurrentPeriodinTicks);

    if ((CurrentBeaconFreq >= (BEACON_FREQ - BEACON_TOL)) &&
        (CurrentBeaconFreq <= (BEACON_FREQ + BEACON_TOL)))
    {
        CurrentBeaconState = 1;
    }
    else
        CurrentBeaconState = 0;

    // event detected, so post detected event
    if ((CurrentBeaconState == 1)) // && (CurrentBeaconState != LastBeaconState))
    {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = ES_BEACON_DETECTED;
        ES_PostAll(ThisEvent);
#ifdef DEBUG_PRINT
        DB_printf("Beacon Detected! Freq: %u Hz\r\n", (unsigned int)CurrentBeaconFreq);
#endif
        ReturnVal = true;
    } else
    {
        #ifdef DEBUG_PRINT
        DB_printf("No Beacon. Freq: %u Hz\r\n", (unsigned int)CurrentBeaconFreq);
        #endif
    }
    LastBeaconState = CurrentBeaconState; // update the state for next time
    return ReturnVal;
}

static uint32_t GetFreqFromPeriod(uint32_t periodTicks)
{
    if (periodTicks == 0)
        return 0;                    // avoid division by zero
    return 10000000UL / periodTicks; // 10,000,000 ticks/s / ticks = freq in Hz
}

void __ISR(_INPUT_CAPTURE_2_VECTOR, IPL7SOFT) ICHandler(void)
{
    uint16_t capture;
    capture = IC2BUF;
    while (IC2CONbits.ICBNE == 1)
    {
        IC2BUF; // clear the buffer
    }
    IC_ClearInterruptFlag(IC2);
    // IFS0CLR = _IFS0_IC2IF_MASK;               // clear interrupt flag

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

#define TAPE_ANSEL ANSELAbits.ANSA0
#define TAPE_TRIS TRISAbits.TRISA0
#define TAPE_LAT LATAbits.LATA0
#define TAPE_PIN PORTAbits.RA0
#define TAPE_PIN_HI 1
#define TAPE_PIN_LO 0
#define TAPE_EVENT TAPE_PIN_HI

bool Check4Tape(void)
{
    static bool Initialized = false;
    if (!Initialized)
    {
        // Disable analog, set input, optional pull-up
        TAPE_ANSEL  = 0; // digital mode
        TAPE_TRIS   = 1; // input
        TAPE_LAT    = 0; // set low initially
        Initialized = true;
    }

    static uint8_t LastPinState = 0;
    uint8_t CurrentPinState;
    bool ReturnVal = false;

    CurrentPinState = TAPE_PIN;
    // check for pin high AND different from last time
    // do the check for difference first so that you don't bother with a test
    // of a port/variable that is not going to matter, since it hasn't changed
    if ((CurrentPinState != LastPinState) &&
        (CurrentPinState == TAPE_EVENT)) // event detected, so post detected event
    {
        ES_Event_t ThisEvent;
        ThisEvent.EventType = ES_TAPE_DETECTED;
        // this could be any of the service post functions, ES_PostListx or
        // ES_PostAll functions
        ES_PostAll(ThisEvent);
#ifdef DEBUG_PRINT
        DB_printf("Tape Detected!\r\n");
#endif
        ReturnVal = true;
    }
    LastPinState = CurrentPinState; // update the state for next time

    return ReturnVal;
}
