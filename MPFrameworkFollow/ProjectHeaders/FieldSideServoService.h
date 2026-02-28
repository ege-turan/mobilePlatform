/****************************************************************************

  Header file for FieldSideServo service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServFieldSideServo_H
#define ServFieldSideServo_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitFieldSideServoService(uint8_t Priority);
bool PostFieldSideServoService(ES_Event_t ThisEvent);
ES_Event_t RunFieldSideServoService(ES_Event_t ThisEvent);

#endif /* ServFieldSideServo_H */

