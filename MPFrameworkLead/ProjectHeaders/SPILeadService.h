/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef SPILeadService_H
#define SPILeadService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitSPILeadService(uint8_t Priority);
bool PostSPILeadService(ES_Event_t ThisEvent);
ES_Event_t RunSPILeadService(ES_Event_t ThisEvent);

#endif /* SPILeadService_H */


