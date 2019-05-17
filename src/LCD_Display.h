#include "gameLogic.h"

#ifndef _LCD_DISPLAY_H
#define _LCD_DISPLAY_H

// Object Colors
#define W White
#define B Black
#define G DarkGrey
#define M Maroon
#define Y Yellow
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// Reduce Goose size for gameplay tolerance
#define GOOSE_XSIZE 34
#define GOOSE_YSIZE 37
#define OBSTACLE_XSIZE 25 
#define OBSTACLE_YSIZE 20

// Title Pages
void displayMainMenu(GameState_t *GameState);
void displayGameOver(GameState_t *GameState);
void displayLifeLost(GameState_t *GameState);

// Object creation and updates
void gooseUpdate(GameState_t *GameState);
void generateObstacle(GameState_t *GameState);
void landscapeUpdate(GameState_t *GameState);
void initLandscape(GameState_t *GameState);
void addOneBar(GameState_t *GameState);
void drawOneBar(int x,int y,int width,int height,int color);
void getNewObstacle(GameState_t *GameState);

// Helper function
int returnRandom(int limit);

#endif /* _LCD_DISPLAY_H */
