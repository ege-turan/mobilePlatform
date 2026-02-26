// #include "PlansAndSteps.h"

// const PlanStep_t StartPos2LoadingDockSeq[] = {
//     { RotateCCW, ES_SIDE_FOUND,       ES_NO_EVENT },
//     { RotateCCW, ES_BEACON_DISPENSER, ES_SIDE_FOUND },
//     { Forwards,  ES_LINE_PIVOT_L,     ES_NO_EVENT },
//     { RotateCW,  ES_LINE_PIVOT_R,     ES_NO_EVENT },
//     { Backwards, ES_LIMIT_SWITCH,     ES_SPI_INTAKE_ON },
//     { Stop,      ES_ERROR, ES_PLAN_DONE }
// };

// const PlanStep_t LoadingDock2Bucket1Steps[] = {
//     { RotateCW,  ES_SIDE_FOUND,   ES_NO_EVENT },
//     { Forwards,  ES_LINE_PIVOT_L, ES_NO_EVENT },
//     { RotateCCW, ES_LINE_PIVOT_R, ES_NO_EVENT },
//     { Forwards,  ES_LIMIT_SWITCH, ES_SPI_DROPOFF_REACHED },
//     { Stop,      ES_ERROR,        ES_PLAN_DONE }
// };

// extern const Plan_t AllPlans[PLAN_COUNT] =
// {
//     [PLAN_STARTPOS2LOADINGDOCK] =
//     {
//         .Steps = StartPos2LoadingDockSeq,
//         .NumSteps = sizeof(StartPos2LoadingDockSeq)/sizeof(PlanStep_t)
//     },

//     [PLAN_LOADINGDOCK2BUCKET1] =
//     {
//         .Steps = LoadingDock2Bucket1Steps,
//         .NumSteps = sizeof(LoadingDock2Bucket1Steps)/sizeof(PlanStep_t)
//     }
// };