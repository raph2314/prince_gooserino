#include <stdlib.h>
#include <stdio.h>
#include <cmsis_os.h>
#include <lpc17xx.h>
#include "gameLogic.h"
#include "pollingIO.h"


void joystickPoll(GameState_t *GameState){
	if(!GameState->mainMenu){		
		// Do nothing until joystick is pressed up
		while(((LPC_GPIO1->FIOPIN >> UPPIN) & 0x01));
		
		GameState->goose.direction = GOOSE_UP; 
		
		// Do nothing until joysick is released
		while(!((LPC_GPIO1->FIOPIN >> UPPIN) & 0x01));
		
		GameState->goose.direction = GOOSE_DOWN; 
	}
}
