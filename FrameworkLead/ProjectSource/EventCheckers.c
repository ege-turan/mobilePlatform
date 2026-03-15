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


#define DEBUG_PRINT



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


/****************************************************************************
****************************************************************************/

#define LINE_PIVOT_L_ANSEL ANSELBbits.ANSB2
#define LINE_PIVOT_L_TRIS  TRISBbits.TRISB2
#define LINE_PIVOT_L_LAT   LATBbits.LATB2
#define LINE_PIVOT_L_PIN   PORTBbits.RB2
#define LINE_PIVOT_L_PIN_HI 1
#define LINE_PIVOT_L_PIN_LO 0
#define LINE_PIVOT_L_EVENT LINE_PIVOT_L_PIN_HI

#define LINE_PIVOT_R_ANSEL ANSELBbits.ANSB3
#define LINE_PIVOT_R_TRIS  TRISBbits.TRISB3
#define LINE_PIVOT_R_LAT   LATBbits.LATB3
#define LINE_PIVOT_R_PIN   PORTBbits.RB3
#define LINE_PIVOT_R_PIN_HI 1
#define LINE_PIVOT_R_PIN_LO 0
#define LINE_PIVOT_R_EVENT LINE_PIVOT_L_PIN_HI

bool Check4TapePivot(void)
{
    static bool Initialized = false;
    if (!Initialized)
    {
        // Disable analog, set input, optional pull-up
        LINE_PIVOT_L_ANSEL  = 0; // digital mode
        LINE_PIVOT_L_TRIS   = 1; // input
        LINE_PIVOT_L_LAT    = 0; // set low initially
        LINE_PIVOT_R_ANSEL  = 0; // digital mode
        LINE_PIVOT_R_TRIS   = 1; // input
        LINE_PIVOT_R_LAT    = 0; // set low initially
        Initialized = true;
    }

    static bool centered    = 0;
    static bool leftOnLine  = 0;
    static bool rightOnLine = 0;

    static uint8_t LastPinLState = 0;
    uint8_t CurrentPinLState;
    static uint8_t LastPinRState = 0;
    uint8_t CurrentPinRState;
    bool ReturnVal = false;

    CurrentPinLState = LINE_PIVOT_L_PIN;
    CurrentPinRState = LINE_PIVOT_R_PIN;
    // check for pin high AND different from last time
    // do the check for difference first so that you don't bother with a test
    // of a port/variable that is not going to matter, since it hasn't changed
    if (CurrentPinLState != LastPinLState) {
      if (CurrentPinLState == LINE_PIVOT_L_EVENT) // event detected, so post detected event
      {
          leftOnLine = true;
          ES_Event_t ThisEvent;
          ThisEvent.EventType = ES_LINE_PIVOT_L;
          ES_PostAll(ThisEvent);
  #ifdef DEBUG_PRINT
          DB_printf("[leader ] Tape Pivot L Detected!\r\n");
  #endif
          ReturnVal = true;
      }
      else {
          leftOnLine = false;
          centered = false;
  #ifdef DEBUG_PRINT
          DB_printf("[leader ] Tape Pivot L NOT Detected!\r\n");
  #endif
      }
    }
        
    if (CurrentPinRState != LastPinRState) {
      if (CurrentPinRState == LINE_PIVOT_R_EVENT) // event detected, so post detected event
      {
          rightOnLine = true;
          ES_Event_t ThisEvent;
          ThisEvent.EventType = ES_LINE_PIVOT_R;
          ES_PostAll(ThisEvent);
  #ifdef DEBUG_PRINT
          DB_printf("[leader ] Tape Pivot R Detected!\r\n");
  #endif
          ReturnVal = true;
      }
      else {
          rightOnLine = false;
          centered = false;
  #ifdef DEBUG_PRINT
          DB_printf("[leader ] Tape Pivot R NOT Detected!\r\n");
  #endif
      }
    }

    if (ReturnVal && (leftOnLine && rightOnLine)){
        ES_Event_t ThisEvent;
        ThisEvent.EventType = ES_CENTERED_PIVOT;
  #ifdef DEBUG_PRINT
          DB_printf("[leader ] Tape Pivot Centered!\r\n");
  #endif
        ES_PostAll(ThisEvent);
    }

    LastPinLState = CurrentPinLState; // update the state for next time
    LastPinRState = CurrentPinRState; // update the state for next time

    return ReturnVal;
}

/****************************************************************************
****************************************************************************/

// #define LINE_FRONT_L_ANSEL ANSELAbits.ANSA3
#define LINE_FRONT_L_TRIS  TRISAbits.TRISA3
#define LINE_FRONT_L_LAT   LATAbits.LATA3
#define LINE_FRONT_L_PIN   PORTAbits.RA3
#define LINE_FRONT_L_PIN_HI 1
#define LINE_FRONT_L_PIN_LO 0
#define LINE_FRONT_L_EVENT LINE_FRONT_L_PIN_HI

