/****************************************************************************
 Module
     PWM_PIC32.c

 Description
     Implementation file for the generalized PWM Library for the PIC32

 Notes
     - No hardcoded timer divisors or tick rates, derived from PBCLK and prescaler
     - Prescaler and timer selectable at runtime
     - Timer configuration is explicit (Timer2 and Timer3 are independent)
     - Uses existing channel/register mapping tables

 History
 When           Who     What/Why
 -------------- ---     --------
 01/17/26       ege     Removed hardcoding, added prescaler and timer selection
 01/16/26       ege     Started programming
 *****************************************************************************/

/*----------------------------- Include Files -----------------------------*/
#include <xc.h>
#include "PIC32_PWM_Lib.h"

/*----------------------------- Module Defines ----------------------------*/

#define MAX_NUM_CHANNELS     5

// Standard PBCLK for PIC32 (used in ME218 systems)
#define PBCLK_RATE 20000000L

/*---------------------------- Module Variables ---------------------------*/

// Cached timer periods (ticks)
static uint16_t T2Period;
static uint16_t T3Period;

// Cached duty cycles (%)
static uint8_t LocalDuty[MAX_NUM_CHANNELS] = {0, 0, 0, 0, 0};

/*----------------------- Channel/Register Mapping ------------------------*/

static volatile uint32_t * const ChannelTo_pControlReg[MAX_NUM_CHANNELS] = {
  &OC1CON, &OC2CON, &OC3CON, &OC4CON, &OC5CON
};

static volatile uint32_t * const ChannelTo_pOCRS_Reg[MAX_NUM_CHANNELS] = {
  &OC1RS, &OC2RS, &OC3RS, &OC4RS, &OC5RS
};

static volatile uint32_t * const ChannelTo_pOCR_Reg[MAX_NUM_CHANNELS] = {
  &OC1R, &OC2R, &OC3R, &OC4R, &OC5R
};

// Default all channels to Timer2
static volatile uint32_t * ChannelTo_pTimer[MAX_NUM_CHANNELS] = {
  &PR2, &PR2, &PR2, &PR2, &PR2
};

/*---------------------------- Pin Mapping -------------------------------*/

static PWM_PinMap_t const LegalOutPins[][MAX_NUM_CHANNELS] = {
  { PWM_RPA0, PWM_RPB3, PWM_RPB4, PWM_RPB7,  PWM_RPB15 },
  { PWM_RPA1, PWM_RPB1, PWM_RPB5, PWM_RPB8,  PWM_RPB11 },
  { PWM_RPA3, PWM_RPB0, PWM_RPB9, PWM_RPB10, PWM_RPB14 },
  { PWM_RPA2, PWM_RPA4, PWM_RPB2, PWM_RPB6,  PWM_RPB13 },
  { PWM_RPA2, PWM_RPA4, PWM_RPB2, PWM_RPB6,  PWM_RPB13 }
};

static volatile uint32_t * const outputMapRegisters[] = {
  &RPA0R, &RPA1R, &RPA2R, &RPA3R, &RPA4R,
  &RPB0R, &RPB1R, &RPB2R, &RPB3R, &RPB4R,
  &RPB5R, &RPB6R, &RPB7R, &RPB8R, &RPB9R,
  &RPB10R, &RPB11R, &RPB12R, &RPB13R, &RPB14R, &RPB15R
};

static uint32_t const mapChannel2PinSelConst[] = {
  0b0101, 0b0101, 0b0101, 0b0101, 0b0110
};

/*--------------------------- Private Helpers -----------------------------*/

static bool IsPinLegalForChannel(PWM_PinMap_t WhichPin, uint8_t WhichChannel)
{
  uint8_t idx;
  WhichChannel--;

  for (idx = 0; idx < MAX_NUM_CHANNELS; idx++)
  {
    if (LegalOutPins[WhichChannel][idx] == WhichPin)
    {
      return true;
    }
  }
  return false;
}

/*------------------------------ Module Code ------------------------------*/

/****************************************************************************
 Function
    PWM_Setup_SetChannel

 Description
    Configures a single OC channel for PWM operation.
****************************************************************************/
bool PWM_Setup_SetChannel(uint8_t WhichChannel)
{
  uint8_t channelIndex;

  if ((0 == WhichChannel) || (WhichChannel > MAX_NUM_CHANNELS))
  {
    return false;
  }

  channelIndex = WhichChannel - 1;

  // turn off the OC system before making changes
  ((__OC1CONbits_t *)ChannelTo_pControlReg[channelIndex])->ON = 0;

  // default to selecting Timer2
  ((__OC1CONbits_t *)ChannelTo_pControlReg[channelIndex])->OCTSEL = 0;

  // set PWM mode with no fault detect
  ((__OC1CONbits_t *)ChannelTo_pControlReg[channelIndex])->OCM = 0b110;

  // initialize duty cycle to 0
  *ChannelTo_pOCRS_Reg[channelIndex] = 0;
  *ChannelTo_pOCR_Reg[channelIndex]  = 0;

  // turn OC system back on
  ((__OC1CONbits_t *)ChannelTo_pControlReg[channelIndex])->ON = 1;

  return true;
}

