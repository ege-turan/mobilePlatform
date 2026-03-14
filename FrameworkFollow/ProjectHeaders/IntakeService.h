/****************************************************************************

  Header file for Intake service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServIntake_H
#define ServIntake_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitIntakeService(uint8_t Priority);
bool PostIntakeService(ES_Event_t ThisEvent);
ES_Event_t RunIntakeService(ES_Event_t ThisEvent);

#endif /* ServIntake_H */