// #define LINE_FRONT_R_ANSEL ANSELAbits.ANSA2
#define LINE_FRONT_R_TRIS  TRISAbits.TRISA2
#define LINE_FRONT_R_LAT   LATAbits.LATA2
#define LINE_FRONT_R_PIN   PORTAbits.RA2
#define LINE_FRONT_R_PIN_HI 1
#define LINE_FRONT_R_PIN_LO 0
#define LINE_FRONT_R_EVENT LINE_FRONT_L_PIN_HI

bool Check4TapeFront(void)
{
    static bool Initialized = false;
    if (!Initialized)
    {
        // Disable analog, set input, optional pull-up
        // LINE_FRONT_L_ANSEL  = 0; // digital mode
        LINE_FRONT_L_TRIS   = 1; // input
        LINE_FRONT_L_LAT    = 0; // set low initially
        // LINE_FRONT_R_ANSEL  = 0; // digital mode
        LINE_FRONT_R_TRIS   = 1; // input
        LINE_FRONT_R_LAT    = 0; // set low initially
        Initialized = true;
    }

    static bool centered    = 0;
    static bool leftOnLine  = 0;
    static bool rightOnLine = 0;

    static uint8_t LastPinLState = 0;
    uint8_t CurrentPinLState;
    static uint8_t LastPinRState = 0;
    uint8_t CurrentPinRState;
    bool ReturnVal = false;

    CurrentPinLState = LINE_FRONT_L_PIN;
    CurrentPinRState = LINE_FRONT_R_PIN;
    // check for pin high AND different from last time
    // do the check for difference first so that you don't bother with a test
    // of a port/variable that is not going to matter, since it hasn't changed
    if (CurrentPinLState != LastPinLState) {
      if (CurrentPinLState == LINE_FRONT_L_EVENT) // event detected, so post detected event
      {
          leftOnLine = true;
          ES_Event_t ThisEvent;
          ThisEvent.EventType = ES_LINE_PIVOT_L;
          ES_PostAll(ThisEvent);
  #ifdef DEBUG_PRINT
          DB_printf("[leader ] Tape Front L Detected!\r\n");
  #endif
          ReturnVal = true;
      }
      else {
          leftOnLine = false;
          centered = false;
  #ifdef DEBUG_PRINT
          DB_printf("[leader ] Tape Front L NOT Detected!\r\n");
  #endif
      }
    }
        
    if (CurrentPinRState != LastPinRState) {
      if (CurrentPinRState == LINE_FRONT_R_EVENT) // event detected, so post detected event
      {
          rightOnLine = true;
          ES_Event_t ThisEvent;
          ThisEvent.EventType = ES_LINE_PIVOT_R;
          ES_PostAll(ThisEvent);
  #ifdef DEBUG_PRINT
          DB_printf("[leader ] Tape Front R Detected!\r\n");
  #endif
          ReturnVal = true;
      }
      else {
          rightOnLine = false;
          centered = false;
  #ifdef DEBUG_PRINT
          DB_printf("[leader ] Tape Front R NOT Detected!\r\n");
  #endif
      }
    }

    if (ReturnVal && (leftOnLine && rightOnLine)){
        ES_Event_t ThisEvent;
        ThisEvent.EventType = ES_CENTERED;
        ES_PostAll(ThisEvent);
  #ifdef DEBUG_PRINT
          DB_printf("[leader ] Tape Front Centered!\r\n");
  #endif
    }

    LastPinLState = CurrentPinLState; // update the state for next time
    LastPinRState = CurrentPinRState; // update the state for next time

    return ReturnVal;
}


/****************************************************************************
****************************************************************************/


#define LIMIT_TRIS TRISAbits.TRISA4
#define LIMIT_LAT LATAbits.LATA4
#define LIMIT_IN PORTAbits.RA4

/****************************************************************************
 Function
     Check4Limit

 Parameters
     None

 Returns
     bool: true if an event was posted, false otherwise

 Description
     Checks the limmit switch for changes. Posts ES_LIMIT
     events to all when a change is detected.

 Author
     Ege Turan
****************************************************************************/
bool Check4Limit(void)
{
    bool ReturnVal = false;

    static bool Initialized = false; 
    if (!Initialized)
    {
        // Disable analog, set input, optional pull-up
//        LIMIT_ANSEL  = 0; // digital mode // no analog for A4
        LIMIT_TRIS   = 1; // input
        LIMIT_LAT    = 0; // set low initially
        Initialized = true;
    }

    static uint8_t LastLimitState = 1;                   // assume button is not initially pressed
    uint8_t CurrentLimitState = LIMIT_IN;

    if ((CurrentLimitState != LastLimitState) && (CurrentLimitState == 0)) {    // when pressed down, sunk low!
#ifdef DEBUG_PRINT
        DB_printf("Limit (%d) -> TOGGLE posted\n", CurrentLimitState);
#endif

        ES_Event_t ThisEvent;
        ReturnVal = true;
        ThisEvent.EventType  = ES_LIMIT_SWITCH;
        ThisEvent.EventParam = CurrentLimitState;        // no additional parameter needed, include CurrentLimitState for debugging
        ES_PostAll(ThisEvent);                           // post event to all
    }

    LastLimitState = CurrentLimitState; // update last state
    return ReturnVal;
}
