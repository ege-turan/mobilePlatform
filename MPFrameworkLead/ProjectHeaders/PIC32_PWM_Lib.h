#ifndef _PWM_PIC32_H
#define _PWM_PIC32_H

/****************************************************************************
 Module
     PIC3_PWM_Lib.h
 Description
     header file to support use of the PWM library on the PIC32
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/17/26       ege     Updated for generalized PWM library
*****************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 Typedefs
*****************************************************************************/

// typdef to specify which timer is being set
#ifndef _TIMER_T
#define _TIMER_T
typedef enum {
  _Timer2_,
  _Timer3_
} WhichTimer_t;
#endif

// typdef to specify pins for PWM. Implementation code depends on this order
// Do not modify!
typedef enum {
        PWM_RPA0 = 0,
        PWM_RPA1,
        PWM_RPA2,
        PWM_RPA3,
        PWM_RPA4,
        PWM_RPB0,
        PWM_RPB1,
        PWM_RPB2,
        PWM_RPB3,
        PWM_RPB4,
        PWM_RPB5,
        PWM_RPB6,
        PWM_RPB7,
        PWM_RPB8,
        PWM_RPB9,
        PWM_RPB10,
        PWM_RPB11,
        PWM_RPB12,
        PWM_RPB13,
        PWM_RPB14,
        PWM_RPB15
} PWM_PinMap_t;

/*------------------------------ Prescaler -------------------------------*/
/*
 Prescaler -> Tick resolution (PBCLK = 20 MHz)

  Prescale | Tick Freq  | Tick Period
 ----------|------------|-------------
     1     | 20 MHz     | 50 ns
     2     | 10 MHz     | 100 ns
     4     | 5 MHz      | 200 ns
     8     | 2.5 MHz    | 400 ns
    16     | 1.25 MHz   | 800 ns
    32     | 625 kHz    | 1.6 us
    64     | 312.5 kHz  | 3.2 us
   256     | 78.125 kHz | 12.8 us

 Typical use:
   - Servos: /8 or /16
   - LEDs:   /1 or /2
   - Motors: /8 or /32

 Also, user needs at least a period of 100 ticks to guarantee >= 1% duty-cycle
 resolution.
*/
// Do not modify!
typedef enum {
  PWM_PS_1   = 0b000,
  PWM_PS_2   = 0b001,
  PWM_PS_4   = 0b010,
  PWM_PS_8   = 0b011,   // recommended default
  PWM_PS_16  = 0b100,
  PWM_PS_32  = 0b101,
  PWM_PS_64  = 0b110,
  PWM_PS_256 = 0b111
} PWM_Prescaler_t;

/****************************************************************************
 Interface
*****************************************************************************/

/****************************************************************************
 Function
    PWM_Setup_SetChannel

 Description
    Init a single OC channel for PWM (0% duty)

 Example
    PWM_Setup_SetChannel(2);
****************************************************************************/
bool PWM_Setup_SetChannel(uint8_t WhichChannel);

/****************************************************************************
 Function
    PWM_Setup_ConfigureTimer

 Description
    Configures a timer with period & prescaler, restarts it
    1 <= periodTicks <= 65536 (2^16)

 Example
    PWM_Setup_ConfigureTimer(_Timer2_, 5000, PWM_PS_8);
****************************************************************************/
bool PWM_Setup_ConfigureTimer(WhichTimer_t whichTimer, uint32_t periodTicks, PWM_Prescaler_t prescaler);

/****************************************************************************
 Function
    PWM_Setup_AssignChannelToTimer

 Description
    Assigns a channel to a timer

 Example
    PWM_Setup_AssignChannelToTimer(2, _Timer2_);
****************************************************************************/
bool PWM_Setup_AssignChannelToTimer(uint8_t whichChannel,
                                   WhichTimer_t whichTimer);

/****************************************************************************
 Function
    PWM_Setup_MapChannelToOutputPin

 Description
    Maps PWM channel to a pin

 Example
    PWM_Setup_MapChannelToOutputPin(2, PWM_RPB5);
****************************************************************************/
bool PWM_Setup_MapChannelToOutputPin(uint8_t WhichChannel, PWM_PinMap_t WhichPin);

/****************************************************************************
 Function
    PWM_Operate_SetDutyOnChannel

 Description
    Set duty cycle (0-100%) on a channel [1% resolution]

 Example
    PWM_Operate_SetDutyOnChannel(50, 2);
****************************************************************************/
bool PWM_Operate_SetDutyOnChannel(uint8_t dutyCycle, uint8_t whichChannel);

/****************************************************************************
 Function
    PWM_Operate_SetPulseWidthOnChannel

 Description
    Set pulse width (ticks) on a channel [more precise than SetDutyOnChannel]

 Example
    PWM_Operate_SetPulseWidthOnChannel(2500, 2);
****************************************************************************/
bool PWM_Operate_SetPulseWidthOnChannel(uint16_t NewPW, uint8_t whichChannel);

/****************************************************************************
 Example Usage:

// 1) Configure Timer2 for a 20ms period (typical servo) with /8 prescaler 
PWM_Setup_ConfigureTimer(_Timer2_, 50000, PWM_PS_8);

// 2) Initialize OC2 for PWM output
PWM_Setup_SetChannel(2);

// 3) Assign OC2 to Timer2
PWM_Setup_AssignChannelToTimer(2, _Timer2_);

// 4) Map OC2 to pin RPB5
PWM_Setup_MapChannelToOutputPin(2, PWM_RPB5);

// 5) Operate OC2
// 5.a) Set duty cycle to 50% on OC2
PWM_Operate_SetDutyOnChannel(50, 2);
// OR
// 5.b) Set pulse width in ticks on OC2
PWM_Operate_SetPulseWidthOnChannel(2500, 2);

// Now OC2 is outputting a PWM signal on RPB5 with 50% duty
****************************************************************************/

#endif //_PWM_PIC32_H