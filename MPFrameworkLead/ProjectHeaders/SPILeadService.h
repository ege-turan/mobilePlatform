/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef SPILeadService_H
#define SPILeadService_H

#include "ES_Types.h"

// Defining SPI commands shorthand in header for easy access across services

/* LAB 8 Keyboard COMMANDS */
#define LAB8_FWD_FULL             0x09 // 'w'
#define LAB8_REV_FULL             0x11 // 's'
#define LAB8_ROT_CCW_90           0x04 // 'a'
#define LAB8_ROT_CW_90            0x02 // 'd'
#define LAB8_STOP                 0x00 // 'x'
#define LAB8_CW_BEACON            0x20 // 'b'
/* LEAD2FOLLOW SPI CMDS*/
#define CMD_SPI_LEAD_INITIAL      0xAA
#define CMD_SPI_INTAKE_ON         0xA1
#define CMD_SPI_DROPOFF_REACHED   0xA2
#define CMD_SPI_BLUE_TEAM         0xA3
#define CMD_SPI_GREEN_TEAM        0xA4
#define CMD_SPI_RELEASE_DROPOFF   0xA5
/* FOLLOW2LEAD SPI CMDS*/
#define CMD_SPI_FOLLOW_INITIAL    0xFF
#define CMD_SPI_START             0xB1
#define CMD_SPI_START_DEATHMATCH  0xB7
#define CMD_SPI_INTAKE_INCOMPLETE 0xB2
#define CMD_SPI_LOADED            0xB3
#define CMD_SPI_UNLOADED          0xB4
#define CMD_SPI_RELEASED          0xB8
#define CMD_SPI_UNLOAD_INCOMPLETE 0xB5
#define CMD_SPI_END               0xB6


// Public Function Prototypes

bool InitSPILeadService(uint8_t Priority);
bool PostSPILeadService(ES_Event_t ThisEvent);
ES_Event_t RunSPILeadService(ES_Event_t ThisEvent);

#endif /* SPILeadService_H */


