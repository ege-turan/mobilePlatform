/****************************************************************************

  Header file for drive course correct service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef DriveCorrectionService_H
#define DriveCorrectionService_H

#include "ES_Types.h"

typedef enum {
    DC_Idle,

    DC_LineFwd,
    DC_LineRev,

    DC_LineFwdMid,
    DC_LineRevMid,

    DC_EncFwd,
    DC_EncRev,

    DC_EncFwdMid,
    DC_EncRevMid

} DriveCorrectionState_t;


// Public Function Prototypes

bool InitDriveCorrectionService(uint8_t Priority);
bool PostDriveCorrectionService(ES_Event_t ThisEvent);
ES_Event_t RunDriveCorrectionService(ES_Event_t ThisEvent);

#endif /* DriveCorrectionService_H */

