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

#include "PIC32_SPI_HAL.h"
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

bool Check4SPI(void)
{
  if (SPIOperate_HasSS1_Risen())
  {
    uint8_t newMessage = (uint8_t)SPIOperate_ReadData(SPI_SPI1);

    // Post New Message to SPIFollowService to interpret it and write new data
    ES_Event_t ThisEvent;
    ThisEvent.EventType  = ES_SPI_COMPLETE;
    ThisEvent.EventParam = newMessage;
    ES_PostAll(ThisEvent);

    // DB_printf("SPI msg from Leader:     0x%x\r\n", (unsigned int)ThisEvent.EventParam);

    return true;
  }
  return false;
}

/****************************************************************************
 Start button event checker
****************************************************************************/

#define BUTTON_IN     PORTAbits.RA3   // Button input line on RA3
#define BUTTON_TRIS TRISAbits.TRISA3
#define BUTTON_LAT LATAbits.LATA3
//#define BUTTON_ANSEL ANSELAbits.ANSA3

#define DEBUG_BUTTON       // enable for debug prints

#ifdef DEBUG_BUTTON
    #include "dbprintf.h"
#endif


/****************************************************************************
 Function
     Check4StartButton

 Parameters
     None

 Returns
     bool: true if an event was posted, false otherwise

 Description
     Checks the button state for changes. Posts ES_BUTTON_UP or ES_BUTTON_DOWN
     events to the Operator FSM when a change is detected.

 Author
     Ege Turan
****************************************************************************/
bool Check4StartButton(void)
{
    bool ReturnVal = false;

    static bool Initialized = false; 
    if (!Initialized)
    {
        // Disable analog, set input, optional pull-up
//        BUTTON_ANSEL  = 0; // digital mode // no analog for A3
        BUTTON_TRIS   = 1; // input
        BUTTON_LAT    = 0; // set low initially
        Initialized = true;
    }

    static uint8_t LastButtonState = 1;                   // assume button is not initially pressed
    uint8_t CurrentButtonState = BUTTON_IN;

    if ((CurrentButtonState != LastButtonState) && (CurrentButtonState == 0)) {
#ifdef DEBUG_BUTTON
        DB_printf("Button (RA3=%d) -> TOGGLE posted\n", CurrentButtonState);
#endif

        ES_Event_t ThisEvent;
        ReturnVal = true;
        ThisEvent.EventType  = ES_START_DOWN;
        ThisEvent.EventParam = CurrentButtonState;        // no additional parameter needed, include CurrentButtonState for debugging
        PostOperatorFSM(ThisEvent);                       // post event to Laser FSM
        PostDropoffLoweringArmFSM(ThisEvent);
    }

    LastButtonState = CurrentButtonState; // update last state
    return ReturnVal;
}