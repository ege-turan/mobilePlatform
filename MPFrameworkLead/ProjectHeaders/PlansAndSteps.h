#ifndef PLANS_AND_STEPS_H
#define PLANS_AND_STEPS_H

#include "ES_Configure.h"
#include "ES_Framework.h"

typedef enum {
    RotateCW,
    RotateCCW,
    Forwards,
    Backwards,
    Stop
} PrimitiveCmd_t;

typedef struct {
    PrimitiveCmd_t PrimitiveCmd;
    ES_EventType_t StoppingCondition;   // use EventType, not full ES_Event_t
    ES_EventType_t PostEvent;
} PlanStep_t;

typedef enum {
    PLAN_STARTPOS2LOADINGDOCK = 0,
    PLAN_LOADINGDOCK2BUCKET1,
    PLAN_COUNT
} PlanIndex_t;

typedef struct {
    const PlanStep_t *Steps;
    uint16_t NumSteps;
} Plan_t;

/* Extern declarations only */
extern const Plan_t AllPlans[PLAN_COUNT];

#endif /* PLANS_AND_STEPS_H */