#include <stdlib.h>
#include <stdio.h>
#include <cmsis_os.h>
#include <lpc17xx.h>
#include "LCD_Display.h"
#include "gameLogic.h"
#include "pollingIO.h"


extern osMutexId LCDMutexId;

extern unsigned short GOOSEMAP[];

/*##########################
  ##			TITLE PAGES 		##
	##########################*/
	
void displayMainMenu(GameState_t* GameState) {
	GLCD_Clear(Black);
	
	unsigned char title[] = "PRINCESS GOOSERINO";
	unsigned char prompt[] = "PRESS BUTTON TO BEGIN";
	unsigned char empty[] = " ";
	
	// Display Title 
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(White);
	GLCD_DisplayString(2, 1, 1, title);
	
	// Display colorful borders and button prompt
	GLCD_SetBackColor(Magenta);
	GLCD_DisplayString(22, 15, 0, prompt);
	
	uint8_t i;
	for(i = 0; i < 60; i++){
		GLCD_DisplayString(0, i, 0, empty);
		GLCD_DisplayString(29,i, 0, empty);
	}
	for(i = 0; i < 30; i++){
		GLCD_DisplayString(i, 0, 0, empty);
		GLCD_DisplayString(i,52, 0, empty);
	}
	
	// Stay in main menu state until button is pressed
	uint8_t buttonPressed = !((LPC_GPIO2->FIOPIN >> BUTTON) & 0x01);
	while(!buttonPressed)
		buttonPressed = !((LPC_GPIO2->FIOPIN >> BUTTON) & 0x01);
	
	// Clear screen and set mainMenu flag to false for game to start
	GLCD_Clear(White);
	GameState->mainMenu = false;
	GLCD_SetBackColor(White);
}

void displayGameOver(GameState_t *GameState){
	osDelay(1000);
	GLCD_Clear(Black);
	
	unsigned char deathMsg[] = "YOU HAVE FAILED";	
	unsigned char prompt[] = "PRESS BUTTON FOR REDEMPTION";
	unsigned char empty[] = " ";
	
	// Display deathMsg 
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Magenta);
	GLCD_DisplayString(2, 2, 1, deathMsg);
	
	// Display prompt
	GLCD_SetBackColor(White);
	GLCD_DisplayString(19, 13, 0, prompt); 
	
	// Display Colorful borders
	uint8_t i;
	for(i = 0; i < 60; i++){
		GLCD_DisplayString(0, i, 0, empty);
		GLCD_DisplayString(29,i, 0, empty);
	}
	for(i = 0; i < 30; i++){
		GLCD_DisplayString(i, 0, 0, empty);
		GLCD_DisplayString(i,52, 0, empty);
	}
	
	
	// Stay in game over state until button is pressed
	uint8_t buttonPressed = !((LPC_GPIO2->FIOPIN >> BUTTON) & 0x01);
	while(!buttonPressed)
		buttonPressed = !((LPC_GPIO2->FIOPIN >> BUTTON) & 0x01);
	
	initGame(GameState); 
	
}

void displayLifeLost(GameState_t *GameState) {
	unsigned char lifeLostMsg[] = "COLLISION - PRESS TO KEEP GOING";
	unsigned char clearStr[]    = "                               ";  
	// Display lifeLostMsg 
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Magenta);
	
	GLCD_DisplayString(19, 13, 0, lifeLostMsg); 
	
	// Stay in life lost state until button is pressed
	uint8_t buttonPressed = !((LPC_GPIO2->FIOPIN >> BUTTON) & 0x01);
	while(!buttonPressed)
		buttonPressed = !((LPC_GPIO2->FIOPIN >> BUTTON) & 0x01);
	
	// Clear text
	GLCD_SetBackColor(White);
	GLCD_SetTextColor(White);
	GLCD_DisplayString(19, 13, 0, clearStr); 
	
	// Set lifeLost flag back to false
	GameState->lifeLost = false;
}

