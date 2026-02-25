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
    PrimitiveCmd_t PrimitiveCmd;       // e.g., RotateCW, Forward
    ES_Event_t StoppingCondition;     // e.g., ES_SPI_DROPOFF_REACHED
    ES_Event_t PostEvent;             // e.g., ES_PLAN_DONE or ES_NO_EVENT if none
} PlanStep_t;

extern const PlanStep_t PlanSteps[];  // array of steps for current plan
extern const uint16_t NUM_STEPS;      // number of steps in current plan


const PlanStep_t StartPos2LoadingDockSeq[] = {
    { RotateCW,        ES_SIDE_FOUND,       ES_NO_EVENT       }, // Step 0
    { RotateCCW,       ES_CENTERED,         ES_SIDE_FOUND     }, // Step 1
    { Forwards,        ES_LINE_PIVOT_L,     ES_NO_EVENT       }, // Step 2
    { RotateCW,        ES_LINE_PIVOT_R,     ES_NO_EVENT       }, // Step 3
    { Backwards,       ES_LIMIT_SWITCH,     ES_SPI_INTAKE_ON  }, // Step 4
    { Stop,            ES_ERROR,            ES_PLAN_DONE      }  // Step 5
};

const uint16_t NUM_STEPS_StartPos2LoadingDock = sizeof(StartPos2LoadingDockSeq)/sizeof(PlanStep_t);