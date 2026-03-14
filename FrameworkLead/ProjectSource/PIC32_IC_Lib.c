/****************************************************************************
 Module
     PIC32_IC_Lib.c
 Description
     Implementation of Input Capture (IC) library with multi-channel support,
     proper Timer selection, rollover handling, and pin mapping.
 History
 When           Who     What/Why
 -------------- ---     --------
 01/24/26       ege     Removed hardcoding, added prescaler and timer selection
 01/23/26       ege     Started programming
*****************************************************************************/

#include <xc.h>
#include "PIC32_IC_Lib.h"

/*------------------ Module Variables ------------------*/
static volatile uint16_t TimerPeriodTicks[2] = {0}; // Timer2/Timer3

/*------------------ Functions ------------------------*/

bool IC_Setup_ConfigureTimer(WhichTimer_t whichTimer, uint32_t periodTicks, IC_Prescaler_t prescaler)
{
    if (periodTicks < 1 || periodTicks > 0x10000) return false;

    switch(whichTimer)
    {
        case _Timer2_:
            T2CONbits.ON = 0;
            T2CONbits.TCS = 0;           // PBCLK
            T2CONbits.TCKPS = prescaler;
            PR2 = TimerPeriodTicks[0] = periodTicks - 1;
            IFS0CLR = _IFS0_T2IF_MASK;
            IPC2bits.T2IP = 6;
            T2CONbits.ON = 1;
            break;

        case _Timer3_:
            T3CONbits.ON = 0;
            T3CONbits.TCS = 0;           // PBCLK
            T3CONbits.TCKPS = prescaler;
            PR3 = TimerPeriodTicks[1] = periodTicks - 1;
            IFS0CLR = _IFS0_T3IF_MASK;
            IPC3bits.T3IP = 6;
            T3CONbits.ON = 1;
            break;

        default:
            return false;
    }

    return true;
}

bool IC_Setup_EnableChannel(IC_Channel_t channel, IC_CaptureMode_t mode)
{
    if (channel < IC1 || channel > IC5) return false;

    // Table of ICxCON registers
    static volatile void * const ICxCON_Table[5] = {
        &IC1CON, &IC2CON, &IC3CON, &IC4CON, &IC5CON
    };

    volatile __IC1CONbits_t *pICx = (__IC1CONbits_t *)ICxCON_Table[channel-1];

    pICx->ON = 0;             // disable before config
    pICx->ICM = mode;         // capture mode
    pICx->ICTMR = 0;          // use Timer3 (for IC2 in our service)
                              // IC1 can still use Timer2, adjust if needed

    IC_ClearInterruptFlag(channel);
    IC_ClearErrorFlag(channel);

    // Enable interrupt and set priority
    switch(channel)
    {
        case IC1: IPC1bits.IC1IP = 7; IEC0SET = _IEC0_IC1IE_MASK; break;
        case IC2: IPC2bits.IC2IP = 7; IEC0SET = _IEC0_IC2IE_MASK; break;
        case IC3: IPC3bits.IC3IP = 7; IEC0SET = _IEC0_IC3IE_MASK; break;
        case IC4: IPC4bits.IC4IP = 7; IEC0SET = _IEC0_IC4IE_MASK; break;
        case IC5: IPC5bits.IC5IP = 7; IEC0SET = _IEC0_IC5IE_MASK; break;
    }

    pICx->ON = 1;  // re-enable channel
    return true;
}

bool IC_Setup_MapChannelToPin(IC_Channel_t channel, IC_PinMap_t pin)
{
    // Map IC channel to input pin (RP number)
    // RP number corresponding to pin; for IC2 -> RPB9
    switch(channel)
    {
        case IC1: IC1R = pin; break;
        case IC2: IC2R = pin; break;
        case IC3: IC3R = pin; break;
        case IC4: IC4R = pin; break;
        case IC5: IC5R = pin; break;
        default: return false;
    }

    return true;
}

void IC_ClearInterruptFlag(IC_Channel_t channel)
{
    switch(channel)
    {
        case IC1: IFS0CLR = _IFS0_IC1IF_MASK; break;
        case IC2: IFS0CLR = _IFS0_IC2IF_MASK; break;
        case IC3: IFS0CLR = _IFS0_IC3IF_MASK; break;
        case IC4: IFS0CLR = _IFS0_IC4IF_MASK; break;
        case IC5: IFS0CLR = _IFS0_IC5IF_MASK; break;
    }
}

void IC_ClearErrorFlag(IC_Channel_t channel)
{
    switch(channel)
    {
        case IC1: IFS0CLR = _IFS0_IC1EIF_MASK; break;
        case IC2: IFS0CLR = _IFS0_IC2EIF_MASK; break;
        case IC3: IFS0CLR = _IFS0_IC3EIF_MASK; break;
        case IC4: IFS0CLR = _IFS0_IC4EIF_MASK; break;
        case IC5: IFS0CLR = _IFS0_IC5EIF_MASK; break;
    }
}
