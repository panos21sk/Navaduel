#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"
#include "ship.h"
#define WIDTH 1280
#define HEIGHT 700
#define CAMERA_DISTANCE_VECTOR (Vector3){0.0f, 25.0f, 50.0f}

typedef enum
{
    MAIN, //entry screen
    OPTIONS, //options
    GAME, //main game screen
    GAME_OVER, //game over screen - when a ship gets destroyed
    ABOUT // credits and basic gameplay
} screen;

extern screen current_screen;
extern Rectangle play_button;

void InitMainWindow();
void DeinitMainWindow();
void DisplayMainScreen();
void DisplayGameScreen(Ship *ship1, Ship *ship2, Model water_model);
void DisplayGameOverScreen();
void DisplayOptionsScreen();
void DisplayAboutScreen();

#endif //SCREENS_H
