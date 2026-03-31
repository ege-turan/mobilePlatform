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

#include "DropoffLoweringArmFSM.h"
#include "FieldSideServoService.h"
#include "IntakeService.h"
#include "OperatorFSM.h"
#include "SPIFollowService.h"
#include "StepperService.h"

#include "PIC32_PWM_Lib.h"

#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/

// Measured
#define CAPACITY 30
#define GAME_COUNT 10

// Timers
#define GAME_TIME_MS     21800
#define INTAKE_PACE_MS   10000
#define HARD_CODED_INTAKE_COMPLETE_MS 8000
#define DROPOFF_PACE_MS  7500

// Pins
#define DIN_GAMEMODE PORTBbits.RB4
#define DIN_GAMEMODE_TRIS TRISBbits.TRISB4

// Agitator
#define M_AGITATOR_LAT LATBbits.LATB5
#define M_AGITATOR_TRIS TRISBbits.TRISB5
#define AGITATOR_DUTY_STEPS 5      // 0,20,40,60,80,100%

// Intake PWM
#define INTAKE_PWM_FREQ 10000

#define M_INTAKE_CH 4
#define M_INTAKE_PWM_TIMER _Timer2_
#define M_INTAKE_PWM_PIN PWM_RPB2

#define INTAKE_PWM_PSC PWM_PS_1
#define INTAKE_PWM_PERIOD 2000
#define INTAKE_PWM_DUTY 25 // as percentage

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
static void InitOperatorInterrupts(void);

// Intake Helpers
static void Intake_Init(void);
static void Intake_On(void);
static void Intake_Off(void);

// Agitator Helpers
static void Agitator_Init(void);
static void Agitator_On(void);
static void Agitator_Off(void);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match that of enum in header file
static OperatorState_t CurrentState;

static uint8_t carrying;
static uint8_t gameCounter;
static uint8_t GameMode;
static uint8_t Strategy;

