#include "ES_Configure.h"
#include "ES_Framework.h"

#include "PlansAndSteps.h"
#include "SPILeadService.h"

#define ONE_SEC   1000
#define HALF_SEC  (ONE_SEC / 2)
#define TWO_SEC   (ONE_SEC * 2)
#define THREE_SEC (ONE_SEC * 3)
#define FOUR_SEC  (ONE_SEC * 4)
#define FIVE_SEC  (ONE_SEC * 5)
#define SIX_SEC   (ONE_SEC * 6)
#define TIME_90_DEG_MS 1850

#define STEP(primCmd, stopCondType, stopCondParam, postEventType, postEventParam) \
    { .PrimitiveCmd = primCmd, .StoppingCondition = { .EventType = stopCondType, .EventParam = stopCondParam }, .PostEvent = { .EventType = postEventType, .EventParam = postEventParam } }

// NORMAL GAME PLANS
const PlanStep_t StartPos2LoadingDockSeq[] = {
//  STEP(primitive command,      stopEvent type, stopEvent param,    postEvent type,       postEvent param   ),
    STEP(RotateCCW,              ES_TIMEOUT,         DriverTimer,    ES_DRIVER_TIMEOUT,     2200             ),
    STEP(RotateCW,               ES_SIDE_FOUND,                0,    ES_LOOK_4_DISPENSER,   0                ),
    STEP(RotateCW,               ES_TIMEOUT,         DriverTimer,    ES_DRIVER_TIMEOUT,     200              ),
    STEP(Forwards,               ES_TIMEOUT,         DriverTimer,    ES_DRIVER_TIMEOUT,     SIX_SEC          ),
    STEP(RotateCCW,              ES_TIMEOUT,         DriverTimer,    ES_DRIVER_TIMEOUT,     1700             ),
    STEP(Forwards,               ES_TIMEOUT,         DriverTimer,    ES_DRIVER_TIMEOUT,     1950             ),
    STEP(Backwards,              ES_COUNT_DONE,                0,    ES_START_ENC_REV_MID,  260              ),
    STEP(RotateCCW,              ES_TIMEOUT,         DriverTimer,    ES_DRIVER_TIMEOUT,     TIME_90_DEG_MS   ),
    STEP(Backwards,              ES_TIMEOUT,         DriverTimer,    ES_DRIVER_TIMEOUT,     4500             ),
    STEP(Forwards_count_mid,     ES_CENTERED_PIVOT,            0,    ES_NO_EVENT,           0                ),
    STEP(Stop,                   ES_TIMEOUT,         DriverTimer,    ES_DRIVER_TIMEOUT,     HALF_SEC         ),
    STEP(RotateCCW,              ES_CENTERED_FRONT,            0,    ES_NO_EVENT,           0                ),
    STEP(Stop,                   ES_TIMEOUT,         DriverTimer,    ES_NEW_SPI_CMD_SEND,   CMD_SPI_INTAKE_ON),
    STEP(Backwards_line_mid,     ES_LIMIT_SWITCH,              0,    ES_NO_EVENT,           0                ),
    STEP(Stop,                   ES_ERROR,                     0,    ES_PLAN_DONE,          0                )
};

const PlanStep_t LoadingDock2Bucket1Seq[] = {
    STEP(Forwards,               ES_COUNT_DONE,              0,    ES_START_ENC_REV_MID, 300           ),
    STEP(Stop,                   ES_ERROR,                   0,    ES_PLAN_DONE,         0             )
};

const Plan_t Plans[NUM_PLANS] =
{
    [PLAN_STARTPOS2LOADINGDOCK] =
    {
        .Steps = StartPos2LoadingDockSeq,
        .NumSteps = sizeof(StartPos2LoadingDockSeq)/sizeof(PlanStep_t)
    },

    [PLAN_LOADINGDOCK2BUCKET1] =
    {
        .Steps = LoadingDock2Bucket1Seq,
        .NumSteps = sizeof(LoadingDock2Bucket1Seq)/sizeof(PlanStep_t)
    }
};

// DEATHMATCH GAME PLANS
const PlanStep_t DM_StartrSeeBeacon[] = {
    { .PrimitiveCmd = RotateCCW, .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = THREE_SEC },        .PostEvent = { .EventType = ES_DRIVER_TIMEOUT, .EventParam = THREE_SEC } },
    { .PrimitiveCmd = RotateCW,  .StoppingCondition = { .EventType = ES_SIDE_FOUND, .EventParam = 0 },             .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
    { .PrimitiveCmd = Forwards,  .StoppingCondition = { .EventType = ES_LINE_PIVOT_L, .EventParam = 0 },           .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
    { .PrimitiveCmd = RotateCCW, .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = HALF_SEC },         .PostEvent = { .EventType = ES_DRIVER_TIMEOUT, .EventParam = HALF_SEC } },
    { .PrimitiveCmd = Forwards,  .StoppingCondition = { .EventType = ES_CENTERED_PIVOT, .EventParam = 0 },         .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
    { .PrimitiveCmd = RotateCCW, .StoppingCondition = { .EventType = ES_LINE_PIVOT_R, .EventParam = 0 },           .PostEvent = { .EventType = ES_DRIVER_TIMEOUT, .EventParam = FIVE_SEC } },
    { .PrimitiveCmd = Backwards, .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = StartRotateTimer }, .PostEvent = { .EventType = ES_NEW_SPI_CMD_SEND, .EventParam = CMD_SPI_INTAKE_ON } },
    { .PrimitiveCmd = Stop,      .StoppingCondition = { .EventType = ES_ERROR, .EventParam = 0 },                  .PostEvent = { .EventType = ES_PLAN_DONE, .EventParam = 0 } }
};

const PlanStep_t DM_Start2ClosestBucket[] = {
    { Forwards,       {ES_LINE_PIVOT_L, 0}, {ES_NO_EVENT, 0} },
    { Stop,           {ES_ERROR, 0},        {ES_PLAN_DONE, 0} }
};

const Plan_t DM_Plans[DM_NUM_PLANS] =
{
    [PLAN_DM_START_SEE_BEACON] =
    {
        .Steps = DM_StartrSeeBeacon,
        .NumSteps = sizeof(DM_StartrSeeBeacon)/sizeof(PlanStep_t)
    },

    [PLAN_DM_START2CLOSESTBUCKET] =
    {
        .Steps = DM_Start2ClosestBucket,
        .NumSteps = sizeof(DM_Start2ClosestBucket)/sizeof(PlanStep_t)
    }
};