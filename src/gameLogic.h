#include "GLCD.h"
#include <stdbool.h>

#ifndef _GAMELOGIC_H
#define _GAMELOGIC_H

// Game Levels
#define EASY 	   0
#define MED		   1
#define HARD 	   2

// Background Colors
#define BKGRD_COLOR White
#define EASY_BKGRD Green
#define MED_BKGRD  Orange
#define HARD_BKGRD Red

// Goose Directions
#define GOOSE_UP 	 0
#define GOOSE_DOWN 1

// Landscape Parameters
#define BAR_NUM 6
#define PROGRESS_LIMIT 64
#define BAR_WIDTH 64
#define PROGRESS_STEP 16
#define CLEAR_PARAM 3
#define OBS_COLOUR 4 

typedef struct {
	uint32_t xpos, ypos;
	uint8_t direction; 
} goose_t;

typedef struct {
	uint8_t progress;
	unsigned char landscapeArray[2][BAR_NUM];
} landscape_t; 


typedef struct {
	uint32_t xpos, ypos, height;
} obstacle_t;

typedef struct {
	bool mainMenu; 
	bool gameOver;
	bool collided; 
	bool lifeLost;
	uint8_t difficultyLevel;
	uint8_t lives;
	uint32_t score; 

	// Game objects 
	goose_t goose; 
	landscape_t landscape; 
	obstacle_t obstacle;
	
} GameState_t;

void initGame(GameState_t *GameState);
void displayLEDLives(GameState_t *GameState);
void collisionDetect(GameState_t *GameState);


#endif /* _GAMELOGIC_H */
