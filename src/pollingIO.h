#ifndef _POLLINGIO_H
#define _POLLINGIO_H

#include "gameLogic.h"

//////////////// PIN DEFINITIONS //////////////////
#define UPPIN 		0x00000017
#define BUTTON 		0x0000000A


void joystickPoll(GameState_t *GameState);


#endif /* _POLLINGIO_H */