/*##########################
  ##		 GAME OBJECTS 		##
	##########################*/

void gooseUpdate(GameState_t *GameState){
		if(!GameState->mainMenu){		
			if (GameState->goose.direction == GOOSE_UP)
				GameState->goose.ypos -= 3;

			else 
				GameState->goose.ypos += 5;
			
			osMutexWait(LCDMutexId, osWaitForever);
			GLCD_Bitmap(GameState->goose.xpos,GameState->goose.ypos, 34, 38, (unsigned char*)GOOSEMAP);
			osMutexRelease(LCDMutexId); 
			
			osDelay(100);
		}
} 

void getNewObstacle(GameState_t *GameState){
	//the obstacle is either 10 or 20 pixels tall
	GameState->obstacle.height=(rand()%2+1)*20;
}
//Set the landscape to height of 0 (blank landscape)
void initLandscape(GameState_t *GameState){
	GameState_t *Gooserino = GameState;	
	
	//Keep track of the progress of the landscape
	Gooserino->landscape.progress=0;
	
		for (int x = 0;x<BAR_NUM;x++){
			Gooserino->landscape.landscapeArray[0][x]=0;
			Gooserino->landscape.landscapeArray[1][x]=0;
		}
	//Set the obstacle x to max
	Gooserino->obstacle.xpos=320;
	Gooserino->obstacle.ypos=120;
	Gooserino->obstacle.height=(rand()%2+1)*20;
}

void landscapeUpdate(GameState_t *GameState){
	GameState_t *Gooserino=GameState;
	
	// Increment Game Score and copy into string buffer
	Gooserino->score++;	
	char buffer[4];
	snprintf(buffer, 4, "%d", Gooserino->score);	
	GLCD_SetTextColor(Black); 
	
	// Display Game Score
	osMutexWait(LCDMutexId, osWaitForever);
	GLCD_DisplayString(1, 50, 0, (unsigned char*)buffer);
	osMutexRelease(LCDMutexId); 
	
	// Increment difficulty based on score
	if(Gooserino->score > 50) {
		Gooserino->difficultyLevel = MED; 
	}
	if(Gooserino->score > 100) {
		Gooserino->difficultyLevel = HARD;
	}	
	
	
	//Variables for drawing the landscape
	uint16_t xCoord,yCoordUpper,yCoordLower, heightUpper,heightLower, width;
	
	//display the landscape bars
	for (int x=0;x<BAR_NUM;x++){
		//calculate the appropriate parameters
		xCoord=(x>0)*(BAR_WIDTH-Gooserino->landscape.progress)+(x>1)*BAR_WIDTH*(x-1);
		yCoordUpper=20;
		yCoordLower=SCREEN_HEIGHT-(Gooserino->landscape.landscapeArray[1][x])*10;
		
		width=((x==0)*(BAR_WIDTH-Gooserino->landscape.progress)+(x>0)*(x<(BAR_NUM-1))*BAR_WIDTH+(x==(BAR_NUM-1))*(Gooserino->landscape.progress));
		heightUpper=(Gooserino->landscape.landscapeArray[0][x])*10;
		heightLower=(Gooserino->landscape.landscapeArray[1][x])*10;
		
		//draw the top and bottom landscape
		drawOneBar(xCoord,yCoordUpper,width,heightUpper,Gooserino->difficultyLevel);
		drawOneBar(xCoord,yCoordLower,width,heightLower,Gooserino->difficultyLevel);
	}
	
	//display the obstacle
	drawOneBar(Gooserino->obstacle.xpos,Gooserino->obstacle.ypos,BAR_WIDTH/2,Gooserino->obstacle.height,OBS_COLOUR);
	
	osDelay((Gooserino->difficultyLevel==2)*3000+(Gooserino->difficultyLevel==1)*1500);
	//erase the landscape
	for (int x=1;x<BAR_NUM;x++){
		//calculate the corresponding white bar to draw
		xCoord=(x>0)*(BAR_WIDTH-Gooserino->landscape.progress)+(x>1)*BAR_WIDTH*(x-1)-PROGRESS_STEP;
		yCoordUpper=(Gooserino->landscape.landscapeArray[0][x])*10+20;
		yCoordLower=SCREEN_HEIGHT-(Gooserino->landscape.landscapeArray[1][x-1])*10;
		
		width=PROGRESS_STEP;
		heightUpper=(Gooserino->landscape.landscapeArray[0][x-1]-Gooserino->landscape.landscapeArray[0][x])*10;
		heightLower=(Gooserino->landscape.landscapeArray[1][x-1]-Gooserino->landscape.landscapeArray[1][x])*10;

		//if upper bars need erasing
		if (Gooserino->landscape.landscapeArray[0][x]<Gooserino->landscape.landscapeArray[0][x-1]){
			drawOneBar(xCoord,yCoordUpper,width,heightUpper,CLEAR_PARAM);
		}
		//if lower bars need erasing
		if (Gooserino->landscape.landscapeArray[1][x]<Gooserino->landscape.landscapeArray[1][x-1]){
		drawOneBar(xCoord,yCoordLower,width,heightLower,CLEAR_PARAM);
		}
	}
	//Erase the obstacle trail
	drawOneBar(Gooserino->obstacle.xpos+(BAR_WIDTH/2-PROGRESS_STEP),Gooserino->obstacle.ypos,PROGRESS_STEP,Gooserino->obstacle.height,CLEAR_PARAM);
	
	// Increment the landscape and obstacle progress
	Gooserino->landscape.progress+=PROGRESS_STEP;
	Gooserino->obstacle.xpos-=PROGRESS_STEP;
	
	// if the landscape progress reaches the limit
	if (Gooserino->landscape.progress==PROGRESS_LIMIT){
		// if we are going to add a fresh bar at the end, remove the front bar
		addOneBar(Gooserino);
		Gooserino->landscape.progress=0;
	}
	//if the obstacle progress is at the limit
	if (Gooserino->obstacle.xpos<(PROGRESS_STEP)){
		//clear the obstacle 
		osMutexWait(LCDMutexId, osWaitForever);
		GLCD_Bargraph(0,Gooserino->obstacle.ypos,BAR_WIDTH,Gooserino->obstacle.height,CLEAR_PARAM);
		osMutexRelease(LCDMutexId);
		
		Gooserino->obstacle.xpos=SCREEN_WIDTH;
		//Get a new obstacle
		getNewObstacle(Gooserino);
	}
}

