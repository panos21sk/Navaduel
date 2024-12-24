#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"
#include "ship.h"
//#include "stdbool.h" - Dont know whether the inclusion is implicit or even needed for the bool type
#define WIDTH 1280
#define HEIGHT 700

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

typedef struct {
    bool show_reticle;
    bool first_or_third_person_cam;
} setting;
extern setting settings;

void InitMainWindow();
void DeinitMainWindow();
void DisplayMainScreen();
void DisplayGameScreen(Ship *ship1, Ship *ship2, Model water_model, Model sky_model);
void DisplayGameOverScreen();
void DisplayOptionsScreen();
void DisplayAboutScreen();

#endif //SCREENS_H
