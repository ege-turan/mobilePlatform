/****************************************************************************
 Module
   HSMTemplate.c

 Revision
   2.0.1

 Description
   This is a template file for implementing state machines.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 25             ege      updated and cleaned
 02/27/17 09:48 jec      another correction to re-assign both CurrentEvent
                         and ReturnEvent to the result of the During function
                         this eliminates the need for the prior fix and allows
                         the during function to-remap an event that will be
                         processed at a higher level.
 02/20/17 10:14 jec      correction to Run function to correctly assign 
                         ReturnEvent in the situation where a lower level
                         machine consumed an event.
 02/03/16 12:38 jec      updated comments to reflect changes made in '14 & '15
                         converted unsigned char to bool where appropriate
                         spelling changes on true (was True) to match standard
                         removed local var used for debugger visibility in 'C32
                         commented out references to Start & RunLowerLevelSM so
                         that this can compile. 
 02/07/13 21:00 jec      corrections to return variable (should have been
                         ReturnEvent, not CurrentEvent) and several EV_xxx
                         event names that were left over from the old version
 02/08/12 09:56 jec      revisions for the Events and Services Framework Gen2
 02/13/10 14:29 jec      revised Start and run to add new kind of entry function
                         to make implementing history entry cleaner
 02/13/10 12:29 jec      added NewEvent local variable to During function and
                         comments about using either it or Event as the return
 02/11/10 15:54 jec      more revised comments, removing last comment in during
                         function that belongs in the run function
 02/09/10 17:21 jec      updated comments about internal transitions on During funtion
 02/18/09 10:14 jec      removed redundant call to RunLowerlevelSM in EV_Entry
                         processing in During function
 02/20/07 21:37 jec      converted to use enumerated type for events & states
 02/13/05 19:38 jec      added support for self-transitions, reworked
                         to eliminate repeated transition code
 02/11/05 16:54 jec      converted to implment hierarchy explicitly
 02/25/03 10:32 jec      converted to take a passed event parameter
 02/18/99 10:19 jec      built template from MasterMachine.c
 02/14/99 10:34 jec      Began Coding
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"

/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "HSMDriverFromTo.h"

#include "TopHSMPathPlanner.h"
#include "DCMotorService.h"
#include "SPILeadService.h"
#include "BeaconService.h"
#include "PlansAndSteps.h"

#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

#define ENTRY_STATE STANDBY_STEP

#define VERBOSE_DRIVER

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
static ES_Event_t DuringStandbyStep( ES_Event_t Event);
static ES_Event_t DuringRunningStep( ES_Event_t Event);

static void DoStepActions(void);


/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static TemplateState_t CurrentState;
static uint16_t StepCounter; 
static const Plan_t *ActivePlan = NULL;
static PlanIndex_t CurrentPlanIndex;
static DMPlanIndex_t CurrentDMPlanIndex;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
    RunTemplateSM

 Parameters
   ES_Event_t: the event to process

 Returns
   ES_Event_t: an event to return

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 2/11/05, 10:45AM
****************************************************************************/
ES_Event_t RunDriverFromToSM( ES_Event_t CurrentEvent )
{
   bool MakeTransition = false;/* are we making a state transition? */
   TemplateState_t NextState = CurrentState;
   ES_Event_t EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event_t ReturnEvent = CurrentEvent; // assume we are not consuming event

   switch ( CurrentState )
   {
       case STANDBY_STEP :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lower level state machines to re-map
         // or consume the event
         ReturnEvent = CurrentEvent = DuringStandbyStep(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case ES_START_PLAN : //If event is event one
                #ifdef VERBOSE_DRIVER
                DB_printf("[Driver ] Received:     ES_START_PLAN");
                #endif
                  // Execute action function for state one : event one
                  CurrentPlanIndex = (PlanIndex_t)CurrentEvent.EventParam;
                  ActivePlan = &Plans[CurrentPlanIndex];

                  NextState = RUNNING_STEP;//Decide what the next state will be
                  StepCounter = 0; // initialize step counter
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = true; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
              case ES_START_DM_PLAN :
                #ifdef VERBOSE_DRIVER
                DB_printf("[Driver ] Received:     ES_START_DM_PLAN");
                #endif
                  // Execute action function for state one : event one
                  CurrentDMPlanIndex = (DMPlanIndex_t)CurrentEvent.EventParam;
                  ActivePlan = &DM_Plans[CurrentDMPlanIndex];

                  NextState = RUNNING_STEP;//Decide what the next state will be
                  StepCounter = 0; // initialize step counter
                  MakeTransition = true; //mark that we are taking a transition
                  EntryEventKind.EventType = ES_ENTRY;
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
                
                 default:
                  break;
                // repeat cases as required for relevant events
            }
         }
       break;
       case RUNNING_STEP :
         ReturnEvent = CurrentEvent = DuringRunningStep(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case ES_STEP_DONE : //If event is event one
                  // Execute action function for state one : event one
                  NextState = RUNNING_STEP;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = false; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  break;
               case ES_PLAN_DONE :
                    #ifdef VERBOSE_DRIVER
                    DB_printf("[Driver] Received Plan %u Done\r\n", (unsigned int)CurrentPlanIndex);
                    #endif
                  NextState = STANDBY_STEP;
                  MakeTransition = true;
                  EntryEventKind.EventType = ES_ENTRY;
                  break;
               default:
                  break;
                // repeat cases as required for relevant events
            }
         }
      // repeat state pattern as required for other states
    }
    //   If we are making a state transition
    if (MakeTransition == true)
    {
       //   Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunDriverFromToSM(CurrentEvent);

       CurrentState = NextState; //Modify state variable

       //   Execute entry function for new state
       // this defaults to ES_ENTRY
       RunDriverFromToSM(EntryEventKind);
     }
     return(ReturnEvent);
}
/****************************************************************************
 Function
     StartDriverFromToSM

 Parameters
     None

 Returns
     None

 Description
     Does any required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 2/18/99, 10:38AM
****************************************************************************/
void StartDriverFromToSM ( ES_Event_t CurrentEvent )
{
   // to implement entry to a history state or directly to a substate
   // you can modify the initialization of the CurrentState variable
   // otherwise just start in the entry state every time the state machine
   // is started
   if ( ES_ENTRY_HISTORY != CurrentEvent.EventType )
   {
        CurrentState = ENTRY_STATE;
   }
   // announce
    DB_printf("\rStarting HSMDriverFromTo: ");
    DB_printf("compiled at %s on %s", __TIME__, __DATE__);
    DB_printf("\n\r");
   // call the entry function (if any) for the ENTRY_STATE
   RunDriverFromToSM(CurrentEvent);
   StepCounter = 0; // initialize variable
}