static uint8_t DutyCycle = 1;     // 0–5
static uint8_t HighTime = 0;
static uint8_t LowTime = 0;
static bool OutputState = false;
static bool agitatorOn = false;

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

    // Announce initialisation of DCMotorService
    DB_printf("\rStarting OperatorFSM: ");
    DB_printf("compiled at %s on %s", __TIME__, __DATE__);
    DB_printf("\n\r");

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

    if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == GAME_TIMER)
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
            spiEvent.EventType  = ES_NEW_SPI_CMD_SEND;
            spiEvent.EventParam = CMD_SPI_END;
            PostSPIFollowService(spiEvent);

            // create an event ES_FEEDER_STOP and post it to StepperService
            ES_Event_t feederStopEvent;
            feederStopEvent.EventType = ES_FEEDER_STOP;
            PostStepperService(feederStopEvent);

            // create an event to reset field side indicator (ES_RESET)
            ES_Event_t resetFieldSideEvent;
            resetFieldSideEvent.EventType = ES_RESET;
            PostFieldSideServoService(resetFieldSideEvent);

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
    if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == AGITATOR_PWM_TIMER)
    {
        if(OutputState)
        {
            M_AGITATOR_LAT = 0;
            OutputState = false;
            if (LowTime > 0)
                ES_Timer_InitTimer(AGITATOR_PWM_TIMER, LowTime);
        } else
        {
            if(agitatorOn)
            {
                M_AGITATOR_LAT = 1;
                OutputState = true;
            }

            if (HighTime > 0)
                ES_Timer_InitTimer(AGITATOR_PWM_TIMER, HighTime);
        }
    }

    switch (CurrentState)
    {

        /********************* INIT PSEUDOSTATE *********************/
        case OperatorInitPState:
        {
            if (ThisEvent.EventType == ES_INIT)
            {
                DB_printf("\rES_INIT received in OperatorFSM, priority: %d\r\n", MyPriority);

                // Initialize variables
                carrying    = 0;
                GameMode    = 0;
                Strategy    = 0;
                gameCounter = 0;

                // Set RB4 as digital input for GAMEMODE
                DIN_GAMEMODE_TRIS = 1;

                // Initialize agitator and intake
                Agitator_Init();
                Intake_Init();
                DB_printf("\r IntakeInit Complete \r\n");

                // Intake_On();
                // DB_printf("\r IntakeOn \r\n");

                // Agitator_On();
                // DB_printf("\r AgitatorOn \r\n");

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
                    carrying = 1; // starting with 1 cargo for testing
                    DB_printf("\rStandby, ES_StART_DOWN ");

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
                
                case ES_NEW_SPI_CMD_RECEIVED: if(ThisEvent.EventParam == CMD_SPI_INTAKE_ON){Intake_On();} break;
                case ES_FEEDER_STOP: Intake_Off();     break;
                case ES_AGITATOR_START: Agitator_On(); break;
                case ES_AGITATOR_STOP: Agitator_Off(); break;
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
                        ES_Timer_InitTimer(INTAKE_COMPLETE_TIMER, HARD_CODED_INTAKE_COMPLETE_MS);
                        DB_printf("\rWaitingForNavigation CMD_SPI_INTAKE_ON ");

                        CurrentState = Intaking;

                        // turn on intake PWM
                        Intake_On();
                        // Agitator_On();
                    }
                    else if (ThisEvent.EventParam == CMD_SPI_DROPOFF_REACHED)
                    {
                        DB_printf("\rWaitingForNavigation CMD_SPI_DROPOFF_REACHED, starting DROPOFF_PACE_TIMER\r\n ");
                        ES_Timer_InitTimer(DROPOFF_PACE_TIMER, DROPOFF_PACE_MS);

                        ES_Event_t armEvent;
                        armEvent.EventType = ES_START_LOWERING_ARM;
                        PostDropoffLoweringArmFSM(armEvent);

                        CurrentState = LoweringDropoff;
                    }
                    else if (ThisEvent.EventParam == CMD_SPI_RELEASE_DROPOFF)
                    {
                        // Leader requested dropoff to be raised
                        ES_Event_t armEvent;
                        armEvent.EventType = ES_START_RAISING_ARM;
                        PostDropoffLoweringArmFSM(armEvent);

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
                        ES_Timer_InitTimer(INTAKE_PACE_TIMER, INTAKE_PACE_MS); // sets & starts
                    }
                    else
                    {
                        // Stop intake
                        Intake_Off(); // <- turn off intake
                        // Stop intake pacing timer
                        ES_Timer_StopTimer(INTAKE_PACE_TIMER); // stop completely

                        // Notify SPI and IntakeService
                        ES_Event_t loadedEvent;
                        loadedEvent.EventType  = ES_NEW_SPI_CMD_SEND;
                        loadedEvent.EventParam = CMD_SPI_LOADED; // or payload if needed

                        PostSPIFollowService(loadedEvent); // send to leader

                        CurrentState = WaitingForNavigation;
                    }
                }
                break;

                case ES_TIMEOUT:
                {
                    if (ThisEvent.EventParam == INTAKE_PACE_TIMER)
                    {
                        ES_Event_t incompleteEvent;
                        incompleteEvent.EventType  = ES_NEW_SPI_CMD_SEND;
                        incompleteEvent.EventParam = CMD_SPI_INTAKE_INCOMPLETE;
                        PostSPIFollowService(incompleteEvent);
                        Intake_Off(); // <- also turn off intake on timeout
                        CurrentState = WaitingForNavigation;
                    }
                    if (ThisEvent.EventParam == INTAKE_COMPLETE_TIMER)
                    {
                        Intake_Off();
                        // Start Raising the arm and send command that it's loaded
                        // Raise arm
                        ES_Event_t armEvent;
                        armEvent.EventType = ES_START_RAISING_ARM;
                        PostDropoffLoweringArmFSM(armEvent);
                        CurrentState = RaisingDropoff;

                        // Send Loaded Command to SPI
                        ES_Event_t completeEvent;
                        completeEvent.EventType  = ES_NEW_SPI_CMD_SEND;
                        completeEvent.EventParam = CMD_SPI_LOADED;
                        PostSPIFollowService(completeEvent);
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
                case ES_ARM_LOWERED:
                {
                    if (carrying == 0)
                    {
                        ES_Timer_StopTimer(
                            DROPOFF_PACE_TIMER); // double check that we stopped the timer

                        // Notify SPI that dropoff is unloaded
                        ES_Event_t unloadEvent;
                        unloadEvent.EventType  = ES_NEW_SPI_CMD_SEND;
                        unloadEvent.EventParam = CMD_SPI_UNLOADED;
                        PostSPIFollowService(unloadEvent);

                        CurrentState = WaitingForNavigation;
                    }
                    else // carrying > 0
                    {
                        DB_printf("\rLoweringDropoff, ES_ARM_LOWERED, going to DroppingOff state\r\n ");
                        // // Stop pacing timer and move to DroppingOff state
                        // ES_Timer_StopTimer(DROPOFF_PACE_TIMER);
                        CurrentState = DroppingOff;

                        // create an event ES_FEEDER_START and post it to StepperService
                        ES_Event_t feederStartEvent;
                        feederStartEvent.EventType = ES_FEEDER_START;
                        PostStepperService(feederStartEvent);

                        // turn on agitator
                        Agitator_On();
                    }
                }
                break;

                case ES_NEW_SPI_CMD_RECEIVED:
                {
                    if (ThisEvent.EventParam == CMD_SPI_RELEASE_DROPOFF)
                    {
                        // Stop pacing timer and move to RaisingDropoff state
                        ES_Event_t armEvent;
                        armEvent.EventType = ES_START_RAISING_ARM;
                        PostDropoffLoweringArmFSM(armEvent);

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
                            ES_Timer_StopTimer(
                                DROPOFF_PACE_TIMER); // double check that we stopped the timer

                            // Notify SPI that all cargo has been unloaded
                            ES_Event_t unloadEvent;
                            unloadEvent.EventType  = ES_NEW_SPI_CMD_SEND;
                            unloadEvent.EventParam = CMD_SPI_UNLOADED;
                            PostSPIFollowService(unloadEvent);

                            // create an event ES_FEEDER_STOP and post it to StepperService
                            ES_Event_t feederStopEvent;
                            feederStopEvent.EventType = ES_FEEDER_STOP;
                            PostStepperService(feederStopEvent);

                            // turn off agitator
                            Agitator_Off();

                            CurrentState = WaitingForNavigation;
                        }
                    }
                }
                break;
                case ES_TIMEOUT:
                {
                    if (ThisEvent.EventParam == DROPOFF_PACE_TIMER)
                    {
                        DB_printf("\rDroppingOff, DROPOFF_PACE_TIMER timeout, going to RaisingDropoff state\r\n ");
                        // Stop feeder and agitator
                        ES_Event_t feederStopEvent;
                        feederStopEvent.EventType = ES_FEEDER_STOP;
                        PostStepperService(feederStopEvent);
                        Agitator_Off();

                        // Notify SPI that dropoff is unloaded
                        ES_Event_t unloadEvent;
                        unloadEvent.EventType  = ES_NEW_SPI_CMD_SEND;
                        unloadEvent.EventParam = CMD_SPI_UNLOADED;
                        PostSPIFollowService(unloadEvent);

                        // // Raise arm back up
                        // ES_Event_t armEvent;
                        // armEvent.EventType = ES_START_RAISING_ARM;
                        // PostDropoffLoweringArmFSM(armEvent);

                        CurrentState = WaitingForNavigation;
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
                case ES_ARM_RELEASED:
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

//---------------- Agitator ----------------
static void Agitator_Init(void)
{
    M_AGITATOR_TRIS = 0; // output
    M_AGITATOR_LAT  = 0; // start off
}

static void Agitator_On(void)
{
    DB_printf("[follower ] Operator FSM: AGITATOR START\r\n");
    agitatorOn = true;
    M_AGITATOR_LAT = 1;
    uint8_t period = AGITATOR_DUTY_STEPS;

    HighTime = DutyCycle;
    LowTime = period - DutyCycle;

    OutputState = true;
    ES_Timer_InitTimer(AGITATOR_PWM_TIMER, HighTime);
}

static void Agitator_Off(void)
{
    DB_printf("[follower ] Operator FSM: AGITATOR STOP\r\n");
    M_AGITATOR_LAT = 0;
    agitatorOn = false;
    ES_Timer_StopTimer(AGITATOR_PWM_TIMER); // stop toggling
}

//---------------- Intake ------------------
static void Intake_Init(void)
{
    // Configure Timer2 for 10 kHz PWM
    PWM_Setup_ConfigureTimer(M_INTAKE_PWM_TIMER, INTAKE_PWM_PERIOD, INTAKE_PWM_PSC);

    // Map PWM channel
    PWM_Setup_SetChannel(M_INTAKE_CH);
    PWM_Setup_AssignChannelToTimer(M_INTAKE_CH, M_INTAKE_PWM_TIMER);
    PWM_Setup_MapChannelToOutputPin(M_INTAKE_CH, M_INTAKE_PWM_PIN);

    ANSELBbits.ANSB2 = 0;
    TRISBbits.TRISB2 = 0;
    // Start OFF
    PWM_Operate_SetPulseWidthOnChannel(0, M_INTAKE_CH);
    DB_printf("\r Intake Initialized \r\n ");
}

static void Intake_On(void)
{
    PWM_Operate_SetPulseWidthOnChannel(INTAKE_PWM_PERIOD * INTAKE_PWM_DUTY / 100,
                                       M_INTAKE_CH); // 50%
    DB_printf("\r INTAKE ON \r\n ");
    //    while(1){};
}

static void Intake_Off(void)
{
    PWM_Operate_SetPulseWidthOnChannel(0, M_INTAKE_CH);
    DB_printf("\r INTAKE OFF \r\n ");
}

static void InitOperatorInterrupts(void)
{
    __builtin_disable_interrupts();

    // ---- Disable analog on cargo input pins ----
    // No analog selection for these pins
    //    ANSELBbits.ANSB8 = 0;   // DIN_CARGO_IN
    //    ANSELBbits.ANSB9 = 0;   // DIN_CARGO_OUT

    // ---- Set inputs ----
    TRISBbits.TRISB8 = 1; // DIN_CARGO_IN
    TRISBbits.TRISB9 = 1; // DIN_CARGO_OUT

    // ---- Map pins to external interrupts ----
    // PRB8 -> INT3
    INT3Rbits.INT3R = 0b0100; // check datasheet: RB8 -> INT3
    // PRB9 -> INT1
    INT1Rbits.INT1R = 0b0100; // check datasheet: RB9 -> INT1

    // ---- Configure interrupts ----
    // INT1 -> Cargo OUT (falling edge)
    INTCONbits.INT1EP = 1; // falling edge
    IFS0bits.INT1IF   = 0; // clear flag
    IEC0bits.INT1IE   = 1; // enable interrupt
    IPC1bits.INT1IP   = 5; // priority 5

    // INT3 -> Cargo IN (rising edge)
    INTCONbits.INT3EP = 0; // rising edge
    IFS0bits.INT3IF   = 0; // clear flag
    IEC0bits.INT3IE   = 1; // enable interrupt
    IPC3bits.INT3IP   = 5; // priority 5

    __builtin_enable_interrupts();
}

/***************************************************************************
 ISR (Interrupt Service Routine)
 ***************************************************************************/
// use INT1 at #define DIN_CARGO_OUT at PRB9 at falling edge to post an ES_CARGO_OUT event to OperatorFSM
// and INT3 at #define DIN_CARGO_IN at PRB8 at rising edge to post ES_CARGO_IN to OperatorFSM
void __ISR(_EXTERNAL_1_VECTOR, IPL5SOFT) _CargoOut_ISR(void)
{
    IFS0bits.INT1IF = 0; // clear interrupt flag
    ES_Event_t CargoEvent;
    CargoEvent.EventType = ES_CARGO_OUT;
    PostOperatorFSM(CargoEvent);
}

void __ISR(_EXTERNAL_3_VECTOR, IPL5SOFT) _CargoIn_ISR(void)
{
    IFS0bits.INT3IF = 0; // clear interrupt flag
    ES_Event_t CargoEvent;
    CargoEvent.EventType = ES_CARGO_IN;
    PostOperatorFSM(CargoEvent);
}