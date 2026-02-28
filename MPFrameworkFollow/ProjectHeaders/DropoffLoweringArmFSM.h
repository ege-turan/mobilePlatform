/****************************************************************************

  Header file for DropoffLoweringArm Flat Sate Machine
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef FSMDropoffLoweringArm_H
#define FSMDropoffLoweringArm_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  ArmInitPState, ArmReady, Raising, Lowering
}DropoffLoweringArmState_t;

// Public Function Prototypes

bool InitDropoffLoweringArmFSM(uint8_t Priority);
bool PostDropoffLoweringArmFSM(ES_Event_t ThisEvent);
ES_Event_t RunDropoffLoweringArmFSM(ES_Event_t ThisEvent);
DropoffLoweringArmState_t QueryDropoffLoweringArmSM(void);

#endif /* FSMDropoffLoweringArm_H */