/****************************************************************************
 Function
     QueryDriverFromToSM

 Parameters
     None

 Returns
     TemplateState_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
     J. Edward Carryer, 2/11/05, 10:38AM
****************************************************************************/
TemplateState_t QueryDriverFromToSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event_t DuringStandbyStep( ES_Event_t Event)
{
    ES_Event_t ReturnEvent = Event; // assume no re-mapping or consumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        ES_Event_t ThisEvent;
        ThisEvent.EventType = ES_MOTORS_OFF;
        PostDCMotorService(ThisEvent);
        
        // after that start any lower level machines that run in this state
        //StartLowerLevelSM( Event );
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
        //RunLowerLevelSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
      
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        // ReturnEvent = RunLowerLevelSM(Event);
      
        // repeat for any concurrent lower level machines
      
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}


static ES_Event_t DuringRunningStep( ES_Event_t Event)
{
    ES_Event_t ReturnEvent = Event; // assume no re-mapping or consumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        /* Execute the first step */
        DoStepActions();

        // after that start any lower level machines that run in this state
        //StartLowerLevelSM( Event );
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
        //RunLowerLevelSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
      
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        // ReturnEvent = RunLowerLevelSM(Event);
        if (ActivePlan == NULL)
        {
            return ReturnEvent;
        }
        PlanStep_t *CurrentStep = &ActivePlan->Steps[StepCounter];
        if ((Event.EventType == CurrentStep->StoppingCondition.EventType) &&
            (Event.EventParam == CurrentStep->StoppingCondition.EventParam))
        {
          StepCounter++;
          if (StepCounter >= ActivePlan->NumSteps)
          {
            // All steps done, post step complete to this SM to transition
            ReturnEvent.EventType = ES_PLAN_DONE;
          } else
          {
            DoStepActions();
            ReturnEvent.EventType = ES_NO_EVENT;
          }
        }
      
        // repeat for any concurrent lower level machines
      
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}


static void DoStepActions (void)
{
    /* Send Primitive CMD to DCMotorService*/
    // ANNOUNCE WHAT PLAN, STEP, MotorPrimitive, StopCondition and Event2Post HERE
    #ifdef VERBOSE_DRIVER
        DB_printf("\r[Driver] Plan: %u, Step: %u, Primitive: %u, StopCondition Type: %u, StopCondition Param: %u, Event2Post Type: %u, Event2Post Param: %u\n",
        (unsigned int)CurrentPlanIndex,
        (unsigned int)StepCounter,
        (unsigned int)ActivePlan->Steps[StepCounter].PrimitiveCmd,
        (unsigned int)ActivePlan->Steps[StepCounter].StoppingCondition.EventType,
        (unsigned int)ActivePlan->Steps[StepCounter].StoppingCondition.EventParam,
        (unsigned int)ActivePlan->Steps[StepCounter].PostEvent.EventType,
        (unsigned int)ActivePlan->Steps[StepCounter].PostEvent.EventParam
    );
    #endif
    ES_Event_t MotorCmdEvent;
    MotorCmdEvent.EventType = ES_MOTOR_PRIMITIVE;
    MotorCmdEvent.EventParam = ActivePlan->Steps[StepCounter].PrimitiveCmd; // Add parameters if needed
    PostDCMotorService(MotorCmdEvent);
    PostBeaconService(MotorCmdEvent);
    /* Post Event Sent on Entry */
    if (ES_NO_EVENT != ActivePlan->Steps[StepCounter].PostEvent.EventType)
    {
        ES_Event_t Event2Post;
        Event2Post = ActivePlan->Steps[StepCounter].PostEvent;
        ES_PostAll(Event2Post);
        if ((ActivePlan->Steps[StepCounter].PostEvent.EventType == ES_TIMEOUT) &&
             ActivePlan->Steps[StepCounter].PostEvent.EventParam == GameStartTimer){
          ES_Timer_InitTimer(GameStartTimer, INITIAL_ROTATE_CCW_MS);
        }
    }
    
}