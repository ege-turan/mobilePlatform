/****************************************************************************
 Module: DCMotorService.c
 Revision: 1.0.1
 Description:
    This is a template file for implementing a simple service under the
    Gen2 Events and Services Framework.
 Notes:
    - Tested Forward and Reverse at 100 percent duty cycle
    - Tested Forward and Reverse at 50 percent duty cycle; right wheel feels weaker 
        may be a problem
    - 

 History
 When           Who     What/Why
 -------------- ---     --------
 01/16/12 09:58 nick    because i can
****************************************************************************/

// Motor 1 is the left motor when driving forward

/*-------------------------------------------------------------------------*/
/*----------------------------- Include Files -----------------------------*/
/*-------------------------------------------------------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "DCMotorService.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "PIC32_PWM_Lib.h"
#include "PIC32_IC_Lib.h"
#include "dbprintf.h"
#include <sys/attribs.h>

/*-------------------------------------------------------------------------*/
/*---------------------------- Testing Defines ----------------------------*/
/*-------------------------------------------------------------------------*/
// #define VERBOSE_MODE
ES_EventType_t LastTestAction;

/*-------------------------------------------------------------------------*/
/*----------------------------- Module Defines ----------------------------*/
/*-------------------------------------------------------------------------*/

// Used to initialise the PWM and also to select motor to command
typedef enum
{
    // Set to output compare channels 1 and 3
    Motor1ChannelOC = 2,
    Motor2ChannelOC = 3,
} MotorChannel_t;

// Define the tris bits (input/output)
#define MOTOR_1_PWM_PIN_TRIS (TRISBbits.TRISB8)
#define MOTOR_2_PWM_PIN_TRIS (TRISBbits.TRISB10)
#define MOTOR_1_DIR_PIN_TRIS (TRISBbits.TRISB9)
#define MOTOR_2_DIR_PIN_TRIS (TRISBbits.TRISB12)

// // Define the analog select for the pins which allow this
#define MOTOR_1_PWM_PIN_ANSEL (ANSELBbits.ANSB8)
#define MOTOR_1_DIR_PIN_ANSEL (ANSELBbits.ANSB9)
#define MOTOR_2_PWM_PIN_ANSEL (ANSELBbits.ANSB10)
#define MOTOR_2_DIR_PIN_ANSEL (ANSELBbits.ANSB12)

// Define the lat bits (write)
#define MOTOR_1_DIR_PIN_WRITE (LATBbits.LATB9)
#define MOTOR_2_DIR_PIN_WRITE (LATBbits.LATB12)

// Define pine names for PWM_Setup_MapChannelToOutputPin
#define Motor1PWMPinName PWM_RPB8
#define Motor2PWMPinName PWM_RPB10

// // Constants relating to output compare module 1
// #define OC1_PIN_SELECT (RPB15Rbits.RPB15R)
// #define OC1_PERIPHERAL (0b0101)

// TIMERS:
// This is the period of the PWM in timer ticks - for Timer2
// PBClock is 20 MHz, we want 200 Hz for PWM and we want a prescaler of at least
// 2 to be fit within the 16 bit timer register.
// Formula: PRy = F_PBCLK / (F_PWM * prescaler) - 1

// TIMER 2 DEFINES
// Prescaler: 2
// Notes: 200 Hz => 49999
//        250 Hz => 39999
//        500 Hz => 19999
//        1000 Hz => 9999
//        2000 Hz => 4999
//        5000 Hz => 3999
//        10000 Hz => 999
#define TimeConstant 0.000174 // in sec
#define PWM_PERIOD_TICKS 4999 // in ticks, 10Khz with prescaler 2

// TODO maybe lower the duty cycle for more precision??
#define R45DEG_100DUTY_DELAY_MS 500  // TODO tune this to be better approx. value
#define R90DEG_100DUTY_DELAY_MS 1000 // TODO tune this to be better approx. value

/* LAB 8 Keyboard COMMANDS */
#define LAB8_FWD_FULL   0x09 // 'w'
#define LAB8_REV_FULL   0x11 // 's'
#define LAB8_ROT_CCW_90 0x04 // 'a'
#define LAB8_ROT_CW_90  0x02 // 'd'
#define LAB8_STOP       0x00 // 'x'
#define LAB8_CCW_BEACON 0x20 // 'b'

