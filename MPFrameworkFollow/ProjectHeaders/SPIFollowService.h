/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef SPIFollowService_H
#define SPIFollowService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitSPIFollowService(uint8_t Priority);
bool PostSPIFollowService(ES_Event_t ThisEvent);
ES_Event_t RunSPIFollowService(ES_Event_t ThisEvent);

#endif /* SPIFollowService_H */

