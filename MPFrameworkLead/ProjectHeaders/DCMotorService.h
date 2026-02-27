//   Header file for template service
//   based on the Gen 2 Events and Services Framework

#pragma once

#include "ES_Events.h"
#include "ES_Types.h"

// Enumerate the possible states for the finite state machine
typedef enum
{
    FreeState = 0,
    LookingForBeacon,
    LookingForTape,
    TestingState
} DCMotorServiceState_t;

// Public Function Prototypes
bool InitDCMotorService(uint8_t Priority);
bool PostDCMotorService(ES_Event_t ThisEvent);
ES_Event_t RunDCMotorService(ES_Event_t ThisEvent);

// Used to set the direction pin to Hi or Lo
typedef enum
{
    Forward = 0,
    Reverse = 1,
} MotorDir_t;

// Helper
void DCMotor_ApplyTrim(int32_t trim, MotorDir_t dir);