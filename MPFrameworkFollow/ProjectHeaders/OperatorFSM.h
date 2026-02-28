/****************************************************************************

  Header file for Operator Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef FSMOperator_H
#define FSMOperator_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  OperatorInitPState, Standby, WaitingForNavigation, Intaking, LoweringDropoff, DroppingOff
}OperatorState_t;

// Public Function Prototypes

bool InitOperatorFSM(uint8_t Priority);
bool PostOperatorFSM(ES_Event_t ThisEvent);
ES_Event_t RunOperatorFSM(ES_Event_t ThisEvent);
OperatorState_t QueryOperatorSM(void);

#endif /* FSMOperator_H */

