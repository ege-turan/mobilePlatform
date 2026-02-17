#ifndef _IC_PIC32_H
#define _IC_PIC32_H

/****************************************************************************
 Module
     PIC32_IC_Lib.h
 Description
     Header file to support Input Capture (IC) on the PIC32
 Notes
     - Mirrors PWM library style
     - Supports IC1..IC5 channels
     - Timer2 is dedicated as the time base
     - Pin mapping included
 History
 When           Who     What/Why
 -------------- ---     --------
 01/20/26       ege     Simplified, added pin map
*****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 Typedefs
*****************************************************************************/

// IC channels
typedef enum {
    IC1 = 1,
    IC2,
    IC3,
    IC4,
    IC5
} IC_Channel_t;

// Timer used as IC time base
#ifndef _TIMER_T
#define _TIMER_T
typedef enum {
  _Timer2_,
  _Timer3_
} WhichTimer_t;
#endif

// Capture mode
typedef enum {
    IC_DisableMode = 0b00, // DASO I think 0b00 is Capture Disable mode
    IC_EveryEdge   = 0b01, 
    IC_FallingEdge = 0b10, 
    IC_RisingEdge  = 0b11
} IC_CaptureMode_t;

// Prescaler for Timer2
typedef enum {
    IC_PS_1   = 0b000,
    IC_PS_2   = 0b001,
    IC_PS_4   = 0b010,
    IC_PS_8   = 0b011,
    IC_PS_16  = 0b100,
    IC_PS_32  = 0b101,
    IC_PS_64  = 0b110,
    IC_PS_256 = 0b111
} IC_Prescaler_t;

// Pins that can be mapped to IC channels
typedef enum {
    IC_RPA0 = 0,
    IC_RPA1,
    IC_RPA2,
    IC_RPA3,
    IC_RPA4,
    IC_RPB0,
    IC_RPB1,
    IC_RPB2,
    IC_RPB3,
    IC_RPB4,
    IC_RPB5,
    IC_RPB6,
    IC_RPB7,
    IC_RPB8,
    IC_RPB9,
    IC_RPB10,
    IC_RPB11,
    IC_RPB12,
    IC_RPB13,
    IC_RPB14,
    IC_RPB15
} IC_PinMap_t;

/****************************************************************************
 Interface
****************************************************************************/

bool IC_Setup_ConfigureTimer(WhichTimer_t whichTimer, uint32_t periodTicks, IC_Prescaler_t prescaler);
bool IC_Setup_EnableChannel(IC_Channel_t channel, IC_CaptureMode_t mode);
bool IC_Setup_DisableChannel(IC_Channel_t channel);
uint32_t IC_ReadCapturedValue(IC_Channel_t channel);
void IC_ClearInterruptFlag(IC_Channel_t channel);
void IC_ClearErrorFlag(IC_Channel_t channel);
bool IC_Setup_MapChannelToPin(IC_Channel_t channel, IC_PinMap_t pin);

#endif //_IC_PIC32_H
