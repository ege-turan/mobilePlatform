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
#define TIME_90_DEG_MS 1850

// NORMAL GAME PLANS
const PlanStep_t StartPos2LoadingDockSeq[] = {
    { .PrimitiveCmd = RotateCCW,           .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = DriverTimer },   .PostEvent = { .EventType = ES_DRIVER_TIMEOUT, .EventParam = 2200 } },
    { .PrimitiveCmd = RotateCW,            .StoppingCondition = { .EventType = ES_SIDE_FOUND, .EventParam = 0 },          .PostEvent = { .EventType = ES_LOOK_4_DISPENSER, .EventParam = 0 } },
    { .PrimitiveCmd = Forwards,            .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = DriverTimer },   .PostEvent = { .EventType = ES_DRIVER_TIMEOUT, .EventParam = TWO_SEC } },
    { .PrimitiveCmd = RotateCCW,           .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = DriverTimer },   .PostEvent = { .EventType = ES_DRIVER_TIMEOUT, .EventParam = 1600 } },
    { .PrimitiveCmd = Forwards,            .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = DriverTimer },   .PostEvent = { .EventType = ES_DRIVER_TIMEOUT, .EventParam = 1800 } },
    { .PrimitiveCmd = Backwards,           .StoppingCondition = { .EventType = ES_COUNT_DONE, .EventParam = 0 },          .PostEvent = { .EventType = ES_START_ENC_REV_MID, .EventParam = 265 } },
    { .PrimitiveCmd = RotateCCW,           .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = DriverTimer },   .PostEvent = { .EventType = ES_DRIVER_TIMEOUT, .EventParam = TIME_90_DEG_MS } },
    { .PrimitiveCmd = Backwards_count_mid, .StoppingCondition = { .EventType = ES_CENTERED_PIVOT, .EventParam = 0 },      .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
    { .PrimitiveCmd = Stop,                .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = DriverTimer },   .PostEvent = { .EventType = ES_DRIVER_TIMEOUT, .EventParam = HALF_SEC } },
    { .PrimitiveCmd = Backwards_count_mid, .StoppingCondition = { .EventType = ES_CENTERED_PIVOT, .EventParam = 0 },      .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
    { .PrimitiveCmd = Stop,                .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = DriverTimer },   .PostEvent = { .EventType = ES_DRIVER_TIMEOUT, .EventParam = HALF_SEC } },
    { .PrimitiveCmd = RotateCCW,           .StoppingCondition = { .EventType = ES_CENTERED_FRONT, .EventParam = 0 },      .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
    { .PrimitiveCmd = Stop,                .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = DriverTimer },   .PostEvent = { .EventType = ES_NEW_SPI_CMD_SEND, .EventParam = CMD_SPI_INTAKE_ON } }, // This postEvent also start a timer to wait for the intake to start
    { .PrimitiveCmd = Backwards_line_mid,  .StoppingCondition = { .EventType = ES_LIMIT_SWITCH, .EventParam = 0 },        .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
    { .PrimitiveCmd = Stop,                .StoppingCondition = { .EventType = ES_ERROR, .EventParam = 0 },               .PostEvent = { .EventType = ES_PLAN_DONE, .EventParam = 0 } }
};
// const PlanStep_t StartPos2LoadingDockSeq[] = {
//     { .PrimitiveCmd = RotateCCW, .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = GameStartTimer }, .PostEvent = { .EventType = ES_TIMEOUT, .EventParam = GameStartTimer } },
//     { .PrimitiveCmd = RotateCW, .StoppingCondition = { .EventType = ES_SIDE_FOUND, .EventParam = 0 }, .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
//     { .PrimitiveCmd = Forwards, .StoppingCondition = { .EventType = ES_LINE_PIVOT_L, .EventParam = 0 }, .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
//     { .PrimitiveCmd = RotateCCW, .StoppingCondition = { .EventType = ES_CENTERED, .EventParam = 0 }, .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
//     { .PrimitiveCmd = Forwards, .StoppingCondition = { .EventType = ES_LINE_PIVOT_L, .EventParam = 0 }, .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
//     { .PrimitiveCmd = RotateCCW, .StoppingCondition = { .EventType = ES_LINE_PIVOT_R, .EventParam = 0 }, .PostEvent = { .EventType = ES_NO_EVENT, .EventParam = 0 } },
//     { .PrimitiveCmd = Backwards, .StoppingCondition = { .EventType = ES_TIMEOUT, .EventParam = StartRotateTimer }, .PostEvent = { .EventType = ES_NEW_SPI_CMD_SEND, .EventParam = CMD_SPI_INTAKE_ON } },
//     { .PrimitiveCmd = Stop, .StoppingCondition = { .EventType = ES_ERROR, .EventParam = 0 }, .PostEvent = { .EventType = ES_PLAN_DONE, .EventParam = 0 } }
// };

const PlanStep_t LoadingDock2Bucket1Seq[] = {
    { Forwards,       {ES_LINE_PIVOT_L, 0}, {ES_NEW_SPI_CMD_SEND, CMD_SPI_DROPOFF_REACHED}},
    { Stop,           {ES_ERROR, 0},        {ES_PLAN_DONE, 0} }
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