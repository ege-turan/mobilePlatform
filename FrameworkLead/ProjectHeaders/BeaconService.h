/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServBeacon_H
#define ServBeacon_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitBeaconService(uint8_t Priority);
bool PostBeaconService(ES_Event_t ThisEvent);
ES_Event_t RunBeaconService(ES_Event_t ThisEvent);

#endif /* ServBeacon_H */