/* Primitive Commands */
#include "PlansAndSteps.h"

// DUTY CYCLES FOR SPEEDS

#define SPEED_FORWARDS_PERCENT 40
#define SPEED_BACKWARDS_PERCENT 40
#define SPEED_ROTATE_PERCENT 30
#define SPEED_RIGHT_OFFSET_PERCENT 0 // SPEED + Offset should NOT exceed 100%


// #define TESTING_MODE // Set to 1 to enter testing mode on init

/*-------------------------------------------------------------------------*/
/*-------------------------- Function Prototypes --------------------------*/
/*-------------------------------------------------------------------------*/
void _InitMotorPWM();
void _DriveMotor(MotorChannel_t MotorChannel, uint32_t PulseWidthTicks, MotorDir_t Dir);
void _StopRobot();
void _DriveForward50();
void _DriveForward100();
void _DriveReverse50();
void _DriveReverse100();
void _RotateRobotCW();
void _RotateRobotCCW();
void _RotateForBeacon();

bool DCMotor_ExecutePrimitive(PrimitiveCmd_t cmd);
/*-------------------------------------------------------------------------*/
/*---------------------------- Module Variables ---------------------------*/
/*-------------------------------------------------------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

// This is the last known potentiometer value send from ADService, initialise to 0
static volatile uint16_t CurrentPotVal = 0;

// Declare the current state for the finite state machine (FSM)
static DCMotorServiceState_t CurrentState;

/*-------------------------------------------------------------------------*/
/*------------------------------ Module Code ------------------------------*/
/*-------------------------------------------------------------------------*/

