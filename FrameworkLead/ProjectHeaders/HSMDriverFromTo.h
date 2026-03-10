/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef HSMDriver_H
#define HSMDriver_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { STANDBY_STEP, RUNNING_STEP } TemplateState_t ;


// Public Function Prototypes

ES_Event_t RunDriverFromToSM( ES_Event_t CurrentEvent );
void StartDriverFromToSM ( ES_Event_t CurrentEvent );
TemplateState_t QueryDriverFromToSM ( void );

#endif /*HSMDriver_H */

