#include <stdlib.h>
#include <stdio.h>
#include <cmsis_os.h>
#include <lpc17xx.h>
#include <stdbool.h>
#include "GLCD.h"
#include "gameLogic.h"
#include "pollingIO.h"
#include "LCD_Display.h"

//////////////// MUTEX DEFINITIONS //////////////////
osMutexDef(LCDMutex);
osMutexId LCDMutexId;


/*******************************************************************************
* Display landscape bars at top and bottom, whose height is determined by 		 *
* randomly generated number, with an upper limit															 *
*******************************************************************************/
void landscapeUpdateThread(const void* args) {
	GameState_t *Gooserino = (GameState_t*)args;
	
	//Initialize the landscape with bars of random heights
	initLandscape(Gooserino);
	
	while (1){
		if(!Gooserino->mainMenu && !Gooserino->gameOver && !Gooserino->lifeLost) {
			landscapeUpdate(Gooserino);
		}
	}
}
osThreadDef(landscapeUpdateThread,osPriorityNormal, 1, 0);


/*******************************************************************************
* Initialize goose figure, and update continuously														 *
********************************************************************************/
void gooseUpdateThread(const void* args) {
	GameState_t *Gooserino = (GameState_t*)args;

	// Initalize Gooserino's location and display on LCD
	Gooserino->goose.direction = GOOSE_DOWN;
	Gooserino->goose.xpos = BAR_WIDTH-GOOSE_XSIZE;
	Gooserino->goose.ypos  = 120; 
	
	while(1) {
		if(!Gooserino->mainMenu && !Gooserino->gameOver && !Gooserino->lifeLost) {
			gooseUpdate(Gooserino);
		}
	}
}
osThreadDef(gooseUpdateThread, osPriorityNormal, 1, 0);


/*******************************************************************************
* Joystick polling task. Update Gooserino's direction upon joystick up press 	 *
********************************************************************************/
void joystickPollThread(const void* args) {
	GameState_t *Gooserino = (GameState_t*)args;
	while(1) {
		if(!Gooserino->mainMenu && !Gooserino->gameOver && !Gooserino->lifeLost){
			joystickPoll(Gooserino);
		}
	}		
	
}
osThreadDef(joystickPollThread, osPriorityNormal, 1, 0);

/**********************************************************************************
* Collision Detection task. Continually verify intersection of goose and landscape*
***********************************************************************************/
void collisionDetectThread(const void* args) {
	GameState_t *Gooserino = (GameState_t*)args;
	while(1) {
		if(!Gooserino->mainMenu && !Gooserino->gameOver && !Gooserino->lifeLost) {
			collisionDetect(Gooserino);
		}	
	}

}
osThreadDef(collisionDetectThread, osPriorityNormal, 1, 0);

int main(void) {	
	printf("");	

	///// KERNEL INITIALIZATION /////
	osKernelInitialize();
	osKernelStart();
	
	///// MUTEX INITIALIZATION /////
	LCDMutexId = osMutexCreate(osMutex(LCDMutex));
	osMutexWait(LCDMutexId, osWaitForever);
	
	///// LCD INITIALIZATION /////
	GLCD_Init();
	GLCD_Clear(White);
	
	///// GAME STATE INITIALIZATION /////
	GameState_t* Gooserino = malloc(sizeof(GameState_t));
	initGame(Gooserino); 
	
	///// THREAD INITIALIZATION /////
	osThreadCreate(osThread(landscapeUpdateThread), Gooserino);
	osThreadCreate(osThread(gooseUpdateThread), Gooserino);	
	osThreadCreate(osThread(collisionDetectThread), Gooserino);
	osThreadCreate(osThread(joystickPollThread), Gooserino);
}