//this is invoked is the progress reaches the limit
void addOneBar(GameState_t *GameState){
	GameState_t *Gooserino = GameState;	
	//Copy over the values 
	for (int x=0;x<BAR_NUM-1;x++){
		Gooserino->landscape.landscapeArray[0][x] = Gooserino->landscape.landscapeArray[0][x+1];
		Gooserino->landscape.landscapeArray[1][x] = Gooserino->landscape.landscapeArray[1][x+1];
	}
	Gooserino->landscape.landscapeArray[0][BAR_NUM-1]=returnRandom(4);
	Gooserino->landscape.landscapeArray[1][BAR_NUM-1]=returnRandom(4);
}

void drawOneBar(int x, int y, int width, int height, int level){
	//Draw bar with different color depending on the level
	switch(level){
		case EASY:
			GLCD_SetBackColor(Green);
			break;
		case MED:
			GLCD_SetBackColor(Yellow);
			break;
		case HARD:
			GLCD_SetBackColor(Red);
			break;
		case CLEAR_PARAM:
			GLCD_SetBackColor(White);
			break;
		default:
			GLCD_SetBackColor(Black);
	}
	if (width>0&&height>0){
		osMutexWait(LCDMutexId, osWaitForever);
		GLCD_Bargraph(x,y,width,height,1);
		osMutexRelease(LCDMutexId);

	}
}

int returnRandom(int limit){
	return rand()%(limit)+1;
}
