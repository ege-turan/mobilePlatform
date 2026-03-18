 #ifndef PLANS_AND_STEPS_H
#define PLANS_AND_STEPS_H

#include "ES_Configure.h"
#include "ES_Framework.h"


#define INTAKE_WAIT_MS 1000
#define LOWER_BUCKET_WAIT_MS 1000

typedef enum {
    RotateCW,
    RotateCW_count,
    RotateCCW,
    RotateCCW_count,

    Forwards,
    Forwards_slow,
    Forwards_count,
    Forwards_count_mid,
    Forwards_line,
    Forwards_line_mid,

    Backwards,
    Backwards_slow,
    Backwards_count,
    Backwards_count_mid,
    Backwards_line,
    Backwards_line_mid,

    Stop
} PrimitiveCmd_t;

typedef struct {
    PrimitiveCmd_t PrimitiveCmd;
    ES_Event_t StoppingCondition;   // use full ES_Event_t
    ES_Event_t PostEvent;
} PlanStep_t;

typedef enum {
    PLAN_STARTPOS2LOADINGDOCK = 0,
    PLAN_LOADINGDOCK2BUCKET1,
    PLAN_BUCKET1_2_BUCKET2,
    /* NUM_PLANS tells how many plans there are. Line below goes last always */
    NUM_PLANS
} PlanIndex_t;

typedef enum {
    PLAN_DM_START_SEE_BEACON = 0,
    PLAN_DM_START2CLOSESTBUCKET,
    /* DM_NUM_PLANS tells how many plans there are. Line below goes last always */
    DM_NUM_PLANS
} DMPlanIndex_t;

typedef struct {
    const PlanStep_t *Steps;
    uint16_t NumSteps;
} Plan_t;

extern const Plan_t Plans[NUM_PLANS];



extern const Plan_t DM_Plans[DM_NUM_PLANS];

#endif /* PLANS_AND_STEPS_H */