/****************************************************************************
 Function
    PWM_Setup_ConfigureTimer
****************************************************************************/
bool PWM_Setup_ConfigureTimer(WhichTimer_t whichTimer, uint32_t periodTicks, PWM_Prescaler_t prescaler)
{
  if (prescaler > PWM_PS_256) return false;     // invalid prescaler

  if (periodTicks < 1 || periodTicks > 0x10000) return false; // invalid number of ticks

  switch (whichTimer)
  {
    case _Timer2_:
      T2CONbits.ON = 0;
      T2CONbits.TCS = 0;
      T2CONbits.TCKPS = prescaler;
      PR2 = T2Period = periodTicks - 1;
      T2CONbits.ON = 1;
      break;

    case _Timer3_:
      T3CONbits.ON = 0;
      T3CONbits.TCS = 0;
      T3CONbits.TCKPS = prescaler;
      PR3 = T3Period = periodTicks - 1;
      T3CONbits.ON = 1;
      break;

    default:
      return false; // invalid timer
  }

  return true;
}

/****************************************************************************
 Function
    PWM_Setup_AssignChannelToTimer
****************************************************************************/
bool PWM_Setup_AssignChannelToTimer(uint8_t whichChannel,
                                   WhichTimer_t whichTimer)
{
  uint8_t idx;

  if ((0 == whichChannel) || (whichChannel > MAX_NUM_CHANNELS))
  {
    return false;
  }

  idx = whichChannel - 1;

  if (_Timer2_ == whichTimer)
  {
    ChannelTo_pTimer[idx] = &PR2;
    ((__OC1CONbits_t *)ChannelTo_pControlReg[idx])->OCTSEL = 0;
  }
  else if (_Timer3_ == whichTimer)
  {
    ChannelTo_pTimer[idx] = &PR3;
    ((__OC1CONbits_t *)ChannelTo_pControlReg[idx])->OCTSEL = 1;
  }
  else
  {
    return false;
  }

  return true;
}

/****************************************************************************
 Function
    PWM_Setup_MapChannelToOutputPin
****************************************************************************/
bool PWM_Setup_MapChannelToOutputPin(uint8_t WhichChannel, PWM_PinMap_t WhichPin)
{
  if ((0 == WhichChannel) || (WhichChannel > MAX_NUM_CHANNELS) ||
      !IsPinLegalForChannel(WhichPin, WhichChannel))
  {
    return false;
  }

  *outputMapRegisters[WhichPin] =
    mapChannel2PinSelConst[WhichChannel - 1];

  return true;
}

/****************************************************************************
 Function
    PWM_Operate_SetDutyOnChannel
****************************************************************************/
bool PWM_Operate_SetDutyOnChannel(uint8_t dutyCycle, uint8_t whichChannel)
{
  uint32_t updateVal;
  uint8_t idx = whichChannel - 1;

  if ((0 == whichChannel) || (whichChannel > MAX_NUM_CHANNELS) ||
      (dutyCycle > 100))
  {
    return false;
  }

  LocalDuty[idx] = dutyCycle;

  if (0 == dutyCycle)
  {
    updateVal = 0;
  }
  else if (100 == dutyCycle)
  {
    updateVal = (*(ChannelTo_pTimer[idx])) + 1;
    if (updateVal == 0) updateVal = (*(ChannelTo_pTimer[idx])); // to handle edge case of PR = 0xFFFF overflow
  }
  else
  {
    updateVal = (*(ChannelTo_pTimer[idx]) * dutyCycle) / 100;
  }

  *ChannelTo_pOCRS_Reg[idx] = updateVal;
  return true;
}

/****************************************************************************
 Function
    PWM_Operate_SetPulseWidthOnChannel
****************************************************************************/
bool PWM_Operate_SetPulseWidthOnChannel(uint16_t NewPW, uint8_t whichChannel)
{
  if ((0 == whichChannel) || (whichChannel > MAX_NUM_CHANNELS) ||
      (NewPW > *ChannelTo_pTimer[whichChannel - 1]))
  {
    return false;
  }

  *ChannelTo_pOCRS_Reg[whichChannel - 1] = NewPW;
  return true;
}