// Function: InitDCMotorService
// Parameters: uint8_t : the priorty of this service
// Returns: bool, false if error in initialization, true otherwise
// Description:
// This function initialises this service.
bool InitDCMotorService(uint8_t Priority)
{
    ES_Event_t ThisEvent;
    MyPriority = Priority;

    // Announce initialisation of DCMotorService
    DB_printf("\rStarting DCMotorService: ");
    DB_printf("compiled at %s on %s", __TIME__, __DATE__);
    DB_printf("\n\r");

    // Initialise necessary timers and output compares for the PWM drive
    _InitMotorPWM();

    // Always stop the robot on initialisation to ensure it doens't run away :)
    _StopRobot();

    // Initialise the current state to the FreeState where we could receive any command
    CurrentState = FreeState;
    #ifdef TESTING_MODE
    CurrentState = TestingState;
    #endif            

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

// Function: PostDCMotorService
// Parameters: EF_Event_t ThisEvent, the event to post to the queue
// Returns: bool false if the Enqueue operation failed, true otherwise
// Description:
// This is the post function for this service.
bool PostDCMotorService(ES_Event_t ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

// Function: RunTemplateService
// Parameters: ES_Event_t : the event to process
// Returns: ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise
// Description:
// This is the main function that runs this service. It sends commands to the
// pwm signal to the dc motor and also listens for inputs from the ADService
// to control the speed.
ES_Event_t RunDCMotorService(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent;
    ES_Event_t NewEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    // Outer level FSM, this FSM assumes no commands AT ALL are received during the
    // waiting for completion of previous commands. The state machine WILL BREAK if
    // commands are received faster than specified.
    switch (CurrentState)
    {
        // In the state where we are not in the middle of doing anything and thus
        // are open to any command
        case FreeState:
        {
            switch (ThisEvent.EventType)
            {
                // This event is run once at the end of service initialisation
                case ES_INIT:
                {
                    DB_printf("\rES_INIT received in DCMotorService, priority: %d\r\n", MyPriority);
                }
                break;

                case ES_MOTORS_OFF: _StopRobot(); break;

                case ES_MOTOR_PRIMITIVE: DCMotor_ExecutePrimitive(ThisEvent.EventParam); break;

                case ES_NEW_SPI_CMD_RECEIVED: 
                {
                    switch (ThisEvent.EventParam)
                    {
                        case LAB8_FWD_FULL:
                        {
                            ES_Event_t NewSPIEvent;
                            NewSPIEvent.EventType  = ES_MOTOR_PRIMITIVE; NewSPIEvent.EventParam = Forwards;
                            PostDCMotorService(NewSPIEvent);
                            break;
                        }
                        case LAB8_REV_FULL:
                        {
                            ES_Event_t NewSPIEvent;
                            NewSPIEvent.EventType  = ES_MOTOR_PRIMITIVE; NewSPIEvent.EventParam = Backwards;
                            PostDCMotorService(NewSPIEvent);
                            break;
                        }
                        case LAB8_ROT_CCW_90:
                        {
                            ES_Event_t NewSPIEvent;
                            NewSPIEvent.EventType  = ES_MOTOR_PRIMITIVE; NewSPIEvent.EventParam = RotateCCW;
                            PostDCMotorService(NewSPIEvent);
                            break;
                        }
                        case LAB8_ROT_CW_90:
                        {
                            ES_Event_t NewSPIEvent;
                            NewSPIEvent.EventType  = ES_MOTOR_PRIMITIVE; NewSPIEvent.EventParam = RotateCW;
                            PostDCMotorService(NewSPIEvent);
                            break;
                        }
                        case LAB8_STOP:
                        {
                            ES_Event_t NewSPIEvent;
                            NewSPIEvent.EventType  = ES_MOTOR_PRIMITIVE; NewSPIEvent.EventParam = Stop;
                            PostDCMotorService(NewSPIEvent);
                            break;
                        }

                        default:
                            break;
                    }
                    break;
                }

                default:
                    break;
            }
        }
        break;

        // Actively looking for the beacon and not listening to any other commands
        // until the beacon is found - WILL NOT exit state until beacon is found.
        case LookingForBeacon:
        {
            switch (ThisEvent.EventType)
            {
                // Stop the robot when the robot is pointing at the beacon and transition
                // back to FreeState.
                case ES_BEACON_DISPENSER:
                {
                    _DriveForward100();
                    CurrentState = FreeState;
                }
                break;

                case ES_NEW_SPI_CMD_RECEIVED:
                {
                    // Go back to FreeState on any new command
                    CurrentState = FreeState;
                    // Post event again to handle it
                    PostDCMotorService(ThisEvent);
                }
                break;
                default:
                    break;
            }
        }
        break;

        // Actively looking for the tape and not listening to any other commands
        // until the tape is found - WILL NOT exit state until tape is found.
        case LookingForTape:
        {
            switch (ThisEvent.EventType)
            {
                // Stop the robot when the robot has found the tape and transition
                // back to FreeState.
                case ES_TAPE_DETECTED:
                {
                    _StopRobot();
                    CurrentState = FreeState;
                }
                break;

                case ES_NEW_SPI_CMD_RECEIVED:
                {
                    // Go back to FreeState on any new command
                    CurrentState = FreeState;
                    // Post event again to handle it
                    PostDCMotorService(ThisEvent);
                }
                break;
                default:
                    break;
            }
        }
        break;

        // Use this state to test aspects of the robot
        #ifdef TESTING_MODE
        case TestingState:
        {
            uint32_t TestTimerDelay_ms = 1000;

            switch (ThisEvent.EventType)
            {
                // This event is run once at the end of service initialisation
                case ES_INIT:
                {
                    DB_printf("\rES_INIT received in TestingState DCMotorService, priority: %d\r\n",
                              MyPriority);

                    // // Post an intial test event
                    // NewEvent.EventType = ET_TEST_ACTION_1;
                    // PostDCMotorService(NewEvent);
                    // LastTestAction = ET_TEST_ACTION_1;

                    // ES_Timer_InitTimer(TestingTimer, TestTimerDelay_ms);
                }
                break;

                // Change the action to whatever you want.
                case ES_NEW_KEY:
                {
                    // For manual testing via keyboard input
                    if (ThisEvent.EventParam == 'w')
                    {
                        _DriveForward100();
                    }
                    else if (ThisEvent.EventParam == 's')
                    {
                        _DriveReverse100();
                    }
                    else if (ThisEvent.EventParam == 'a')
                    {
                        _RotateRobotCCW();
                    }
                    else if (ThisEvent.EventParam == 'd')
                    {
                        _RotateRobotCW();
                    }
                    else if (ThisEvent.EventParam == 'x')
                    {
                        _StopRobot();
                    }
                    else if (ThisEvent.EventParam == 'q')
                    {
                        CurrentState = FreeState;
                        #ifdef VERBOSE_MODE
                        DB_printf("\rES_NEW_KEY: Exiting Testing State\r\n");
                        #endif
                    }
                    
                }
                break;

                // Change the action to whatever you want.
                case ET_TEST_ACTION_1:
                {
                    _DriveForward100();
                    // _DriveForward50();
                    // _RotateRobotCW();
                }
                break;

                // Change the action to whatever you want.
                case ET_TEST_ACTION_2:
                {
                    _DriveReverse100();
                    // _DriveReverse50();
                    // _RotateRobotCCW();
                }
                break;

                case ES_TIMEOUT:
                {
                    switch (ThisEvent.EventParam)
                    {
                        // Only do stuff if TestingTimer expires, (don't get confused by other services)
                        // Currently oscillates between the two test actions.
                        case TestingTimer:
                        {
                            // Always start another test timer.
                            ES_Timer_InitTimer(TestingTimer, TestTimerDelay_ms);

                            // Drive forward if last action was drive reverse
                            if (ET_TEST_ACTION_2 == LastTestAction)
                            {
                                NewEvent.EventType = ET_TEST_ACTION_1;
                                PostDCMotorService(NewEvent);
                                LastTestAction = ET_TEST_ACTION_1;
                            }
                            // Drive in reverse if last action was drive forward
                            else if (ET_TEST_ACTION_1 == LastTestAction)
                            {
                                NewEvent.EventType = ET_TEST_ACTION_2;
                                PostDCMotorService(NewEvent);
                                LastTestAction = ET_TEST_ACTION_2;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
        break;
        #endif
    }
    return ReturnEvent;
}

/*-------------------------------------------------------------------------*/
/*------------------------ Private Init Functions -------------------------*/
/*-------------------------------------------------------------------------*/

// Uses the PWM library written by Ege (bloody legend) to set up both motors
// in during the initialisation of the service.
void _InitMotorPWM()
{
    // Initialize Timer 2 for PWM
    PWM_Setup_ConfigureTimer(_Timer2_, PWM_PERIOD_TICKS + 1, PWM_PS_2);

    // Initialize OC1 for PWM1 output
    PWM_Setup_SetChannel(Motor1ChannelOC);
    PWM_Setup_AssignChannelToTimer(Motor1ChannelOC, _Timer2_);

    // Initialize OC3 for PWM2 output
    PWM_Setup_SetChannel(Motor2ChannelOC);
    PWM_Setup_AssignChannelToTimer(Motor2ChannelOC, _Timer2_);

    // Assign output compares for both pwm channels to respective pins
    PWM_Setup_MapChannelToOutputPin(Motor1ChannelOC, Motor1PWMPinName);
    PWM_Setup_MapChannelToOutputPin(Motor2ChannelOC, Motor2PWMPinName);

    // // Initialize pins to digital output
    // MOTOR_1_PWM_PIN_ANSEL = 0;
    // MOTOR_1_DIR_PIN_ANSEL = 0;
    // MOTOR_2_PWM_PIN_ANSEL = 0;
    MOTOR_2_DIR_PIN_ANSEL = 0;
    MOTOR_1_PWM_PIN_TRIS  = 0;
    MOTOR_2_PWM_PIN_TRIS  = 0;
    MOTOR_1_DIR_PIN_TRIS  = 0;
    MOTOR_2_DIR_PIN_TRIS  = 0;

    // Map Pins to OC modules
    PWM_Setup_MapChannelToOutputPin(Motor1ChannelOC, Motor1PWMPinName);
    PWM_Setup_MapChannelToOutputPin(Motor2ChannelOC, Motor2PWMPinName);
}

/*-------------------------------------------------------------------------*/
/*--------------------------- Private Functions ---------------------------*/
/*-------------------------------------------------------------------------*/

// This function allows us to control either motor and drive it at any PWM
// pulsewidth either forward or reverse.
void _DriveMotor(MotorChannel_t MotorChannel, uint32_t PulseWidthTicks, MotorDir_t Dir)
{
    // Write the current direction to the correct motor pin
    if (Motor1ChannelOC == MotorChannel)
    {
        MOTOR_1_DIR_PIN_WRITE = Dir;
    }
    else if (Motor2ChannelOC == MotorChannel)
    {
        MOTOR_2_DIR_PIN_WRITE = Dir;
    }
    else
    {
        DB_printf("_DriveMotor: Warning! Invalid motor selected, not "
                  "`Motor1ChannelOC` or "
                  "`Motor2ChannelOC`");
    }

    // Do nothing to the duty cycle if Reverse, invert the duty cycle with
    // a (1 - period_on_time) for the forward mode, as we are inverting the pin
    // voltage and thus the duty cycle gets inverted.
    if (Forward == Dir)
    {
        PWM_Operate_SetPulseWidthOnChannel(PulseWidthTicks, MotorChannel);
    }
    else if (Reverse == Dir)
    {
        PWM_Operate_SetPulseWidthOnChannel(PWM_PERIOD_TICKS - PulseWidthTicks, MotorChannel);
    }
    else
    {
        DB_printf("_DriveMotor2: Warning! Invalid direction selected.");
    }
} // TESTED

/*-------------------------------------------------------------------------*/
/*--------------------------- Command Functions ---------------------------*/
/*-------------------------------------------------------------------------*/

// Calling this function stops both motors from turning instantly
void _StopRobot()
{
#ifdef VERBOSE_MODE
    DB_printf("\rCommand Received: _StopRobot\r\n");
#endif

    // Could use forward or reverse
    _DriveMotor(Motor1ChannelOC, 0, Forward);
    _DriveMotor(Motor2ChannelOC, 0, Forward);
}

// Calling this function drives both motors forward at 50 percent duty cycle
void _DriveForward50()
{
#ifdef VERBOSE_MODE
    DB_printf("\rCommand Received: _DriveForward50\r\n");
#endif

    _DriveMotor(Motor1ChannelOC, PWM_PERIOD_TICKS*SPEED_FORWARDS_PERCENT/200, Forward);
    _DriveMotor(Motor2ChannelOC, PWM_PERIOD_TICKS*(SPEED_FORWARDS_PERCENT+SPEED_RIGHT_OFFSET_PERCENT)/200, Forward);
} // TESTED

// Calling this function drives both motors in reverse at 50 percent duty cycle
void _DriveReverse50()
{
#ifdef VERBOSE_MODE
    DB_printf("\rCommand Received: _DriveReverse50\r\n");
#endif

    _DriveMotor(Motor1ChannelOC, PWM_PERIOD_TICKS*SPEED_FORWARDS_PERCENT/200, Forward);
    _DriveMotor(Motor2ChannelOC, PWM_PERIOD_TICKS*(SPEED_FORWARDS_PERCENT+SPEED_RIGHT_OFFSET_PERCENT)/200, Forward);
} // TESTED

// Calling this function drives both motors forward at 100 percent duty cycle
void _DriveForward100()
{
#ifdef VERBOSE_MODE
    DB_printf("\rCommand Received: _DriveForward100\r\n");
#endif

    _DriveMotor(Motor1ChannelOC, PWM_PERIOD_TICKS*SPEED_FORWARDS_PERCENT/100, Forward);
    _DriveMotor(Motor2ChannelOC, PWM_PERIOD_TICKS*(SPEED_FORWARDS_PERCENT+SPEED_RIGHT_OFFSET_PERCENT)/100, Forward);
} // TESTED

// Calling this function drives both motors in reverse at 100 percent duty cycle
void _DriveReverse100()
{
#ifdef VERBOSE_MODE
    DB_printf("\rCommand Received: _DriveReverse100\r\n");
#endif

    _DriveMotor(Motor1ChannelOC, PWM_PERIOD_TICKS*SPEED_BACKWARDS_PERCENT/100, Reverse);
    _DriveMotor(Motor2ChannelOC, PWM_PERIOD_TICKS*(SPEED_BACKWARDS_PERCENT+SPEED_RIGHT_OFFSET_PERCENT)/100, Reverse);
} // TESTED

// Calling this function rotates the entire robot clockwise at 100 percent duty
// cycle about the point that lies exactly in between the two wheels
void _RotateRobotCW()
{
#ifdef VERBOSE_MODE
    DB_printf("\rCommand Received: _RotateRobotCW\r\n");
#endif

    _DriveMotor(Motor1ChannelOC, PWM_PERIOD_TICKS*SPEED_ROTATE_PERCENT/100, Forward);
    _DriveMotor(Motor2ChannelOC, PWM_PERIOD_TICKS*(SPEED_ROTATE_PERCENT+SPEED_RIGHT_OFFSET_PERCENT)/100, Reverse);
} // TESTED

// Calling this function rotates the entire robot clockwise at 100 percent duty
// cycle about the point that lies exactly in between the two wheels
void _RotateRobotCCW()
{
#ifdef VERBOSE_MODE
    DB_printf("\rCommand Received: _RotateRobotCCW\r\n");
#endif

    _DriveMotor(Motor1ChannelOC, PWM_PERIOD_TICKS*SPEED_ROTATE_PERCENT/100, Reverse);
    _DriveMotor(Motor2ChannelOC, PWM_PERIOD_TICKS*(SPEED_ROTATE_PERCENT+SPEED_RIGHT_OFFSET_PERCENT)/100, Forward);
} // TESTED

void _RotateForBeacon()
{
#ifdef VERBOSE_MODE
    DB_printf("\rCommand Received: _RotateForBeacon\r\n");
#endif

    _DriveMotor(Motor1ChannelOC, PWM_PERIOD_TICKS*SPEED_ROTATE_PERCENT/150, Reverse);
    _DriveMotor(Motor2ChannelOC, PWM_PERIOD_TICKS*(SPEED_ROTATE_PERCENT+SPEED_RIGHT_OFFSET_PERCENT)/150, Forward);
} // TESTED



bool DCMotor_ExecutePrimitive(PrimitiveCmd_t cmd) {
    switch(cmd) {
        case RotateCW:        _RotateRobotCW();   ES_PostAll((ES_Event_t){ES_STOP_DRIVE_CORRECT}); break;
        case RotateCCW:       _RotateRobotCCW();  ES_PostAll((ES_Event_t){ES_STOP_DRIVE_CORRECT}); break;
        case Forwards:        _DriveForward100(); ES_PostAll((ES_Event_t){ES_STOP_DRIVE_CORRECT}); break;
        case Forwards_slow:   _DriveForward50();  ES_PostAll((ES_Event_t){ES_STOP_DRIVE_CORRECT}); break;
        case Backwards:       _DriveReverse100(); ES_PostAll((ES_Event_t){ES_STOP_DRIVE_CORRECT}); break;
        case Backwards_slow:  _DriveReverse50();  ES_PostAll((ES_Event_t){ES_STOP_DRIVE_CORRECT}); break;
        case Stop:            _StopRobot();       ES_PostAll((ES_Event_t){ES_STOP_DRIVE_CORRECT}); break;
        
        // Line following and encoder count course correction
        case Forwards_line_mid:
            ES_PostAll((ES_Event_t){ES_START_LINE_FWD_MID});
            break;

        case Backwards_line_mid:
            ES_PostAll((ES_Event_t){ES_START_LINE_REV_MID});
            break;

        case Forwards_count_mid:
            ES_PostAll((ES_Event_t){ES_START_ENC_FWD_MID});
            break;

        case Backwards_count_mid:
            ES_PostAll((ES_Event_t){ES_START_ENC_REV_MID});
            break;

        default: return false; // unknown primitive
    }
    return true;
}

// For course-correction
void DCMotor_ApplyTrim(int32_t trim, MotorDir_t dir)
{
    
    uint32_t base = PWM_PERIOD_TICKS;
    if (dir == Forward)
    {
        base = PWM_PERIOD_TICKS*SPEED_FORWARDS_PERCENT/100;
    } else if (dir == Reverse)
    {
        base = PWM_PERIOD_TICKS*SPEED_BACKWARDS_PERCENT/100;
    } 
    int32_t left  = base - trim;
    int32_t right = base + trim;

    if(left < 0) left = 0;
    if(right < 0) right = 0;
    if(left > PWM_PERIOD_TICKS) left = PWM_PERIOD_TICKS;
    if(right > PWM_PERIOD_TICKS) right = PWM_PERIOD_TICKS;

    _DriveMotor(Motor1ChannelOC, left, dir);
    _DriveMotor(Motor2ChannelOC, right, dir);
}

/*-------------------------------------------------------------------------*/
/*------------------------- Private Test Functions ------------------------*/
/*-------------------------------------------------------------------------*/

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/