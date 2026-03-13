/****************************************************************************
 Module
   DropoffLoweringArmFSM.c

 Revision
   1.0.1

 Description
   This file implements DropoffLoweringArmService as a flat state machines under
the Gen2 Events and Services Framework.

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
#include "DropoffLoweringArmFSM.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "OperatorFSM.h"
#include "PIC32_PWM_Lib.h"
#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/
// PWM Channels
#define ARM_ELBOW_CH 1
#define ARM_HAND_CH 2

// PWM Pins
#define M_ARM_ELBOW_PWM PWM_RPA0
#define M_ARM_HAND_PWM PWM_RPA1
#define M_ARM_PWM_TIMER _Timer3_

// Timer already configured globally (Timer2 @ 20ms)

// Motion timer
#define ARM_STEP_TIME 500 // ms between trajectory points

// TRAJECTORIES
#define ARM_SEQ_LEN 5

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static DropoffLoweringArmState_t CurrentState;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;

// TRAJECTORIES
static uint8_t seqIndex;

// static const uint16_t LowerJ1[ARM_SEQ_LEN] = {375, 350, 320, 280, 250};
// static const uint16_t LowerJ2[ARM_SEQ_LEN]  = {375, 360, 340, 300, 270};
// static const uint16_t RaiseJ1[ARM_SEQ_LEN] = {250, 280, 320, 350, 375};
// static const uint16_t RaiseJ2[ARM_SEQ_LEN]  = {270, 300, 340, 360, 375};

// Sequence for raising the robot arm, J1 refers to joint 1
static const uint16_t LowerJ1[] = {600, 375};
static const uint16_t LowerJ2[] = {570, 375};

// 600 is about horizontal for J1
static const uint16_t RaiseJ1[] = {730};
static const uint16_t RaiseJ2[] = {700};

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitDropoffLoweringArmFSM

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
bool InitDropoffLoweringArmFSM(uint8_t Priority)
{
    ES_Event_t ThisEvent;

    MyPriority = Priority;
    // put us into the Initial PseudoState
    CurrentState = ArmInitPState;
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
     PostDropoffLoweringArmFSM

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
bool PostDropoffLoweringArmFSM(ES_Event_t ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunDropoffLoweringArmFSM

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
ES_Event_t RunDropoffLoweringArmFSM(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    switch (CurrentState)
    {
        case ArmInitPState: // If current state is initial Psedudo State
        {
            if (ThisEvent.EventType == ES_INIT)
            {
                // ----- Configure Timer3 for PWM at 50 Hz (20 ms period) -----
                // PBCLK = 20 MHz
                // Prescaler options: 1,8,64,256
                // PWM period ticks = PBCLK / (Prescaler * PWM_FREQ)
                // For 50 Hz: Period = 1 / 50 = 0.02 s
                // Using prescaler 64: ticks = 20e6 / (64 * 50) = 6250 ticks
                PWM_Setup_ConfigureTimer(M_ARM_PWM_TIMER, 6250, PWM_PS_64);

                // Map PWM channels
                PWM_Setup_SetChannel(ARM_ELBOW_CH);
                PWM_Setup_AssignChannelToTimer(ARM_ELBOW_CH, M_ARM_PWM_TIMER);
                PWM_Setup_MapChannelToOutputPin(ARM_ELBOW_CH, M_ARM_ELBOW_PWM);

                PWM_Setup_SetChannel(ARM_HAND_CH);
                PWM_Setup_AssignChannelToTimer(ARM_HAND_CH, M_ARM_PWM_TIMER);
                PWM_Setup_MapChannelToOutputPin(ARM_HAND_CH, M_ARM_HAND_PWM);

                // Home position
                // Pulse widths: 1.5 ms center, 1–2 ms range
                // DS3235: 20 ms period, 1 ms -> 5% duty, 2 ms -> 10% duty
                PWM_Operate_SetPulseWidthOnChannel(0, ARM_ELBOW_CH); // 1.5 ms center
                PWM_Operate_SetPulseWidthOnChannel(0, ARM_HAND_CH);

                CurrentState = ArmWaiting;
            }
        }
        break;

        case ArmWaiting:
        {
            switch (ThisEvent.EventType)
            {
                case ES_START_DOWN:
                {
                    PWM_Operate_SetPulseWidthOnChannel(375, ARM_ELBOW_CH); // 1.5 ms center
                    PWM_Operate_SetPulseWidthOnChannel(375, ARM_HAND_CH);
                }
                break;

                case ES_START_LOWERING_ARM:
                {
                    seqIndex = 0;
                    ES_Timer_InitTimer(ARM_MOVE_TIMER, ARM_STEP_TIME);
                    CurrentState = Lowering;
                }
                break;

                case ES_START_RAISING_ARM:
                {
                    DB_printf("\rES_START_RAISING_ARM\r\n");
                    seqIndex = 0;
                    ES_Timer_InitTimer(ARM_MOVE_TIMER, ARM_STEP_TIME);
                    CurrentState = Raising;
                }
                break;
            }
        }
        break;

        case Lowering:
        {
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == ARM_MOVE_TIMER)
            {
                PWM_Operate_SetPulseWidthOnChannel(LowerJ1[seqIndex], ARM_ELBOW_CH);
                PWM_Operate_SetPulseWidthOnChannel(LowerJ2[seqIndex], ARM_HAND_CH);

                seqIndex++;

                if (seqIndex >= sizeof(LowerJ1) / sizeof(LowerJ1[0]))
                {
                    ES_Event_t done;
                    done.EventType = ES_ARM_LOWERED;
                    PostOperatorFSM(done);

                    CurrentState = ArmWaiting;
                }
                else
                {
                    ES_Timer_InitTimer(ARM_MOVE_TIMER, ARM_STEP_TIME);
                }
            }
        }
        break;

        case Raising:
        {
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == ARM_MOVE_TIMER))
            {
                PWM_Operate_SetPulseWidthOnChannel(RaiseJ1[seqIndex], ARM_ELBOW_CH);
                PWM_Operate_SetPulseWidthOnChannel(RaiseJ2[seqIndex], ARM_HAND_CH);

                seqIndex++;

                DB_printf("\rStep Complete\r\n");

                if (seqIndex >= sizeof(RaiseJ1) / sizeof(RaiseJ1[0]))
                {
                    ES_Event_t done;
                    done.EventType = ES_ARM_RELEASED;
                    PostOperatorFSM(done);

                    CurrentState = ArmWaiting;
                }
                else
                {
                    ES_Timer_InitTimer(ARM_MOVE_TIMER, ARM_STEP_TIME);
                }
            }
        }
        break;
    } // end switch on Current State
    return ReturnEvent;
}

/****************************************************************************
 Function
     QueryDropoffLoweringArmSM

 Parameters
     None

 Returns
     DropoffLoweringArmState_t The current state of the DropoffLoweringArm state
machine

 Description
     returns the current state of the DropoffLoweringArm state machine
 Notes

 Author
     Ege Turan
****************************************************************************/
DropoffLoweringArmState_t QueryDropoffLoweringArmFSM(void)
{
    return CurrentState;
}

/***************************************************************************
 private functions
 ***************************************************************************/
