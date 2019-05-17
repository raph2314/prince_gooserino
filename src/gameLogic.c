#include <stdlib.h>
#include <stdio.h>
#include <cmsis_os.h>
#include <lpc17xx.h>
#include "gameLogic.h"
#include "LCD_Display.h"

extern osMutexId LCDMutexId; 
extern unsigned short GOOSE_CLR[];

void initGame(GameState_t *GameState) {
	GLCD_Clear(Black); 
	
	// Initialize game parameters to default
	GameState->lives = 3; 
	GameState->score = 0; 
	GameState->difficultyLevel = EASY;
	GameState->collided = false; 
	GameState->mainMenu = true; 
	GameState->gameOver = false; 
	
	// Instantiate first obstacle
	GameState->obstacle.xpos = 320;
	GameState->obstacle.ypos = 100; 
	
	// Keep track of the progress of the landscape
	GameState->landscape.progress=0;
	
	// Initialize the landscape with bars of random heights
	initLandscape(GameState);
	
	// Display the default LED Lives
	displayLEDLives(GameState);
	
	// Display Main Menu
	osMutexWait(LCDMutexId, osWaitForever);
	displayMainMenu(GameState); 
	osMutexRelease(LCDMutexId);
}

void displayLEDLives(GameState_t* GameState) {
	// Start by clearing all LEDs 
	LPC_GPIO2->FIODIR |= 0x0000007C;
	LPC_GPIO1->FIODIR |= 0xB0000000;
	LPC_GPIO2->FIOCLR |= 0x00000070;
	
	if(GameState->lives == 3) 
		LPC_GPIO2->FIOSET |= 0x00000070;
	
	else if(GameState->lives == 2) 
		LPC_GPIO2->FIOSET |= 0x00000060;
	
	else if(GameState->lives == 1) 
		LPC_GPIO2->FIOSET |= 0x00000040;
	
	else if(GameState->lives == 0) 
		LPC_GPIO2->FIODIR |= 0x0000007C;
}

void collisionDetect(GameState_t *GameState) {		
	if(!GameState->collided) {
		/*#################################
		## GOOSE -> OBSTACLE COLLISION ##
		#################################*/
			
		// Check if goose lines up with obstacle along x-axis
		if(((GameState->goose.xpos + GOOSE_XSIZE > GameState->obstacle.xpos) && (GameState->goose.xpos < GameState->obstacle.xpos )) || 
			 ((GameState->goose.xpos + GOOSE_XSIZE > GameState->obstacle.xpos + OBSTACLE_XSIZE) && (GameState->goose.xpos < GameState->obstacle.xpos + OBSTACLE_XSIZE))){ 
					 
			// The next two cases directly cover collision from above and below by comparing the y-positions
			// Frontal collision is covered indirectly by these two conditions and does not have to be further 
			//dealt with 
			
			// Goose is above obstacle top
			if(GameState->goose.ypos < GameState->obstacle.ypos) {
				for(uint16_t i = 0; i < OBSTACLE_YSIZE; i++) {
					if(GameState->goose.ypos + GOOSE_YSIZE == (GameState->obstacle.ypos + i))
						GameState->collided = true; 
				}
			}
			else if (GameState->goose.ypos > GameState->obstacle.ypos) {
				for(uint16_t i = 0; i < OBSTACLE_YSIZE; i++) {
					if(GameState->goose.ypos == (GameState->obstacle.ypos + i))
						GameState->collided = true; 
				}
			}
		}
			 
		/*#################################
		## GOOSE -> LANDSCAPE COLLISION ##
		#################################*/
		
		if(GameState->landscape.progress <= 16){
			if ((GameState->goose.ypos<=(GameState->landscape.landscapeArray[0][0]*10+20))||((GameState->goose.ypos+GOOSE_YSIZE)>=
				(SCREEN_HEIGHT-GameState->landscape.landscapeArray[1][0]*10))){
				GameState->collided = true; 
			}
		}
		if(GameState->landscape.progress >= 16){
		if ((GameState->goose.ypos<=(GameState->landscape.landscapeArray[0][1]*10+20))||((GameState->goose.ypos+GOOSE_YSIZE)>=
			(SCREEN_HEIGHT-GameState->landscape.landscapeArray[1][1]*10))){
			GameState->collided = true; 
			}
		}
	}
	
	// Collide flag is high 
	if(GameState->collided){	
		// Set collide flag back to low 
		GameState->collided = false;
		
		if(GameState->lives == 1) {
			// Flag gameOver state to stop 
			GameState->gameOver = true;
			
			osMutexWait(LCDMutexId, osWaitForever);
			displayGameOver(GameState);
			osMutexRelease(LCDMutexId);
		}
		else {
			// Set GameState flags
			GameState->lifeLost = true; 
			GameState->lives -= 1;

			// Reset LEDs to current life count
			displayLEDLives(GameState);
			
			// Clear and reset current obstacle AND goose figure
			osMutexWait(LCDMutexId, osWaitForever);
			drawOneBar(GameState->obstacle.xpos+14, GameState->obstacle.ypos, BAR_WIDTH/2, GameState->obstacle.height, CLEAR_PARAM); 
			
			// GLCD_Bargraph(GameState->obstacle.xpos+14, GameState->obstacle.ypos, BAR_WIDTH/2, GameState->obstacle.height, White);
			GLCD_Bitmap(GameState->goose.xpos, GameState->goose.ypos, GOOSE_XSIZE, GOOSE_YSIZE, (unsigned char*)GOOSE_CLR);
			osMutexRelease(LCDMutexId);
			
			// Reset Game State parameters for both goose and obstacle locations
			GameState->goose.ypos = 100;
			GameState->obstacle.xpos = 320;
			GameState->obstacle.ypos = 100; 
			
			// Show collision message
			osMutexWait(LCDMutexId, osWaitForever);
			displayLifeLost(GameState); 
			osMutexRelease(LCDMutexId);
		}
	}
}
