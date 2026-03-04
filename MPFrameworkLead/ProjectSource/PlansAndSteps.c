#include "ES_Configure.h"
#include "ES_Framework.h"

#include "PlansAndSteps.h"


// NORMAL GAME PLANS
const PlanStep_t StartPos2LoadingDockSeq[] = {
    { RotateCCW, {ES_TIMEOUT, GameStartTimer}, {ES_TIMEOUT, GameStartTimer} },
    { RotateCW,  {ES_SIDE_FOUND, 0},           {ES_NO_EVENT, 0} },
    { RotateCCW, {ES_BEACON_DISPENSER, 0},     {ES_NO_EVENT, 0} },
    { Forwards,  {ES_LINE_PIVOT_L, 0},         {ES_NO_EVENT, 0} },
    { RotateCW,  {ES_LINE_PIVOT_R, 0},         {ES_NO_EVENT, 0} },
    { Backwards, {ES_LIMIT_SWITCH, 0},         {ES_SPI_INTAKE_ON, 0} },
    { Stop,      {ES_ERROR, 0},                {ES_PLAN_DONE, 0} }
};

const PlanStep_t LoadingDock2Bucket1Seq[] = {
    { Forwards,       {ES_LINE_PIVOT_L, 0}, {ES_NO_EVENT, 0} },
    { Forwards_count, {ES_COUNT_DONE, 0},   {ES_SPI_DROPOFF_REACHED, 0} },
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
    { RotateCCW, {ES_SIDE_FOUND, 0},        {ES_NO_EVENT, 0} },
    { RotateCCW, {ES_BEACON_DISPENSER, 0},  {ES_NO_EVENT, 0} },
    { RotateCW,  {ES_CENTERED, 0},          {ES_NO_EVENT, 0} },
    { Forwards,  {ES_LINE_PIVOT_L, 0},      {ES_NO_EVENT, 0} },
    { RotateCW,  {ES_LINE_PIVOT_R, 0},      {ES_NO_EVENT, 0} },
    { Backwards, {ES_LIMIT_SWITCH, 0},      {ES_SPI_INTAKE_ON, 0} },
    { Stop,      {ES_ERROR, 0},             {ES_PLAN_DONE, 0} }
};

const PlanStep_t DM_Start2ClosestBucket[] = {
    { Forwards,       {ES_LINE_PIVOT_L, 0}, {ES_NO_EVENT, 0} },
    { Forwards_count, {ES_COUNT_DONE, 0},   {ES_SPI_DROPOFF_REACHED, 0} },
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