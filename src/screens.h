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

void displayMainScreen();
void displayGameScreen(Ship *ship1, Ship *ship2, Model water_model, RenderTexture screenShip1, RenderTexture screenShip2);
void displayGameOverScreen();
void displayOptionsScreen();
void displayAboutScreen();

#endif //SCREENS_H
